#include <ncurses.h>
#include <stdio.h>   
#include <unistd.h>
#include <limits.h>
#include <string.h>
#include <stdlib.h>

char *line;
char input_char;
char *input_char_p = &input_char;
int debug = 0;
int eject = 0;

// Converts line/chararcter combo to position in buffer
int lineCharToPos(int line, int character, char* buffer)
{
	
	int position = 0;
	int search_line = 0;
	int search_character = 0;

	while(*(buffer + position) != '\0' && search_line != line && search_character != character)
	{
		position++;
		search_character++;
		if(*(buffer + position) == '\n')
		{
			search_line++;
			search_character = 0;
		}
	}

	return position;
}

void insertCharacter(char c, char* filebuffer, int position)
{
	char tmp_char;
	char previous_char;
	int new_amount_of_elements = 0;
	new_amount_of_elements = strlen(filebuffer) + 2;
	// Reallocate buffer (1 more character)
	char* tmp_buffer = filebuffer;
	printf("buffer %d\n",new_amount_of_elements);
	
	if(debug == 15){
		printw("%d",new_amount_of_elements);	
		eject = 1;
		return;

	}

	filebuffer = realloc(filebuffer, new_amount_of_elements);
	if(filebuffer == NULL)
	{
		filebuffer = "tmp_buffer";
		printf("%d", new_amount_of_elements);
		return;
	}
	int i = 0;

	// Get the character at current pos you wanna replace
	previous_char = filebuffer[position + i];
	// Insert by replacing
	filebuffer[position + i] = c;
	
	while((position + i) < new_amount_of_elements - 2){
		
		i++;
		//move rest forward until EOF
		tmp_char = *(filebuffer + position + i);
		*(filebuffer + position + i) = previous_char;
		previous_char = tmp_char;

	}
	
	// Ought to be terminated
	filebuffer[new_amount_of_elements - 1] = '\0';
}

void concatenate_string(char* s, char* s1)
{
    int i;
 
    int j = strlen(s);
 
    for (i = 0; s1[i] != '\0'; i++) {
        s[i + j] = s1[i];
    }
 
    s[i + j] = '\0';
 
    return;
}

char* readFile(char* filepath)
{
	if(filepath[0] == '\0')
	{
		printf("No file name given!");
	}
	
	//Get current DIR
	char cwd[PATH_MAX];
	char *cwd_pointer = cwd;
	getcwd(cwd, sizeof(cwd));
	
	// Concatenate path to file, then open file
	char path_to_file[PATH_MAX + strlen(filepath)];
	strcpy(path_to_file, cwd_pointer);
	concatenate_string(path_to_file, "/");
	concatenate_string(path_to_file, filepath);
	
	printf("Opening file %s\n", path_to_file);
	
	//read characters into a buffer
	FILE *fp = fopen(path_to_file, "r");
	fseek(fp, 0, SEEK_END);
	long fsize = ftell(fp);
	char *file_buffer = (char*)malloc(fsize + 1);
	fseek(fp, 0, SEEK_SET);
	fread(file_buffer, fsize, 1, fp);
	file_buffer[fsize] = '\0';
	fclose(fp);
	
	//Need some variables for loop
	int characters = 0;
	int i = 0;
	int max_lines = 0;
	int *max_characters_per_line;

	//calculate characters until newline
	while (*(file_buffer + i) != '\0')
	{
		characters++;
		if(*(file_buffer + i) == '\n')
		{
			//Do realloc, if fails, use tmp
			// int *tmp = max_characters_per_line;
			// max_characters_per_line = realloc(max_characters_per_line, (max_lines + 1)*sizeof(int));
			// if(!max_characters_per_line)
			// {
			// 	max_characters_per_line = tmp;
			// }
			//
			// max_characters_per_line[max_lines] = characters;
			// max_lines++;
			// characters = 0;
		}

		i++;
	}

	return file_buffer;

}

void renderFile(int line, int character, char* file_buffer)
{
	//Resetting max_lines as file might have changed
	int render_line = 0;
	int render_character = 0;
	int reset_highlight = 0;

	//Clear window
	clear();
	
	int i = 0;

	//Render loop
	while (file_buffer[i] != '\0') 
	{
		
		if(render_line == line && render_character == character) // line is selected
		{
			attron(A_STANDOUT);
			reset_highlight = 1;
		}

		printw("%c",file_buffer[i]);
		render_character++;

		if (reset_highlight){
			attroff(A_STANDOUT);
			reset_highlight = 0;
		}
		
		// Lines only increment per newline
		if(file_buffer[i] == '\n') {
			render_character = 0;
			render_line++;
		}

		i++;
	}
	
	refresh();			/* Print it on to the real screen */

	return;

}

