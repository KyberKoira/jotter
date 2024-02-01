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

struct FileBuffer {
	int size;
	char* buffer;
	int max_lines;
	int* max_characters_per_line;
};
 
typedef struct FileBuffer FileBuffer;

void destroyHelpWindow(WINDOW *helpWindow)
{
	//Delete current window, if it exists
	delwin(helpWindow);
	wrefresh(helpWindow);
}

void renderHelpWindow(WINDOW *helpWindow, int line, int character, int position, int max_lines, int newEls, int c)
{	
	int height = 4;
	int width = 4;
	int starty = 2;
	int startx = 2;
	
	wclear(helpWindow);
	wmove(helpWindow, LINES/4, COLS/4);
	wborder(helpWindow, '.', '.', '.','.','.','.','.','.');
	
	// Print Line
	wmove(helpWindow, 1, 1);
	wprintw(helpWindow,"Current Line: %d\n", line);	

	// Print Character
	wmove(helpWindow, 2, 1);
	wprintw(helpWindow,"Current Character: %d\n", character);	

	// Print Position
	wmove(helpWindow, 3, 1);
	wprintw(helpWindow,"Current Position: %d\n", position);	

	// Print Position
	wmove(helpWindow, 4, 1);
	wprintw(helpWindow,"Max Lines: %d\n", max_lines);	

	// Print Position
	wmove(helpWindow, 5, 1);
	wprintw(helpWindow,"New Elements: %d\n", newEls);	

	// Print Position
	wmove(helpWindow, 6, 1);
	wprintw(helpWindow,"You Pressed: %d\n", c);	

	//wprintw(helpWindow, "FUCK");
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

FileBuffer calculateBufferLines(FileBuffer buffer)
{
	int i = 0;
	int characters = 0;
	int lines = 0;
	int *returnable = malloc(sizeof(int)); //one line atleast
	while(*(buffer.buffer + i) != '\0')
	{
		characters++;
		i++;
		returnable[lines] = characters;

		if(*(buffer.buffer+i) == '\n')
		{
			characters = 0;
			lines++;
			returnable = realloc(returnable, sizeof(int)*(lines + 1));
		}
	}

	buffer.max_characters_per_line = returnable;

	return buffer;
}

// Converts line/chararcter combo to position in buffer
int lineCharToPos(int line, int character, char* buffer)
{
	
	int search_line = 0;
	int search_character = 0;
	int position = 0;

	while(*(buffer + position) != '\0')
	{
		if(search_line == line && search_character == character)
		{
			break;
		}
		
		if(*(buffer + position) == '\n')
		{
			search_line++;
			search_character = 0;
		}
		else
		{
			search_character++;
		}
		
		position++;
	}

	return position;
}

FileBuffer insertCharacterEnd(char c, FileBuffer filebuffer, int position)
{
	char tmp_char;
	char previous_char;
	int new_amount_of_elements = 0;
	new_amount_of_elements = filebuffer.size + 1; // strlen doesnt find termination so add one for character and termination
	filebuffer.size = filebuffer.size + 1;	

	// Reallocate buffer (1 more character)
	char* tmp_buffer = filebuffer.buffer;
	//printw("elements %d", new_amount_of_elements);

	filebuffer.buffer = realloc(filebuffer.buffer, new_amount_of_elements);
	if(filebuffer.buffer == NULL)
	{
		filebuffer.buffer = "tmp_buffer";
		printf("%d", new_amount_of_elements);
		return filebuffer;
	}

	// Ought to be terminated
	filebuffer.buffer[new_amount_of_elements - 1] = '\0';
	filebuffer.buffer[new_amount_of_elements - 2] = c;

	return filebuffer;
}

FileBuffer removeCharacter(FileBuffer filebuffer, int position)
{
	char tmp_char;
	char previous_char;
	// Reallocate buffer (1 more character)
	char* tmp_buffer = filebuffer.buffer;

	filebuffer.buffer = realloc(filebuffer.buffer, filebuffer.size - 1);
	filebuffer.size--;

	if(filebuffer.buffer == NULL)
	{
		filebuffer.buffer = tmp_buffer;
		return filebuffer;
	}

	int i = 0;
	
	// Last character
	char lastchar = filebuffer.buffer[filebuffer.size - 1];

	// Ought to be terminated
	filebuffer.buffer[filebuffer.size - 1] = '\0';

	// Loop from deletion point and replace
	while(filebuffer.buffer[position + i] != '\0')
	{
		// Replace current with next
		filebuffer.buffer[position + i] = filebuffer.buffer[position + i + 1];
		i++;
	}

	// there should be two terminations, and this gets rid of the unnecessary one
	filebuffer.buffer[position + i - 1] = lastchar;

	return filebuffer;
}

FileBuffer insertCharacter(char c, FileBuffer filebuffer, int position)
{
	char tmp_char;
	char previous_char;
	// Reallocate buffer (1 more character)
	char* tmp_buffer = filebuffer.buffer;

	filebuffer.buffer = realloc(filebuffer.buffer, filebuffer.size + 1);
	filebuffer.size++;

	if(filebuffer.buffer == NULL)
	{
		filebuffer.buffer = "tmp_buffer";
		return filebuffer;
	}

	int i = 0;

	// Get the character at current pos you wanna replace
	previous_char = c;
	
	// Ought to be terminated
	filebuffer.buffer[filebuffer.size - 1] = '\0';

	//loop until end
	while(filebuffer.buffer[position + i] != '\0'){
		
		// Move rest forward until EOF

		// Get next character to be replaced to temp
		tmp_char = filebuffer.buffer[position + i];
		
		// Replace with previous
		filebuffer.buffer[position + i] = previous_char;

		// Temp is the new previous
		previous_char = tmp_char;

		i++;

	}

	// there should be two terminations, and this gets rid of the unnecessary one
	filebuffer.buffer[position + i] = previous_char;

	return filebuffer;
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

FileBuffer readFile(char* filepath)
{
	
	FileBuffer buff;

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

	buff.buffer = (char*)malloc(fsize + 1);
	buff.size = fsize + 1;

	fseek(fp, 0, SEEK_SET);
	fread(buff.buffer, fsize, 1, fp);
	buff.buffer[fsize] = '\0';
	fclose(fp);

	return buff;

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
		
		if(file_buffer[i] == '\n')
		{
			wprintw(window, " \n");

		}
		else
		{
			wprintw(window, "%c",file_buffer[i]);
		}
		render_character++;

		if (reset_highlight)
		{
			wattroff(window,A_STANDOUT);
			reset_highlight = 0;
		}
		
		// Lines only increment per newline
		if(file_buffer[i] == '\n') 
		{
			render_character = 0;
			render_line++;
		}

		i++;
	}
	
	wrefresh(window);			/* Print it on to the real screen */

	return;

}

FileBuffer calculateLines(FileBuffer buffer)
{
	int i = 0;
	int returnable = 0;
	
	while(*(buffer.buffer + i) != '\0')
	{
		i++;
		if(*(buffer.buffer+i) == '\n')
		{
			returnable++;
		}
	}
	
	buffer.max_lines = returnable;

	return buffer;
}
