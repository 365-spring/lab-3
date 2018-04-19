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
#include <stdio.h>

//
// Globals and constants
//
static int verbose = 0;
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

void client(int port_number)
{

    /*
    setup a connection to port_(client)

    read the get request line from buffer
    GET http://www.cs.sonoma.edu/index.html HTTP/1.1
    Host: www.cs.sonoma.edu
    ...
    */
    int cfd, pid, len, done;
    struct addrinfo hint, *aip, *rp;
    char buf[1024];

    // initialize variables
    pid = getpid();
    done = 0;
    memset((void *) &hint, 0, sizeof(hint));

    // hints will help addrinfo to populate addr in a specific way
    hint.ai_family = AF_UNSPEC;
    hint.ai_socktype = SOCK_STREAM;   // TCP info
    hint.ai_flags = AI_PASSIVE;       // use my IP address

    getaddrinfo(NULL, "8090", &hint, &aip);
    for (rp = aip; rp != NULL; rp = rp->ai_next) {
        cfd = socket(rp->ai_family,
                 rp->ai_socktype,
                 rp->ai_protocol);
        if (cfd == -1)
            continue;
        if (connect(cfd, rp->ai_addr, rp->ai_addrlen) != -1)
            break; // success!
        close(cfd);
    }
    exit_msg(rp == NULL, "Error trying to connect");
    //len = read(0, buf, sizeof(buf));
    char get[1024];
    //memset(get, '\0', 1024);

    char host[1024];
    //memset(host, '\0', 1024);

    char accept[1024];
    //memset(accept, '\0', 1024);

    char accept_encoding[1024];
    //memset(accept_encoding, '\0', 1024);

    char accept_language[1024];
    //memset(accept_language, '\0', 1024);

    char user_agent[1024];
    //memset(user_agent, '\0', 1024);

    for(int i = 0; i < 6; i++)
    {
        if (( len=read(0, buf, sizeof(buf)) ) > 0)
        {

            //printf("\n%d ", i);
            //fwrite(buf, len, 1, stdout);
            //printf("\n%s\n", buf);
            switch(i)
            {
                case 0:
                    memcpy(get, buf, 1024);
                    break;
                case 1:
                    memcpy(host, buf, 1024);

                    break;
                case 2:
                    memcpy(accept, buf, 1024);

                    break;
                case 3:
                    memcpy(accept_encoding, buf, 1024);

                    break;
                case 4:
                    memcpy(accept_language, buf, 1024);

                    break;
                case 5:
                    memcpy(user_agent, buf, 1024);

                    break;
                default:
                    break;

            }
            /*for(int i = 0; i < len; i++)
            {
                printf("%c", buf[i]);
            }*/
            memset(buf, '\0', 1024);
            //fflush(stdout);

            write(cfd, buf, len);
        }

    }
    printf("\n%s\n", get);
    printf("\n%s\n", host);
    printf("\n%s\n", accept);
    printf("\n%s\n", accept_encoding);
    printf("\n%s\n", accept_language);
    printf("\n%s\n", user_agent);
    printf("\n");

    // get from the third forward slash to \0 in get
    //int third_forward_slash = findThirdForwordSlash(get);

    //int null_char = findNullCharacter(get);

    char stripped_get[1024];
    memset(stripped_get, '\0', 1024);
    
    char get_word[] = "GET ";
    memcpy(stripped_get, get_word, 4);

    /*memcpy(stripped_get + 4,
           get          + third_forward_slash,
           null_char    - third_forward_slash);*/

    printf("\n%s\n", stripped_get);

    return 0;
    //fflush(stdout);

    //do {
        // prompt user for input
        //printf("%5d > ", pid);
        //fflush(stdout);
        /*
        get the url from the get request:
        GET /page_name.html HTTP/1.0
        Host: blue.cs.sonoma.edu:3333
        Accept: text/html, text/plain, text/sgml, text/css, application/xhtml+xml, *\/*;q=0.01
        Accept-Encoding: gzip, bzip2
        Accept-Language: en
        User-Agent: Lynx/2.8.9dev.4 libwww-FM/2.14 SSL-MM/1.4.1 OpenSSL/1.0.1k-fips

        get url from the request
        use getaddrinfo() to get the server from the url found

        */
        // buf is the get request
        //if ((len = read(0, buf, sizeof(buf))) > 0) {
        //    if (buf[0] == '.') done = 1;

            // send request to server
            //printf("%5d send to server: ", pid);
            //printf("send to server: ", pid);

            //fwrite(buf, len, 1, stdout);
            //fflush(stdout);

        //    write(cfd, buf, len);

            // wait for response
            /*
            if ((len = read(cfd, buf, sizeof(buf))) > 0) {
                printf("%5d got from server: ", pid);
                fwrite(buf, len, 1, stdout);
                fflush(stdout);
            }*/
        //}
    //} while (!done);
    freeaddrinfo(aip);
}
void server()
{
    /*
    setup a listenting socket to listen to www.cs.sonoma.edu


    setup a connection to www.cs.sonoma.edu(server)
    send a get request that looks like
    
    GET /index.html HTTP/1.1
    Host: www.cs.sonoma.edu
    ...
    */
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

            // only child sees 0
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
    /*
    send a get request that looks like
    
    GET /index.html HTTP/1.1
    Host: www.cs.sonoma.edu
    ...
    to www.cs.sonoma.edu
    */
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
        // Handle the client’sconnection(server's connection in this case)  using read(), write(), fread(), fwrite(), etc.
        // buf is the data from the client
        while ((len = read(fd, buf, sizeof(buf))) > 0) {
            // do work
            for(i = 0; i < len; i++) {
                buf[i] = toupper(buf[i]);
            }
            // send to webserver
            write(fd, buf, len);
        }
    } while (0);
}

void makeConnection(int cfd,
                    int pid,
                    int len,
                    int done,
                    struct addrinfo hint,
                    struct addrinfo* aip,
                    struct addrinfo* rp,
                    char buf[1024])
{
    //int cfd, pid, len, done;
    //struct addrinfo hint, *aip, *rp;
    //char buf[1024];

    // initialize variables
    pid = getpid();
    done = 0;
    memset((void *) &hint, 0, sizeof(hint));

    // hints will help addrinfo to populate addr in a specific way
    hint.ai_family = AF_UNSPEC;
    hint.ai_socktype = SOCK_STREAM;   // TCP info
    hint.ai_flags = AI_PASSIVE;       // use my IP address

    getaddrinfo(NULL, "8090", &hint, &aip);
    for (rp = aip; rp != NULL; rp = rp->ai_next) {
        cfd = socket(rp->ai_family,
                 rp->ai_socktype,
                 rp->ai_protocol);
        if (cfd == -1)
            continue;
        if (connect(cfd, rp->ai_addr, rp->ai_addrlen) != -1)
            break; // success!
        close(cfd);
    }
    exit_msg(rp == NULL, "Error trying to connect");
}
//
// Main
//
int main(int argc, char *argv[])
{
    int i, opt;
    char *port = NULL;
// /home/student/dtauraso/cs365/lab-3/webproxy-assignment
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
	// sprintf to convert cstring port to int
	char port_buf[1024];
	int port_;
    //printf("%s\n", argv[1]);
    //char* x[] = argv[1];
    sscanf(argv[1], "%d", &port_);
    // 0 = 48 ascii
    int offset = 48;
    int port_number = 0;
    //printf("%d\n", sizeof(argv[1]));
    // char is 2 bytes so the total size is double the number of characters we want to visit
    for(int i = 0; i < sizeof(argv[1]) / 2; i++)
    {
        //printf("%d", argv[1][i] - offset);
        port_number = 10 * port_number + (argv[1][i] - offset);
    }
    if(verbose)
        printf("\n%d\n", port_number);
    //printf("%d", argv[1][0]);
	/*for(int i = 0; i < argc; i++)
    {
		char character = argv[0][i];
		if(character >= '0' && character <= '9')
			sscanf(&argv[0][i], "%s %*s %d", port_buf, port_);
    }*/
    if(verbose)
    {
        printf("\ndone\n");
        printf("%d\n", port_);
    }
    
    fflush(stdout);
    if (argc - optind != 1) {
        usage(argv[0]);
    } else {
        port = argv[optind];
    }

    if (verbose) {
        printf("Using port [%s]\n", port); fflush(stdout);
    }

    server();
    client(port_);
    /*


    setup a connection to port_(client)

    read the get request line from buffer
    GET http://www.cs.sonoma.edu/index.html HTTP/1.1
    Host: www.cs.sonoma.edu
    ...


    setup a listenting socket to listen to www.cs.sonoma.edu


    setup a connection to www.cs.sonoma.edu(server)
    send a get request that looks like
    
    GET /index.html HTTP/1.1
    Host: www.cs.sonoma.edu
    ...


    wait?
    the listening socket should
    */




    /*
    1) start your proxy
        $ ./proxy 32128
    2) start at client (like wget) to make a request
        request to TO PROXY
        FOR resource http://www.cs.sonoma.edu/index.html
    
        $ env http_proxy=http//blue.cs.sonoma.edu:32128 wget https://www.cs.sonoma.edu/index.html

    3) from inside your proxy you will see
        GET http://www.cs.sonoma.edu/index.html HTTP/1.1
        Host: www.cs.sonoma.edu
        ...

        read request for http://www.cs.sonoma.edu/index.html
        open a connection to www.cs.sonoma.edu
        send a GET request that looks like:

        GET /index.html HTTP/1.1
        Host: www.cs.sonoma.edu
        ...

        wait for response
        any data you get as a response, send that back to the client

        [ wget ] -> [proxy] -> [webserver]
                            <-
                <-
    */

}
