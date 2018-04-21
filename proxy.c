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
    int len, rval;
    fd_set fds;
    struct timeval tv;
    char buf[1024*8];
    char buf2[1024*8];

    exit_msg( (fd < 0) || (fd > FD_SETSIZE), "bad file descriptor");

    //tv.tv_sec = 5;
    // Amount of time before connection timeout
    tv.tv_sec = 60; // In seconds
    tv.tv_usec = 0; // In microseconds

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

        // do work
        // buf contains the string from the user

        /*
        http://blue.cs.sonoma.edu:3333

        GET http://www.cs.sonoma.edu/index.html HTTP/1.1
        Host: blue.cs.sonoma.edu

        GETADDRINFO
        blue.cs.sonoma.edu & 80 —> IP/port destination

        CONNECT to that destination
        and then WRITE/SEND it this:

        GET /index.html HTTP/1.1
        Host: blue.cs.sonoma.edu
        */

        struct addrinfo hints, *res;
        int sockfd;

        // first, load up address structs with getaddrinfo():

        memset(&hints, 0, sizeof hints);
        hints.ai_family = AF_UNSPEC;  // use IPv4 or IPv6, whichever
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_flags = AI_PASSIVE;     // fill in my IP for me

        /*
        int getaddrinfo(const char *node,     // e.g. "www.example.com" or IP
                        const char *service,  // e.g. "http" or port number
                        const struct addrinfo *hints,
                        struct addrinfo **res);
        */

        getaddrinfo("www.cs.sonoma.edu", "80", &hints, &res);

        // make a socket:
        sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

        // bind it to the port we passed in to getaddrinfo():
        bind(sockfd, res->ai_addr, res->ai_addrlen);

        // connect!
        connect(sockfd, res->ai_addr, res->ai_addrlen);

        // Send a message (get)
        // int send(int sockfd, const void *msg, int len, int flags);
        char *msg = "GET /index.html HTTP/1.1 Host: cs.sonoma.edu";
        len = strlen(msg);
        send(sockfd, msg, len, 0);
        recv(sockfd, buf, sizeof(buf), 0);
        printf("%s\n", buf);
        
        // do the read
        while ((len = read(fd, buf2, sizeof(buf2))) > 0) {
            write(fd, buf,  sizeof(buf));
        }
    } while (0);
    return;
}