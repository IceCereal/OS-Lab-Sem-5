/*
	Lab 3: 21-Aug

	Question:
	Processing of commands with pipes and fork

	Write a program that will read a command from a user and execute them. The program should exit
	if the user enters a special command called "quit" or "exit". The program should use pipes
	to communicate between parent and child processes to implement the desired functionality.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>

int execute_process(char *readBuffer){
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
	printf("Starting Lab3...\n");
	printf("Question:\n\n\tWrite a program that will read a command from a user and execute them. The program should exit ");
	printf("if the user enters a special command called \"quit\" or \"exit\". The program should use pipes to communicate ");
	printf("between parent and child processes to implement the desired functionality.\n");

	int signal_sendPipe[2]; // This is a pipe for sending the user input from the parent
	int signal_ackPipe[2]; // This is a pipe to acknowledge from the child

	char quit_word[][10] = {
		"QUIT",
		"EXIT",
		"quit",
		"exit"
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
				if (execute_process(readBuffer) == -1){
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
