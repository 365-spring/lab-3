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
        tv.tv_sec = 10;
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
    char buf[4096];

    exit_msg( (fd < 0) || (fd > FD_SETSIZE), "bad file descriptor");

    // Amount of time before connection timeout
    tv.tv_sec = 30; // In seconds
    tv.tv_usec = 0; // In microseconds


    /*
    for each line in buffer separated by newline
        catagorize it as a certain type of header
        send it if it was catagorized correctly
        if the catagorization fails
            the request was a split request



    */
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

        // If the client sends data, read it into the buffer until it
        // is done
        if ((len = read(fd, buf, sizeof(buf))) > 0);

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

        // String parsing

        struct addrinfo hints, *res;
        int sockfd;
        char buf2[2048];

        // Check that our url length is 2048 or lower
        if(len >= 2048)
        {
            char ERR[] = "Request is too long\n";
            write(fd, ERR,  strlen(ERR));
            FD_ZERO(&fds);
            return;
        }

        // Check first 11 letters.
        // If GET http://, continue
        // Else, stop as the string is bad (this will be changed
        // later)
        if(strncmp(buf, "GET http://", 11) != 0)
        {
            char ERR[] = "Error: not GET request\n";
            write(fd, ERR,  strlen(ERR));
            FD_ZERO(&fds);
            return;
        }

        // Initialize the 1st position variable after GET http://
        // (it will be positioned after the second slash)
        int pos1 = 11;
        int proxyPortNum = -1;

        // Initialze the 2nd position variable once either a colon or
        // forward slash is reached
        int pos2 = pos1;
        for(int i=pos2; i<len; i++)
        {
            if(buf[i] == ':')
            {
                pos2 = i;
                break;
            }

            // Error out if we reach this part and haven't found
            // a colon or we find a slash
            else if(buf[i] == '/' || i == len-1)
            {
                char err[] = "Error: malformed GET requests\n";
                write(fd, err, strlen(err));
                FD_ZERO(&fds);
                return;
            }
        }

        // Now that we have positions 1 and 2 of the GET request,
        // we can set the host name variable and address name
        // variable
        //char hostName[pos2-pos1];
        //for(int i=pos1; i<pos2; i++)
            //hostName[i-pos1] = buf[i];

        char addrName[pos2-pos1+4];
        memcpy(addrName, "www.", 4);
        for(int i=pos1; i<pos2; i++)
            addrName[i-pos1+4] = buf[i];

        int pos3 = pos2;

        // Get the port number
        char portStr[pos3-(pos2+1)];
        for(int i=pos2+1; i<len-1; i++)
        {
            if(buf[i] == '/')
            {
                pos3 = i;
                break;
            }

            // Error out if we reach this part and haven't found
            // a slash
            if(i == len-1)
            {
                char* err = "Error: malformed GET request\n";
                write(fd, err,  strlen(err));
                FD_ZERO(&fds);
                return;
            }
        }

        // Get the port number as a string from the buffer
        for(int i=pos2+1; i<pos3; i++)
            portStr[i] = buf[i];

        // Convert the port string to an integer
        proxyPortNum = atoi(portStr);

        // Get the shortened URL using position 3
        char shortURL[len-pos3+4];
        memcpy(shortURL, "GET ", 4);
        for(int i=pos3; i<len-1; i++)
            shortURL[i-pos3+4] = buf[i];

        //printf("%s", shortURL);

        // first, load up address structs with getaddrinfo():
        memset(&hints, 0, sizeof(hints));
        hints.ai_family = AF_UNSPEC;  // use IPv4 or IPv6, whichever
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_flags = AI_PASSIVE;     // fill in my IP for me

        getaddrinfo(addrName, "80", &hints, &res);
        //getaddrinfo("www.cs.sonoma.edu", "80", &hints, &res);

        // make a socket:
        sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

        // bind it to the port we passed in to getaddrinfo():
        bind(sockfd, res->ai_addr, res->ai_addrlen);

        // connect!
        connect(sockfd, res->ai_addr, res->ai_addrlen);

        // Send a message (get)
        // int send(int sockfd, const void *msg, int len, int flags);
        // GET http://cs.sonoma.edu:3333/index.html HTTP/1.1 Host: cs.sonoma.edu
        // Example input: "GET /index.html HTTP/1.1 Host: cs.sonoma.edu";
        send(sockfd, shortURL, strlen(shortURL), 0);
        //char *msg = "GET /index.html HTTP/1.1 Host: cs.sonoma.edu";
        //len = strlen(msg);
        //send(sockfd, msg, len, 0);
        recv(sockfd, buf2, sizeof(buf2), 0);
        close(sockfd);
        //printf("%s", buf2);
        
        write(fd, buf2,  sizeof(buf2));
        FD_ZERO(&fds);
        
    } while (0);
    return;
}