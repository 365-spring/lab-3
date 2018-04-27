#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>


// put in header and send up to master
int findDot(char url[])
{
	int i = 0;
	for(; url[i] != '.'; i++);
	return i;
}
char* getFileExtension(char url[])
{
	// takes anything of the form "/image_.txt" and gets the file extension
	// assumes extension size <= 24 characters
	int location_of_dot = findDot(url);

	char* file_extension = malloc(sizeof(char) * 24);
	strcpy(file_extension, url + location_of_dot + 1);
	return file_extension;

}
bool validateFileExtension(char* file_extension)
{

	// takes anything of the form "txt" and validates the file extension
	if(strcmp(file_extension, "jpg")  == 0 ||
	   strcmp(file_extension, "png")  == 0 ||
	   strcmp(file_extension, "gif")  == 0 ||
	   strcmp(file_extension, "tiff") == 0 ||
	   strcmp(file_extension, "bin")  == 0 ||
	   strcmp(file_extension, "dat")  == 0 ||
	   strcmp(file_extension, "txt")  == 0)
		return 1;
	else
		return 0;

}
