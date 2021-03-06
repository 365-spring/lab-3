#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <ctype.h>
#include "helper.h"
#include "binary_file_image_recognizer.h"

#define LISTEN_QUEUE 10

void handle_connection(int fd, int argc, char *argv[]);

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
    
    getaddrinfo(NULL, "8091", &hint, &aip);
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
    //timeout = 0;
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
                handle_connection(cfd, argc, argv);
                printf("%5d: Done.\n", getpid());
                exit(0);
            }
            // in parent
            close(cfd);
        }
        waitpid(-1, NULL, WNOHANG);
    } while (1);

    printf("%5d: Server Exiting\n", getpid());
    freeaddrinfo(aip);
    return 0;
}

void handle_connection(int fd, int argc, char *argv[])
{
    int len, rval;
    fd_set fds;
    struct timeval tv;
    char buf[1024];
    char* webPort = "www.blue.cs.sonoma.edu";

    exit_msg( (fd < 0) || (fd > FD_SETSIZE), "bad file descriptor");

    tv.tv_sec = 30;
    tv.tv_usec = 0;

    // read from client
    struct addrinfo hints, *res;
    int sockfd;
    char buf2[1024];

    // wait for a read
    FD_ZERO(&fds);
    FD_SET(fd, &fds);

    rval = select(fd+1, &fds, NULL, NULL, &tv);
    exit_msg(rval < 0, "select() error");

    // first, load up address structs with getaddrinfo():
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;  // use IPv4 or IPv6, whichever
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;     // fill in my IP for me

    if(argc <= 1)
        getaddrinfo(webPort, "8090", &hints, &res);
    else
        getaddrinfo(webPort, argv[1], &hints, &res);

    // make a socket:
    sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

    // connect!
    if(connect(sockfd, res->ai_addr, res->ai_addrlen) == -1)
    {
        printf("Error: could not connect to remote server\n");
        FD_ZERO(&fds);
        return;
    }
    
    // do the read
    while ((len = read(fd, buf, sizeof(buf))) > 0){
        // do work
        printf("%s\n", buf);

        // Send a message (get)
        // int send(int sockfd, const void *msg, int len, int flags);
        // GET http://cs.sonoma.edu:3333/index.html HTTP/1.1 Host: cs.sonoma.edu
        // Example input: "GET /index.html HTTP/1.1 Host: cs.sonoma.edu";

        printf("Bytes sent = %ld\n", send(sockfd, buf, sizeof(buf), 0));

        int isReceived = recv(sockfd, buf2, sizeof(buf2), 0);

        if(isReceived == -1)
            printf("Message was not sent");
        else
            printf("Message was sent");
        
        
        write(fd, buf2,  sizeof(buf2));
    }
    
    close(sockfd);
    FD_ZERO(&fds);
    return;
}

