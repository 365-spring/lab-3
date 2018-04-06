#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <ctype.h>
#include "helper.h"

#define LISTEN_QUEUE 10

void handle_connection(int fd);

int main(int argc, char *argv[])
{
    int lfd, cfd, pid, rval, timeout;
    struct addrinfo hint, *aip, *rp;
    fd_set fds;
    struct timeval tv;

	// 1 Create a listening socket for the server
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

	// 2 Detect read activity on the listen socket, e.g., using select()
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

		// 3 If there is activity, accept() the connection and fork()
        if (rval == 0) {
            timeout++;
        } else {
            // accept
            cfd = accept(lfd, NULL, NULL);
            exit_msg(cfd < 0, "accept() error");
            printf("%5d: Accepted connection\n", getpid());

			// 4 In the server:
			// nothing here
			/*
			Close the resources we do not need, like the connected fd the child will service
			Occasionally and opportunistically reap zombie children using waitpid()
			Go back to (2) and service more clients.

			get rid of cfd
			if too many zombie children
				reap them using waitpid()
			go back to top of loop

			*/

            if ((pid = fork()) == 0) {
	            // in child
				// Close the resources we don’t need, like the listen fd
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
    } while (timeout < 200);

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

    tv.tv_sec = 5;
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
		// Handle the client’s connection using read(), write(), fread(), fwrite(), etc.
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
