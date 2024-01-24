#include <ncurses.h>
#include <stdio.h>   
#include <unistd.h>
#include <limits.h>
#include <string.h>
#include <stdlib.h>

#include "editor_functions.c"

int main(int argc, char *argv[])
{
	int position = 0;
	int line = 1;
	int character = 2;
		
	// If no arguments, exit
	if (argc <= 0) 
	{
		return 0;	
	}

	char* file_buffer = readFile(argv[1]);

	printf("%s\n", file_buffer);

	position = lineCharToPos(line, character, file_buffer);
	printf("This is it %d\n", position);

	return 0;

}
