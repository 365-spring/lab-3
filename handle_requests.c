#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>



int findFirstWhiteSpace(char line[])
{
	// assume line has at least 1 whitespace in it
	int i = 0;
	for(; line[i] != ' '; i++);

	return i;
}
char* getHeaderName(char line[], int location_of_first_white_space)
{
	char* header = malloc(sizeof(char) * 1024);
	for(int i = 0; i < location_of_first_white_space; i++)
	{
		header[i] = line[i];
	}

	return header;
}

bool processGETRequest(char* get_request[], /* probably not needed for file descriptor */int size_)
{


	// while not done reading the get request

	for(int i = 0; i < size_; i++)
	{
		char line[1024];
		strcpy(line, get_request[i]);
		int j = findFirstWhiteSpace(line);
		char* header = getHeaderName(line, j);
		char message[] = "process and transmit";
		if(strcmp(header, "GET") == 0)
		{
			printf("%s GET", message);


		}
		else if(strcmp(header, "User-Agent:") == 0)
		{
			printf("%s User-Agent:", message);
		}
		else if(strcmp(header, "Accept:") == 0)
		{
			printf("%s Accept:", message);
		}
		else if(strcmp(header, "Accept-Encoding:") == 0)
		{
			printf("%s Accept-Encoding:", message);
		}
		else if(strcmp(header, "Host:") == 0)
		{
			printf("%s Host:", message);
		}
		else if(strcmp(header, "Connection:") == 0)
		{
			printf("%s Connection:", message);
		}
		else if(strcmp(header, "Proxy-Connection:") == 0)
		{
			printf("%s Proxy-Connection:", message);
		}
		else
		{
			printf("split request\n");
			// what to do about the headers that were send before this line?
			return 0;
		}
		printf("\n");
		free(header);
		//header[j+1] = '\0';
		//printf("%s", header);
	}
	return 1;
}
int main(int argc, char *argv[])
{

	char* get_request[3] = {"GET /index.html HTTP/1.1","Host: blue.cs.sonoma.edu", "bad stuff in here"};

	// could pass in file descriptor here
	bool success = processGETRequest(get_request, 3);
	if(success)
	{

	}
	else
	{
		printf("failed \n");
	}
	return 0;
	// replace the get_request array with storing data from buffer
	/*
    for each line in buffer separated by newline
        catagorize it as a certain type of header
        send it if it was catagorized correctly
        if the catagorization fails
            the request was a split request



    */
        

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
