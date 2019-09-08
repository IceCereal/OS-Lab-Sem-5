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
#include <errno.h>

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

int exec_process(char *readBuffer){
	/* Util function to execute a process */

	char readBuffer_Copy[strlen(readBuffer)];
	strcpy(readBuffer_Copy, readBuffer);

	/* 1. Count number of words required as well as max word length */
	int wordCount = 0;

	char *tempWord = strtok(readBuffer, " \n");

	while (tempWord != NULL){
		++wordCount;
		tempWord = strtok(NULL, " \n");
	}

	// Make Arguments
	char *args[wordCount + 1];

	char *word = strtok(readBuffer_Copy, " \n");

	int i = 0;
	// Copy tokens to args
	while (word != NULL){
		args[i++] = word;
		word = strtok(NULL, " \n");
	}
	args[i] = NULL;

	/* 2. Execute le command via un fork */
	pid_t execPid;

	execPid = fork();
	if (execPid == -1){
		perror("Fork creation in execute_process failed!\nExiting\n");
		return -1;
	}

	if (execPid == 0){
		/* This is the child process */
		execvp(args[0], args);
		exit(errno);
	} else {
		/* This is the parent process */
		int returnStatus;
		waitpid(execPid, &returnStatus, 0);
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
	for (int i = 1; i < argc; ++i){
		printf("Reading File:\t%s\n", argv[i]);
		if (exec_file(argv[i]) == -1){
			printf("Error:\tMain\nexec_file:\treturned an error\nExiting:\t-1");
			return -1;
		}
	}

	/* Step 2 - Go to interactive mode */
	int signal_sendPipe[2]; // This is a pipe for sending the user input from the parent
	int signal_ackPipe[2]; // This is a pipe to acknowledge from the child

	char quit_word[][10] = {
		"STOP"
	};

	int loopProcess = 1; // The entire program loop is controlled by this variable

	/* Pipe_return_status */
	int signal_sendPipe_rs = pipe(signal_sendPipe);
	int signal_ackPipe_rs = pipe(signal_ackPipe);

	if (signal_sendPipe_rs == -1){
		perror("signal_sendPipe creation failed!\nExiting\n");
		return -1;
	}

	if (signal_ackPipe_rs == -1){
		perror("signal_ackPipe creation failed!\nExiting\n");
		return -1;
	}

	pid_t pid = fork();

	if (pid == -1){
		perror("Fork creation failed!\nExiting\n");
		return -1;
	}

	// Begin Loop
	while (loopProcess){

		if (pid == 0){
			/* This is the Child Process */
			raise(SIGSTOP); // Wait for Parent to give signal to continue

			char readBuffer[1024];

			close(signal_sendPipe[1]); // Close writing
			close(signal_ackPipe[0]); // Close reading

			if (read(signal_sendPipe[0], readBuffer, sizeof(readBuffer)) == -1){
				perror("Child Process:\tReading (readBuffer) from Pipeline in Child failed!\nReturning -1\n");
				return -1;
			}

			// printf("Child Process:\tRead:\t%s\n", readBuffer);

			char send_acknowledge[10];
			strcpy(send_acknowledge, "continue");

			for (int i = 0; i < sizeof(quit_word)/10; ++i){
				if (strncmp(quit_word[i], readBuffer, strlen(quit_word[i])) == 0){
					strcpy(send_acknowledge, "quit");
					loopProcess = 0;
				}
			}

			// Send Acknowledge
			if (write(signal_ackPipe[1], send_acknowledge, sizeof(send_acknowledge)) == -1){
				perror("Child Process:\tWriting (send_acknowledge: success) to Pipeline in Child failed!\nBreaking\n");
				return -1;
			}

			// printf("\nSending:\t%s", send_acknowledge);

			// strncmp evaluates to 0 if it's true.
			if (strncmp(send_acknowledge, "quit", 4) != 0){
				if (exec_process(readBuffer) == -1){
					perror("Child Process:\texecute_process returned -1.\nReturning -1\n");
					return -1;
				}
			}

		} else {
			char writeBuffer[1024];

			/* This is the Parent Process */
			waitpid(pid, NULL, WUNTRACED);

			close(signal_sendPipe[0]); // Close reading
			close(signal_ackPipe[1]); // Close writing

			// printf("\nParent Process:\tType Command:\t");
			printf("\nType Command:\t");

			fgets(writeBuffer, 1024, stdin);

			if (write(signal_sendPipe[1], writeBuffer, sizeof(writeBuffer)) == -1){
				perror("Parent Process:\tWriting (writeBuffer) to Pipeline in Parent failed!\nBreaking\n");
				kill(pid, SIGKILL);
				return -1;
			}

			kill(pid, SIGCONT);

			// Read Acknowledge
			char acknowledge[10];
			if (read(signal_ackPipe[0], acknowledge, sizeof(acknowledge)) == -1){
				perror("Parent Process:\tReading (acknowledge) from Pipeline in Parent failed!\nBreaking\n");
				kill(pid, SIGKILL);
				return -1;
			}

			if (strncmp(acknowledge, "quit", 4) == 0){
				printf("Parent Process:\tAcknowledge QUIT received!\n");
				kill(pid, SIGKILL);
				loopProcess = 0;
			}
		}

	}


	return 0;
}