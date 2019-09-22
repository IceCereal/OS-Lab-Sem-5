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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char *argv[]){
	// CLI:
	// argv[1] - FILENAME

	if (argc != 2){
		printf("Error!\nFilename must be provided:\t./a.out PATH_TO_FILE\nreturning -1\n");
		return -1;
	}

	char filename[strlen(argv[1])];
	strcpy(filename, argv[1]);

	int line_count = 0;
	int LINE_LENGTH = 256;

	while (1){
		pid_t child_pid = fork();

		if (child_pid == 0){
			// We get the current line
			FILE *file_ptr;
			file_ptr = fopen(filename, "r");

			if (file_ptr == NULL){
				printf("Error!\nFile:\t%s\tdoesn't exist\nreturning -1\n", filename);
				return -1;
			}

			char lineTemp[LINE_LENGTH];
			int local_line_count = 0;	// How many lines have been read this iter
			int breakFlag = 0;	// This is a flag for breaking out of the loop when the last
						// line has been reached

			// This while loop reads the n-th line of the file
			while (fgets(lineTemp, LINE_LENGTH, file_ptr)){
				if (strlen(lineTemp) == (LINE_LENGTH-1)){
					if (lineTemp[LINE_LENGTH-1] != '\n'){
						printf("Warning!\nLine Number:\t%d", local_line_count);
						printf("\thas more than LINE_LENGTH (max-length):\t%d\n", LINE_LENGTH);
						printf("skipping line | returning -1\n");
						return -1;
					}
				}

				if (local_line_count == line_count){
					line_count++; // Overall line count increases
					breakFlag = 1; // Don't break this iteration (it's inverted)
					break;
				}

				local_line_count++;
			}

			// No more lines
			if (!breakFlag){
				break;
			}
			
			fclose(file_ptr);

			char command[strlen(lineTemp)];
			strcpy(command, lineTemp);

			int totalArgs = 0;
			int max_argLen = 0;

			// Initial loop for figuring out the number of arguments and
			// max_length(argument)
			char* tok = strtok(lineTemp, " \n");
			while(tok != NULL){
				totalArgs++;

				int tempLen = strlen(tok);

				if (tempLen > max_argLen)
					max_argLen = tempLen;

				tok = strtok(NULL, " \n");
			}

			// Allocate space for the tokens
			char **token = (char **)malloc((totalArgs + 1) * sizeof(char *));
			for (int i = 0; i < totalArgs + 1; ++i)
				token[i] = (char *)malloc((max_argLen) * sizeof(char));

			int argv_counter = 0;
			tok = strtok(command, " \n");
			while (tok != NULL){
				strcpy(token[argv_counter++], tok);
				tok = strtok(NULL, " \n");
			}

			token[argv_counter] = NULL;

			// Now execute this
			// execvp(token[0], token);
			// Now, if this was successful, it wouldn't come here. But if it fails, we deallocate memory and kill this process

		}
	}

	return 0;
}