#include <stdio.h>    
#include <unistd.h>
#include <limits.h>
#include <string.h>

char *line;
char input[255];
char cwd[PATH_MAX];
char *cwd_pointer = cwd;
 
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

int main(int argc, char *argv[]){
	getcwd(cwd, sizeof(cwd));

	if (argc <= 0) {
		return 0;	
	}

	for (int i = 0; i < argc; i++) {
		printf("%s\n", argv[i]);
	}
	
	// Add this shit to the path
	char path_to_file[PATH_MAX + strlen(argv[1])];
	strcpy(path_to_file, cwd_pointer);
	concatenate_string(path_to_file, "/");
	concatenate_string(path_to_file, argv[1]);
	
	printf("%s\n",path_to_file);

	FILE* fp = fopen(path_to_file, "r");
	
	if(fp != NULL) {
		// do something

		while (fgets(input, sizeof(input), fp) != NULL) {
			printf("%s\n",input);
		}
				
		fclose(fp);
	}

	return 0;
}  
