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
	struct sockaddr_in addr;
    fd_set fds;
    struct timeval tv;

	addr.sin_family = AF_INET;
	addr.sin_port = htons(8090);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);

    lfd = socket(addr.sin_family, SOCK_STREAM, 0);
	exit_msg(lfd < 0, "socket failed");

	rval = bind(lfd, (struct sockaddr*) &addr, sizeof(addr));
    exit_msg(rval < 0, "could not bind");

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
    return 0;
}

void handle_connection(int fd)
{
    int len, i;
    char buf[1024];

    exit_msg( (fd < 0) || (fd > FD_SETSIZE), "bad file descriptor");

    // do the read
    while ((len = read(fd, buf, sizeof(buf))) > 0) {
        // do work
        for(i = 0; i < len; i++) {
            buf[i] = toupper(buf[i]);
         }
        write(fd, buf, len);
    }
    return;
}

