#include <ncurses.h>
#include <stdio.h>   
#include <unistd.h>
#include <limits.h>
#include <string.h>

char *line;
char input[255];
char cwd[PATH_MAX];
char *cwd_pointer = cwd;
int max_lines;

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

void renderFile(int line, int character, char* filepath)
{
	// Concatenate path to file, then open file
	char path_to_file[PATH_MAX + strlen(filepath)];
	strcpy(path_to_file, cwd_pointer);
	concatenate_string(path_to_file, "/");
	concatenate_string(path_to_file, filepath);
	FILE* fp = fopen(path_to_file, "r");
	
	//Resetting max_lines as file might have changed
	max_lines = 0;
	
	//Clear window
	clear();

	if(fp != NULL) 
	{
		while (fgets(input, sizeof(input), fp) != NULL) 
		{
			if(input[0] != '\n')
			{
				max_lines++;
				printw("%s\n",input);	
			}
		}
		fclose(fp);
	}
	
	refresh();			/* Print it on to the real screen */

}

int main(int argc, char *argv[])
{
	int line = 1; // What line are you editing
	int character = 1; // What character are you editing
	int c;
	int eject = 0;

	// If no arguments, exit
	if (argc <= 0) 
	{
		return 0;	
	}

	//Get current DIR
	getcwd(cwd, sizeof(cwd));
	
	initscr();			/* Start curses mode 		*/
	raw();				/* Line buffering disabled	*/
	keypad(stdscr, TRUE);		/* We get F1, F2 etc..		*/
	noecho();			/* Don't echo() while we do getch */
	
	//First render
	renderFile(line, character, argv[1]);

	while(1)
	{
		//check inputs
		c = getch();
		switch(c)
		{	case KEY_UP:
				if(line != 1)
					--line;
				break;
			case KEY_DOWN:
				if(line != max_lines)
					++line;
				break;
			case 3:
				eject = 1;
				break;
			default:
				mvprintw(24, 0, "Charcter pressed is = %3d Hopefully it can be printed as '%c'", c, c);
				refresh();
				break;
		}
		
		if (eject) {
			break;
		}
		//render with new info
		renderFile(line, character, argv[1]);

	}	

	endwin();			/* End curses mode		  */
	
	return 0;
}  
