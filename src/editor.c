

int main(int argc, char *argv[])
{
	int line = 0; // What line are you editing
	int character = 0; // What character are you editing
	int c;
	int position; 
	
	// If no arguments, exit
	if (argc <= 0) 
	{
		return 0;	
	}

	//read file to memory
	char *file_buffer = readFile(argv[1]);
	
	initscr();			/* Start curses mode 		*/
	raw();				/* Line buffering disabled	*/
	keypad(stdscr, TRUE);		/* We get F1, F2 etc..		*/
	noecho();			/* Don't echo() while we do getch */
	
	renderFile(line, character, file_buffer);

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
				
				//reallocate memory
				position = lineCharToPos(line, character, file_buffer);
				insertCharacter(c, file_buffer, position);

		}

		
		if (eject) {
			break;
		}
		//render with new info
		renderFile(line, character, file_buffer);

	}	

	endwin();			/* End curses mode		  */
	//close the supid file

	return 0;
} 
