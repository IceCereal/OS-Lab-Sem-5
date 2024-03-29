/*
	Lab 2: August-5

	Question:

	A file contains a list of commands, one per each line, with variable number of arguments.
	Write a C program to read each command and its arguments and execute them.
	The program terminates when all the commands are completed.

	E.g., Sample Commands in the file:

	ls -al
	wc prog.c (replace with your program name)
	ping www.google.com
	whoami
	touch newfile.c
	cp prog.c newfile.c
	diff prog.c newfile.c
	rm prog.c
	gcc newfile.c -o newfile.out
*/
/*
	Reading the file part:
		credit: https://gist.github.com/Rochan-A/d47c3f7abd3f90774f1d06a1ec1903b3
*/
/*
	Okay, so here's the logic. We start from main. We read the file.
	We create a child process. We execute the child process via execvp().
	Parent is to now wait till child is done. Once the child is done, it
	continues.

		The End.
*/

/*
	Note: There should be a `\n`(newline, not literally) at the end of the file:
		FILE_NAME
		ls -al
		ping
		whoami
		
		END
	for the program to work
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char *argv[]){
	// CLI:
	// argv[1] - FILENAME

	if (argc != 2){
		printf("Only two arguments can be taken! Usage: ./a.out Path_To_File\nExiting\n");
		return -1;
	}

	FILE *file_ptr;
	file_ptr = fopen(argv[1], "r");

	if (file_ptr == NULL){
		printf("\nFile Not Found!\nExiting\n");
		return -1;
	}
	
	// Count the number of commands in the file
	int count_lines = 0;
	char ch;
	const char EOL = '\n';

	ch = getc(file_ptr);
	while (ch != EOF)
	{
		if (ch == EOL)
		{
			count_lines = count_lines + 1;
		}
		ch = getc(file_ptr);
	}
	fseek(file_ptr, 0, SEEK_SET);

	// Allocate memory for the first read
	char **buffer = (char **)malloc(count_lines*sizeof(char *));
	for(int i = 0; i < count_lines; i++){
		buffer[i] = (char *)malloc(100*sizeof(char));
	}

	// Allocate memory for the tokens
	char ***token = (char ***)malloc(count_lines*sizeof(char **));
	for(int i = 0; i < count_lines; i++){
		token[i] = (char **)malloc(20*sizeof(char *));
		for(int j = 0; j < 20; j++){
			token[i][j] = (char *)malloc(20*sizeof(char));
		}
	}

	// Read the file
	int i = 0;
	ch = getc(file_ptr);
	while(ch != EOF){
		if(i != 0){
			ch = fgetc(file_ptr);
		}

		// Save the line
		int p = 0;
		while (ch != EOL)
		{
			buffer[i][p++] = ch;
			ch = fgetc(file_ptr);
		}

		// Tokenize the line
		char* tok = strtok(buffer[i], " ");
		int pos = 0;
		while (tok != NULL) {
			token[i][pos++] = tok;
			tok = strtok(NULL, " ");
		}
		while (pos != 20){
			token[i][pos++] = NULL;
		}
		
		i++;

		ch = fgetc(file_ptr);

		// Break if end of file is reached
		if(ch == EOF){
			break;
		}
		else{
			fseek(file_ptr, -1, SEEK_CUR);
		}
	}

	fclose(file_ptr);

	// Begin Executing
	for (int i = 0; i < count_lines; ++i){
		pid_t child_pid = fork();

		if (child_pid == 0){
			execvp(token[i][0], token[i]);
			return 0;
		}
		else
			wait(NULL);
	}

	free(token);
	free(buffer);

	return 0;
}
