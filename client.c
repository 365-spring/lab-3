#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include "helper.h"

int findThirdForwordSlash(char get[])
{
	// assume get has 3 '/' in it
	int count = 0;
	for(int i = 0; i < strlen(get); i++)
	{
		if(get[i] == '/')
		{
			count++;
			if(count == 3)
				return i;
		}
	}
}
int findNullCharacter(char get[])
{
	int i = 0;
	for(; get[i] != '\0'; i++){}
	return i;
}
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
	int third_forward_slash = findThirdForwordSlash(get);

	int null_char = findNullCharacter(get);

	char stripped_get[1024];
	memset(stripped_get, '\0', 1024);
	
	char get_word[] = "GET ";
	memcpy(stripped_get, get_word, 4);

	memcpy(stripped_get + 4,
		   get 			+ third_forward_slash,
		   null_char 	- third_forward_slash);

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
    return 0;
}
