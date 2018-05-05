#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <string.h>




int main(int argc, char *argv[])
{

	char file[] = "GET /index.html HTTP/1.1\nHost: blue.cs.sonoma.edu\nAccept-Encoding: This is an enc4ding\nGET /index.html HTTP/1.1\nHost: blue.cs.sonoma.edu\nAccept-Encoding: This is an enc5ding\nGET /index.html HTTP/1.1\nHost: blue.cs.sonoma.edu\nAccept-Encoding: This is an enc6ding\nGET /index.html HTTP/1.1\nHost: blue.cs.sonoma.edu\nAccept-Encoding: This is an enc7ding\nGET /index.html HTTP/1.1\nHost: blue.cs.sonoma.edu\nAccept-Encoding: This is an enc8ding\nGET /index.html HTTP/1.1\nHost: blue.cs.sonoma.edu\nAccept-Encoding: This is an enc9ding\nGET /index.html HTTP/1.1\nHost: blue.cs.sonoma.edu\nAccept-Encoding: This is an enc10ding\nGET /index.html HTTP/1.1\nHost: blue.cs.sonoma.edu\nAccept-Encoding: This is an enco11ing\nGET /index.html HTTP/1.1\nHost: blue.cs.sonoma.edu\nAccept-Encoding: This is an encod12ng\nGET /index.html HTTP/1.1\nHost: blue.cs.sonoma.edu\nAccept-Encoding: This is an enco13ing\nGET /index.html HTTP/1.1\nHost: blue.cs.sonoma.edu\nAccept-Encoding: This is an encod14ng\nGET /index.html HTTP/1.1\nHost: blue.cs.sonoma.edu\nAccept-Encoding: This is an encod15g\nGET /index.html HTTP/1.1\nHost: blue.cs.sonoma.edu\nAccept-Encoding: This is an enco16ing\nGET /index.html HTTP/1.1\nHost: blue.cs.sonoma.edu\nAccept-Encoding: This is an enco17ing\nGET /index.html HTTP/1.1\nHost: blue.cs.sonoma.edu\nAccept-Encoding: This is an encod18ng\nGET /index.html HTTP/1.1\nHost: blue.cs.sonoma.edu\nAccept-Encoding: This is an encodi19g\nGET /index.html HTTP/1.1\nHost: blue.cs.sonoma.edu\nAccept-Encoding: This is an encod20ng\nGET /index.html HTTP/1.1\nHost: blue.cs.sonoma.edu\nAccept-Encoding: This is an encod21g\nGET /index.html HTTP/1.1\nHost: blue.cs.sonoma.edu\nAccept-Encoding: This is an enc22ing\nGET /index.html HTTP/1.1\nHost: blue.cs.sonoma.edu\nAccept-Encoding: This is an enc23ing\nGET /index.html HTTP/1.1\nHost: blue.cs.sonoma.edu\nAccept-Encoding: This is an enc24ing\nGET /index.html HTTP/1.1\nHost: blue.cs.sonoma.edu\nAccept-Encoding: This is an enc25ing\nGET /index.html HTTP/1.1\nHost: blue.cs.sonoma.edu\nAccept-Encoding: This is an enc26ing\nGET /index.html HTTP/1.1\nHost: blue.cs.sonoma.edu\nAccept-Encoding: This is an enc27ing\nGET /index.html HTTP/1.1\nHost: blue.cs.sonoma.edu\nAccept-Encoding: This is an enc28ing\nGET /index.html HTTP/1.1\nHost: blue.cs.sonoma.edu\nAccept-Encoding: This is an enc29ing\nGET /index.html HTTP/1.1\nHost: blue.cs.sonoma.edu\nAccept-Encoding: This is an enc30ing\nGET /index.html HTTP/1.1\nHost: blue.cs.sonoma.edu\nAccept-Encoding: This is an enc31ing\nGET /index.html HTTP/1.1\nHost: blue.cs.sonoma.edu\nAccept-Encoding: This is an enc32ing\nGET /index.html HTTP/1.1\nHost: blue.cs.sonoma.edu\nAccept-Encoding: This is an enc33ing\nGET /index.html HTTP/1.1\nHost: blue.cs.sonoma.edu\nAccept-Encoding: This is an enc34ing\nGET /index.html HTTP/1.1\nHost: blue.cs.sonoma.edu\nAccept-Encoding: This is an enc35ing\nGET /index.html HTTP/1.1\nHost: blue.cs.sonoma.edu\nAccept-Encoding: This is an enc36ing\nGET /index.html HTTP/1.1\nHost: blue.cs.sonoma.edu\nAccept-Encoding: This is an enc37ing\nGET /index.html HTTP/1.1\nHost: blue.cs.sonoma.edu\nAccept-Encoding: This is an enc38ing\nGET /index.html HTTP/1.1\nHost: blue.cs.sonoma.edu\nAccept-Encoding: This is an enc39ing\nGET /index.html HTTP/1.1\nHost: blue.cs.sonoma.edu\nAccept-Encoding: This is an enc40ing\nGET /index.html HTTP/1.1\nHost: blue.cs.sonoma.edu\nAccept-Encoding: This is an enc41ing\nGET /index.html HTTP/1.1\nHost: blue.cs.sonoma.edu\nAccept-Encoding: This is an enc42ing\nGET /index.html HTTP/1.1\nHost: blue.cs.sonoma.edu\nAccept-Encoding: This is an enc43ing\nGET /index.html HTTP/1.1\nHost: blue.cs.sonoma.edu\nAccept-Encoding: This is an enc44ing\nGET /index.html HTTP/1.1\nHost: blue.cs.sonoma.edu\nAccept-Encoding: This is an enc45ing\nGET /index.html HTTP/1.1\nHost: blue.cs.sonoma.edu\nAccept-Encoding: This is an enc46ing\nGET /index.html HTTP/1.1\nHost: blue.cs.sonoma.edu\nAccept-Encoding: This is an enc47ing\nGET /index.html HTTP/1.1\nHost: blue.cs.sonoma.edu\nAccept-Encoding: This is an enc48ing\nGET /index.html HTTP/1.1\nHost: blue.cs.sonoma.edu\nAccept-Encoding: This is an enc49ing\n";

	/*
	from i = 0 to len(request)
	fill up buffer
	send buffer
	i += 1024

	*/
	//printf("got here\n");
	//printf("%lu\n", sizeof(file) / 1024);
	//printf("%lu\n", sizeof(char));
	for(int i = 0; i < sizeof(file); i += 1024)
	{
		char* buffer = malloc(sizeof(char) * 1024);

		printf("%d\n", i);
		if(sizeof(&file + i) / 1024 > 0 )
		{
			//printf("last part file + i %s\n\n", &file[0] + i);
			strncpy(buffer, file + i, sizeof(&file + i));
		}
		else
		{
			//printf("file + i %s", &file[0] + i);

			strncpy(buffer, file + i, 1024);
		}
		printf("send buffer  %s\n\n", buffer);
		free(buffer);
	}
	
	return 0;
        

//GET http://blue.cs.sonoma.edu:3333/page_name.html HTTP/1.1
//User-Agent: Wget/1.18 (linux-gnu)
//Accept: */*
//Accept-Encoding: identity
//Host: blue.cs.sonoma.edu:3333
//Connection: Keep-Alive
//Proxy-Connection: Keep-Alive

/*
GET
User-Agent:
Accept:
Accept-Encoding:
Host:
Connection:
Proxy-Connection:
*/

       

}
