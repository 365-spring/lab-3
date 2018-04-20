#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <ctype.h>
#include <getopt.h>
#include <time.h>

#define LISTEN_QUEUE 10

//
// Globals and constants
//
static int verbose = 1;
#define CRLF                "\r\n"
#define LISTEN_QUEUE        10
#define CLIENT_TIMEOUT      5
#define MESSAGE_BUFFER_LEN  2048

// Status codes from RFC 1945 (Section 6.1.1)
#define OK              200
#define CREATED         201
#define ACCEPTED        202
#define NO_CONTENT      204
#define MOVED_PERM      301
#define MOVED_TEMP      302
#define NOT_MODIFIED    304
#define BAD_REQUEST     400
#define UNAUTHORIZED    401
#define FORBIDDEN       403
#define NOT_FOUND       404
#define INTERNAL_ERROR  500
#define NOT_IMPLEMENTED 501
#define BAD_GATEWAY     502
#define UNAVAILABLE     503


const struct option long_opts[] = {
    {"help", no_argument, 0, 'h'},
    {"verbose", no_argument, 0, 'v'},
    {0, 0, 0, 0}
};

const char *opts_desc[] = {
    ", to display this usage message.",
    ", for verbose output.",
};

void usage(const char* arg)
{
    int i;
    printf("Usage: %s %s [OPTIONS], where\n", arg, "PORT");
    printf("  OPTIONS:\n");
    for (i = 0; i < sizeof(long_opts) / sizeof(struct option) - 1; i++) {
        printf("\t-%c, --%s%s\n",
            long_opts[i].val, long_opts[i].name, opts_desc[i]);
    }    
    exit(0);
}


//
// A simple function to print error and exit, if cond is true
//
void exit_msg(int cond, const char* msg)
{
    if (cond) {
        perror(msg);
        exit(-1);
    }
    return;
}

void handle_connection(int fd);


//
// Main
//
int main(int argc, char *argv[])
{
    int i, opt;
    char *port = NULL;
    

    // parse arguments
    while( -1 != (opt = getopt_long(argc, argv, "p:r:hv", long_opts, &i)) ) {
        switch(opt) {
            case 'h':
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

    fflush(stdout);
    if (argc - optind != 1) {
        usage(argv[0]);
    } else {
        port = argv[optind];
    }
    
    if (verbose) {
        printf("Using port [%s]\n", port); fflush(stdout);
    }


    // Start of server.c
    int lfd, cfd, pid, rval, timeout;
    struct addrinfo hint, *aip, *rp;
    fd_set fds;
    struct timeval tv;

    // initialize variables
    memset((void *) &hint, 0, sizeof(hint));

    // hints will help addrinfo to populate addr in a specific way
    hint.ai_family = AF_UNSPEC;
    hint.ai_socktype = SOCK_STREAM;   // TCP info
    hint.ai_flags = AI_PASSIVE;       // use my IP address
    
    getaddrinfo(NULL, "8090", &hint, &aip);
    for (rp = aip; rp != NULL; rp = rp->ai_next) {
        lfd = socket(rp->ai_family, 
                     rp->ai_socktype,
                     rp->ai_protocol);
        if (lfd == -1)
            continue;
        if (bind(lfd, rp->ai_addr, rp->ai_addrlen) != -1)
            break; // success!
        close(lfd);
    }
    exit_msg(rp == NULL, "could not bind");

    rval = listen(lfd, LISTEN_QUEUE);
    exit_msg(rval < 0, "listen() error");

    // wait for connection
    timeout = 0;
    do {
        // wait for a connection to be ready
        FD_ZERO(&fds);
        FD_SET(lfd, &fds);
        tv.tv_sec = 20;
        tv.tv_usec = 0;

        rval = select(lfd+1, &fds, NULL, NULL, &tv);
        exit_msg(rval < 0, "select() error");

        if (rval == 0) {
            timeout++;
        } else {
            // accept
            cfd = accept(lfd, NULL, NULL);
            exit_msg(cfd < 0, "accept() error");
            printf("%5d: Accepted connection\n", getpid());

            if ((pid = fork()) == 0) {
            // in child
                close(lfd);
                printf("%5d: Handling connection.\n", getpid());
                handle_connection(cfd);
                printf("%5d: Done.\n", getpid());
                exit(0);
            }
            // in parent
            close(cfd);
        }
        waitpid(-1, NULL, WNOHANG);
    } while (timeout < 5);

    printf("%5d: Server Exiting\n", getpid());
    freeaddrinfo(aip);
    return 0;

}

void handle_connection(int fd)
{
    int len, i, rval;
    fd_set fds;
    struct timeval tv;
    char buf[1024];

    exit_msg( (fd < 0) || (fd > FD_SETSIZE), "bad file descriptor");

    tv.tv_sec = 10;
    tv.tv_usec = 0;

    // read from client
    do {
        // wait for a read
        FD_ZERO(&fds);
        FD_SET(fd, &fds);

        rval = select(fd+1, &fds, NULL, NULL, &tv);
        exit_msg(rval < 0, "select() error");

        if (rval == 0) {
            fprintf(stderr, "connection timeout");
            return;
        }
        
        // do the read
        while ((len = read(fd, buf, sizeof(buf))) > 0) {
            // do work
            for(i = 0; i < len; i++) {
                buf[i] = toupper(buf[i]);
            }
            write(fd, buf, len);
        }
    } while (0);
    return;
}

