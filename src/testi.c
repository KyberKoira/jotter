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
	file_buffer = (char*) malloc(fsize + 1);
	fseek(fp, 0, SEEK_SET);
	fread(file_buffer, fsize, 1, fp);
	file_buffer[fsize] = '\n';
	
	fclose(fp);

	int i = 0;

}

int main(int argc, char* argv[])
{
	readFile(argv[1]);

	printf("%s",file_buffer);
}
