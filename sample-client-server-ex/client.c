#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include "helper.h"

int main(int argc, char *argv[])
{
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

    do {
        // prompt user for input
        printf("%5d > ", pid);
        fflush(stdout);

        if ((len = read(0, buf, sizeof(buf))) > 0) {
            if (buf[0] == '.') done = 1;

            // send request to server
            printf("%5d write: ", pid);
            fwrite(buf, len, 1, stdout);
            fflush(stdout);

            write(cfd, buf, len);

            // wait for response
            if ((len = read(cfd, buf, sizeof(buf))) > 0) {
                printf("%5d read: \n", pid);
                fwrite(buf, len, 1, stdout);
                fflush(stdout);
            }
        }
    } while (!done);
    freeaddrinfo(aip);
    return 0;
}

