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
    int cfd, pid, len, rval, done;
	struct sockaddr_in addr;
    char buf[1024];

    // initialize variables
    pid = getpid();
    done = 0;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8090);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    cfd = socket(addr.sin_family, SOCK_STREAM, 0);
    exit_msg(cfd < 0, "Socket failed");

    rval = connect(cfd, (struct sockaddr *) &addr, sizeof(addr));
    exit_msg(rval < 0, "Error trying to connect");

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
                printf("%5d read: ", pid);
                fwrite(buf, len, 1, stdout);
                fflush(stdout);
            }
        }
    } while (!done);
    return 0;
}

