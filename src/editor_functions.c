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

void destroyHelpWindow(WINDOW * helpWindow)
{
	//Delete current window, if it exists
	delwin(helpWindow);
	wrefresh(helpWindow);
}

void renderHelpWindow(WINDOW *helpWindow)
{	
	int height = 4;
	int width = 4;
	int starty = 2;
	int startx = 2;
	


	/* The parameters taken are 
	 * 1. win: the window on which to operate
	 * 2. ls: character to be used for the left side of the window 
	 * 3. rs: character to be used for the right side of the window 
	 * 4. ts: character to be used for the top side of the window 
	 * 5. bs: character to be used for the bottom side of the window 
	 * 6. tl: character to be used for the top left corner of the window 
	 * 7. tr: character to be used for the top right corner of the window 
	 * 8. bl: character to be used for the bottom left corner of the window 
	 * 9. br: character to be used for the bottom right corner of the window
	 */
	wrefresh(helpWindow);

}

void destroy_win(WINDOW *local_win)
{	
	/* box(local_win, ' ', ' '); : This won't produce the desired
	 * result of erasing the window. It will leave it's four corners 
	 * and so an ugly remnant of window. 
	 */
	delwin(local_win);
}

int *calculateBufferLines(char *buffer)
{
	int i = 0;
	int characters = 0;
	int lines = 0;
	int *returnable = malloc(sizeof(int)); //one line atleast
	while(*(buffer + i) != '\0')
	{
		characters++;
		i++;
		returnable[lines] = characters;

		if(*(buffer+i) == '\n')
		{
			characters = 0;
			lines++;
			returnable = realloc(returnable, sizeof(int)*lines);
		}
	}

	return returnable;
}

// Converts line/chararcter combo to position in buffer
int lineCharToPos(int line, int character, char* buffer)
{
	
	int position = 0;
	int search_line = 1;
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
	new_amount_of_elements = strlen(filebuffer) + 2; // strlen doesnt find termination so add one for character and termination
	// Reallocate buffer (1 more character)
	char* tmp_buffer = filebuffer;
	printf("buffer %d\n",new_amount_of_elements);
	
	printw("elements %d", new_amount_of_elements);

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

	return file_buffer;

}

void renderFile(int line, int character, char* file_buffer, WINDOW *window)
{
	//Resetting max_lines as file might have changed
	int render_line = 0;
	int render_character = 0;
	int reset_highlight = 0;

	//Clear window
	wclear(window);
	
	int i = 0;

	//Render loop
	while (file_buffer[i] != '\0') 
	{
		
		if(render_line == line && render_character == character) // line is selected
		{
			wattron(window,A_STANDOUT);
			reset_highlight = 1;
		}

		wprintw(window, "%c",file_buffer[i]);
		render_character++;

		if (reset_highlight){
			wattroff(window,A_STANDOUT);
			reset_highlight = 0;
		}
		
		// Lines only increment per newline
		if(file_buffer[i] == '\n') {
			render_character = 0;
			render_line++;
		}

		i++;
	}
	
	wrefresh(window);			/* Print it on to the real screen */

	return;

}

