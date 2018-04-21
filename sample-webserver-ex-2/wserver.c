#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <ctype.h>
#include <getopt.h>
#include <linux/limits.h>
#include "http_support.h"

//
// Globals and constants
//
const struct option long_opts[] = {
    {"help", no_argument, 0, 'h'},
    {"verbose", no_argument, 0, 'v'},
    {"port", required_argument, 0, 'p'},
    {"root", required_argument, 0, 'r'},
    {0, 0, 0, 0}
};

const char *opts_desc[] = {
    ", to display this usage message.",
    ", for verbose output.",
    " p1 [p2 p3 ...], to specify one or more ports (default is port " DEFAULT_PORT ").",
    " path, to specify the server root path (default is " DEFAULT_SERVER_ROOT ").",
};

//
// External variables
//
extern char *server_root;

//
// Function prototypes and functions
//
void handle_connection(int fd);

void usage(const char* arg)
{
    int i;
    printf("Usage: %s [OPTIONS], where\n", arg);
    printf("  OPTIONS:\n");
    for (i = 0; i < sizeof(long_opts) / sizeof(struct option) - 1; i++) {
        printf("\t-%c, --%s%s\n",
            long_opts[i].val, long_opts[i].name, opts_desc[i]);
    }
    exit(0);
}

//
// Main
//
int main(int argc, char *argv[])
{
    int i, j, lfd, cfd, pid, rval, verbose, opt, portsiz, lfds_max;
    struct addrinfo hint, *aip, *rp;
    fd_set fds, lfds;
    struct timeval tv;
    char realroot[PATH_MAX];
    char **ports = NULL, *tmp = NULL;

    // initialize variables
    memset((void *) &hint, 0, sizeof(hint));
    memset(realroot, 0, sizeof(realroot));
    server_root = DEFAULT_SERVER_ROOT;
    verbose = 0;
    portsiz = 5;
    FD_ZERO(&lfds);
    lfds_max = -1;

    ports = (char **) malloc(portsiz * sizeof(char *));
    exit_msg(ports == NULL, "malloc() error");
    for (i = 0; i < portsiz; i++)
        ports[i] = NULL;

    // parse arguments
    while( -1 != (opt = getopt_long(argc, argv, "p:r:hv", long_opts, &i)) ) {
        switch(opt) {
            case 'p':
                for (i = 0; i < portsiz; i++) {
                    if (ports[i] != NULL) continue;
                    ports[i] = optarg;
                    break;
                }
                if (i == portsiz) {
                    ports = realloc(ports, (portsiz + 1)*sizeof(char *));
                    exit_msg(ports == NULL, "realloc() error");
                    portsiz++;
                    ports[i] = optarg;
                }
                break;
            case 'r':
                // do some simple checks on server root
                server_root = optarg;
                tmp = realpath(server_root, realroot);
                exit_msg(tmp == NULL, "-r uses bad path");
                exit_msg(-1 == access(realroot, R_OK), "-r path unavailable");
                server_root = realroot;
                break;
            case 'h':
                free(ports);
                usage(argv[0]);
                break;
            case 'v':
                verbose = 1;
                break;
            default:
                fprintf(stderr, "Invalid option.\n");
                usage(argv[0]);
                break;
        }
    } // while

    // if no ports were specified, use the default
    if (ports[0] == NULL) {
        ports[0] = DEFAULT_PORT;
    }

    for (i = 0; i < portsiz; i++) {
        // skip empty entries
        if (ports[i] == NULL)
            continue;

        // hints will help addrinfo to populate addr in a specific way
        hint.ai_family = AF_UNSPEC;
        hint.ai_socktype = SOCK_STREAM;   // TCP info
        hint.ai_flags = AI_PASSIVE;       // use my IP address

        // getaddrinfo
        getaddrinfo(NULL, ports[i], &hint, &aip);
        for (rp = aip; rp != NULL; rp = rp->ai_next) {

            // socket
            lfd = socket(rp->ai_family,
                         rp->ai_socktype,
                         rp->ai_protocol);
            if (lfd == -1)
                continue;

            // setsockopt
            opt = 1;
			// SO_REUSEADDR is for developing support
            rval = setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
            exit_msg(rval < 0, "setsockopt() error");

            // bind
            if (bind(lfd, rp->ai_addr, rp->ai_addrlen) > -1) {
                // success -- add bound socket to list of listen fds
                FD_SET(lfd, &lfds);
                lfds_max = (lfd + 1 > lfds_max) ? lfd + 1 : lfds_max;
                break;
            }
            close(lfd);
        }
        exit_msg(rp == NULL, "could not bind");
        freeaddrinfo(aip);
    }
    // we are done with these, now
    free(ports);

    // listen on all bound sockets
    for(lfd = 0; lfd < lfds_max; lfd++) {
        if (FD_ISSET(lfd, &lfds)) {
            rval = listen(lfd, LISTEN_QUEUE);
            exit_msg(rval < 0, "listen() error");
        }
    }

    // wait for connection on any of our listen sockets
    do {
        // set timeout, for reaping children
        tv.tv_sec = CLIENT_TIMEOUT;
        tv.tv_usec = 0;

        // populate fdset for interesting listen fds
        FD_ZERO(&fds);
        for(i = 0; i < lfds_max; i++) {
            if (FD_ISSET(i, &lfds))
                FD_SET(i, &fds);
        }

        // wait for a connection to be ready
        rval = select(lfds_max, &fds, NULL, NULL, &tv);
        exit_msg(rval < 0, "select() error");

        // do an optimistic reap for children
        waitpid(-1, NULL, WNOHANG);

        // if we hit a timeout
        if (rval == 0)
            continue;

        // else, must be time to accept one or more connections
        for(i = 0; i < lfds_max; i++) {
            if (FD_ISSET(i, &fds)) {
                cfd = accept(i, NULL, NULL);
                exit_msg(cfd < 0, "accept() error");

                if ((pid = fork()) == 0) {
                // in child
                    // close all the resources we don't need
                    for (j = 0; j < lfds_max; j++) {
                        if (FD_ISSET(j, &lfds))
                            close(j);
                    }
                    // now, handle the connection
                    handle_connection(cfd);
                    exit(0);
                    break;
                }
                // in parent
                close(cfd);
            }
        }
    } while (1);

    // exiting -- close fds and free resources
    for(i = 0; i < lfds_max; i++) {
        if (FD_ISSET(i, &lfds))
            close(i);
    }
    if (verbose) {
    	fprintf(stderr, "exiting.\n");
    }
    return 0;
}

//
// The function to handle a client connection
//
void handle_connection(int fd)
{
    http_req req;
    FILE *rx, *tx;

    exit_msg((fd < 0) || (fd > FD_SETSIZE), "bad fd");

    // for streams with sockets, need one for read and one for write
    rx = fdopen(fd, "r");
    tx = fdopen(dup(fd), "w");

    init_req(&req);
    http_get_request(rx, &req);
    http_process_request(&req);
    http_response(tx, &req);

    shutdown(fileno(rx), SHUT_RDWR);
    fclose(rx);
    fclose(tx);
    free_req(&req);
    return;
}
