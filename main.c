#include <ncurses.h>
#include <stdio.h>   
#include <unistd.h>
#include <limits.h>
#include <string.h>
#include <stdlib.h>

char *line;
char input_char;
char *input_char_p = &input_char;
char cwd[PATH_MAX];
char *cwd_pointer = cwd;
int max_lines = 0;
int *max_characters_per_line;
FILE *fp;
char *file_buffer;

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

void readFile(char* filepath)
{
	//Get current DIR
	getcwd(cwd, sizeof(cwd));
	
	int characters = 0;

	// Concatenate path to file, then open file
	char path_to_file[PATH_MAX + strlen(filepath)];
	strcpy(path_to_file, cwd_pointer);
	concatenate_string(path_to_file, "/");
	concatenate_string(path_to_file, filepath);
	fp = fopen(path_to_file, "r");

	//read characters into a buffer
	fseek(fp, 0, SEEK_END);
	long fsize = ftell(fp);
	file_buffer = (char*)malloc(fsize + 1);
	fseek(fp, 0, SEEK_SET);
	fread(file_buffer, fsize, 1, fp);
	file_buffer[fsize] = '\0';
	
	fclose(fp);

	int i = 0;

	//calculate characters until newline
	while (*(file_buffer + i) != '\0')
	{
		characters++;
		if(*(file_buffer + i) == '\n')
		{
			// Do realloc, if fails, use tmp
			int *tmp = max_characters_per_line;
			max_characters_per_line = realloc(max_characters_per_line, (max_lines + 1)*sizeof(int));
			if(!max_characters_per_line)
			{
				max_characters_per_line = tmp;
			}

			max_characters_per_line[max_lines] = characters;
			max_lines++;
			characters = 0;
		}

		i++;
	}

	return;

}

void renderFile(int line, int character)
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

int main(int argc, char *argv[])
{
	int line = 0; // What line are you editing
	int character = 1; // What character are you editing
	int c;
	int eject = 0;
	
	// If no arguments, exit
	if (argc <= 0) 
	{
		return 0;	
	}

	//read file to memory
	readFile(argv[1]);
	
	initscr();			/* Start curses mode 		*/
	raw();				/* Line buffering disabled	*/
	keypad(stdscr, TRUE);		/* We get F1, F2 etc..		*/
	noecho();			/* Don't echo() while we do getch */
	
	renderFile(line, character);

	while(1)
	{
		//check inputs
		c = getch();
		switch(c)
		{	case KEY_UP:
				if(line != 0)
					--line;
				if(character >= max_characters_per_line[line])
					character = max_characters_per_line[line] - 1;
				break;
			case KEY_DOWN:
				if(line != max_lines - 1)
					++line;
				if(character >= max_characters_per_line[line])
					character = max_characters_per_line[line] - 1;
				break;
			case 3:
				eject = 1;
				break;
			case KEY_LEFT:
				if(character != 0)
					--character;
				break;
			case KEY_RIGHT:
				if(character != max_characters_per_line[line] - 2)
					++character;
				break;
			default:
				//mvprintw(24, 0, "Charcter pressed is = %3d Hopefully it can be printed as '%c'", c, c);
				
				//insert to file at position
				
				

				break;
		}
		
		if (eject) {
			break;
		}
		//render with new info
		renderFile(line, character);

	}	

	endwin();			/* End curses mode		  */
	//close the supid file

	return 0;
} 
