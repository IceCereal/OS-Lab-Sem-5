/*
	Lab 4: 2019-19-Aug
	
	History of commands

	Relevant topics/commands: files, pipes, fork, malloc, free and inter-process communication.
	Specification (To be implemented and shown in lab)
	Extend the program written for Lab task 2 to read multiple file names from the command prompt and to be able to read interactively from the user.
	Each file contains a list of commands, one per each line, with variable number of arguments.
	Write a C program to read each command and its arguments and execute them. Once the program doesn't have any more lines to read from a file, it will process the next file and so on.
	For this lab, you may assume that no files have overlapping commands.
	Once there are no more files to read, the program will move to an interactive mode and start reading user inputs.
	You are to implement the following two features.
	
	In the interactive mode, the program should prompt user for inputs as follows: "Please enter your command:"
	The following commands should be supported:
		1. When the user types command "HISTORY BRIEF", the program should list out all the commands (no need for file names) that were executed so far without the respective arguments. 
		The display should show one command per line.
		2. When the user types command "HISTORY FULL", the program should list all the commands so far with their respective arguments.
		Each command should have an INDEX number associated with it and it should be displayed as well.
		3. If the user types "EXEC <COMMAND_NAME>", your program should execute that command with its arguments.
		4. When if the user types "EXEC <COMMAND_INDEX_NUMBER>", your program should execute that command with its arguments.
		5. The program should exit when the user types a special command "STOP".
		6. The program should handle following border conditions:
			a. At least one file name should be supplied when the program starts.
			b. For unrecognized commands (not in history), the program should type appropriate message.
			c. Avoid memory leaks and segmentation faults. Don't allocate memory where not required and as far as possible free memory.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

int exec_file(char *fileName){
	// fileName

	FILE *file_ptr;
	file_ptr = fopen(fileName, "r");

	if (file_ptr == NULL){
		printf("\nFile Not Found!\nExiting\n");
		perror("fopen - bad file");
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

	// Begin Executing
	for (int i = 0; i < count_lines; ++i){
		pid_t child_pid = fork();

		if (child_pid == 0){
			execvp(token[i][0], token[i]);
			return 0;
		} else
			wait(NULL);
	}

	return 0;
}


int main(int argc, char *argv[]){
	// CLI
	// argv[0] ./a.out
	// argv[1-n] filename[1-n].txt

	printf("Starting Lab4...\n");
	printf("Question:\n\tWrite a program to execute a set of files given through CLI. Then, the ");
	printf("program enterse a interactive mode.\n\n");

	if (argc < 2){
		printf("Error:\tMain\nMore than 1 argumnet is required. ./a.out FILENAME[0] ... FILENAME[n-1]\nExiting:\t-1\n");
		return -1;
	}

	/* Step 1 - Read each file iteratively */
	int i;
	for (i = 1; i < argc; ++i){
		printf("Reading File:\t%s\n", argv[i]);
		if (exec_file(argv[i]) == -1){
			printf("Error:\tMain\nexec_file:\treturned an error\nExiting:\t-1");
			return -1;
		}
	}

	return 0;
}