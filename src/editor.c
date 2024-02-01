#include <ncurses.h>
#include <stdio.h>   
#include <unistd.h>
#include <limits.h>
#include <string.h>
#include <stdlib.h>

#include "editor_functions.c"

int main(int argc, char *argv[])
{
	int line = 0; // What line are you editing
	int character = 0; // What character are you editing
	int c, currentX, currentY;
	int position = 0;
	int newEls, saved = 0;
	
	WINDOW *helpWindow;
	WINDOW *renderWindow;
	
	initscr();			/* Start curses mode 		*/
	refresh();
	raw();				/* Line buffering disabled	*/
	keypad(stdscr, TRUE);		/* We get F1, F2 etc..		*/
	noecho();			/* Don't echo() while we do getch */
	
	//create the fullscreen window
	renderWindow = newwin(LINES, COLS, 0, 0);
	helpWindow = newwin(LINES/4, COLS/4, LINES-LINES/3, COLS-COLS/3);
	
	// If no arguments, exit
	if (argc <= 0) 
	{
		return 0;	
	}
	
	//read file to memory
	FileBuffer file_buffer = readFile(argv[1]);
	file_buffer = calculateBufferLines(file_buffer);
	file_buffer = calculateLines(file_buffer);
	
	renderFile(line, character, file_buffer.buffer, renderWindow);
	renderHelpWindow(helpWindow, line, character, position, file_buffer.max_lines, newEls, c, saved);

	while(1)
	{
		//check inputs
		c = getch();
		switch(c)
		{	case KEY_UP:
				if(line != 0)
					--line;
				if(character >= file_buffer.max_characters_per_line[line])
					character = file_buffer.max_characters_per_line[line] - 1;
				break;
			case KEY_DOWN:
				if(line != file_buffer.max_lines - 1)
					++line;
				if(character >= file_buffer.max_characters_per_line[line])
					character = file_buffer.max_characters_per_line[line] - 1;
				break;
			case 3: // CTRL + C
				eject = 1;
				break;
			case 263: //Backspace
				file_buffer = removeCharacter(file_buffer, position - 1);
				character--;
				break;
			case KEY_LEFT:
				if(character != 0)
					--character;
				break;
			case KEY_RIGHT:
				if(character != file_buffer.max_characters_per_line[line] - 1)
					++character;
				break;
			case 19: // CTRL + S
				saved = saveFile(file_buffer);
				break;
			default:
				//reallocate memory
				position = lineCharToPos(line, character, file_buffer.buffer);
				file_buffer = insertCharacter(c, file_buffer, position);
					
				// Once character has been moved, cursor should move one forward
				character++;
		}
		
		if (eject) {
			break;
		}
		
		position = lineCharToPos(line, character, file_buffer.buffer);

		// Render with new info
		renderFile(line, character, file_buffer.buffer, renderWindow);

		// Render helpwindow
		renderHelpWindow(helpWindow, line, character, position, file_buffer.max_lines, newEls, c, saved);
		file_buffer = calculateBufferLines(file_buffer);
		file_buffer = calculateLines(file_buffer);

	}	

	endwin();			/* End curses mode		  */
	//close the supid file

	return 0;
} 
