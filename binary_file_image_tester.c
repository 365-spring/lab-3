
#include "binary_file_image_recognizer.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <getopt.h>



int main(int argc, char *argv[])
{

	char url[] = "/image_.txt";
	// takes anything of the form "/something/image_.txt" and validates the file extension

	printf("file extension is %d\n", validateFileExtension(getFileExtension(url)));
	
	return 0;
}
