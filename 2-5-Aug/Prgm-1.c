/*
	Lab 2: August-5

	Question:

	A file contains a  list of commands, one per each line, with variable number of arguments.
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
	Okay, so here's the logic. We start from main. We read the file.
	We create a child process. We execute the child process via execvp().
	Parent is to now wait till child is done. Once the child is done, it
	continues.

		The End.
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
		printf("\nOnly two arguments can be taken! ./a.out FILENAME.\nExiting\n");
		return -1;
	}

	FILE *file_ptr;
	file_ptr = fopen(argv[1], "r");

	if (file_ptr == NULL){
		printf("\nFile Not Found!\nExiting\n");
		return -1;
	}

	char line[200];

	while ( fgets(line, 1000, file_ptr) != NULL ){
		char *args[20];
		char *token;
		char *command;
		int count = 0;

		printf("%s", line);

		token = strtok(line, " ");

		while (token != NULL){
			printf("%s ", token);
			if (count == 0){
				command = token;
				args[count] = token;
			}
			else
				args[count] = token;
			count += 1;
			token = strtok(NULL, " ");
		}
		args[count] = NULL;

		for (int i = 0; i < count; ++i)
			printf("%s ", args[count]);

		pid_t child_pid = fork();

		if (child_pid == 0)
			execvp(command, args);
		else
			wait(NULL);
	}

	fclose(file_ptr);
	return 0;
}
