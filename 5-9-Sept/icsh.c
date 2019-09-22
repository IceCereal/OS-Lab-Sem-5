// IceCereal-Shell
// ICShell

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/utsname.h>
#include <sys/types.h>
#include <string.h>
#include <sys/wait.h>
#include "prgm1.h"

char home[BUFSIZE];

int main(int argc, char *argv[]){
	char username[BUFSIZE];
	getlogin_r(username, BUFSIZE);

	struct utsname uname_data;
	uname(&uname_data);
	char *nodename = uname_data.nodename;

	getcwd(home, BUFSIZE);

	char *inputLine;
	char **args;
	int statusFlag = 1;

	do{
		printf("%s@%s:~%s/ ", username, nodename, disp_dir());

		inputLine = icsh_getline();
		args = icsh_parse_line(inputLine);
		statusFlag = icsh_execute_input(args);

	} while(statusFlag);

	free(inputLine);
	free(args);

	return 0;
}

char *disp_dir(){
	char cwd[BUFSIZE];
	getcwd(cwd, BUFSIZE);

	int dir_Diff = strlen(cwd) - strlen(home);

	char *cur_Dir;
	cur_Dir = &home[strlen(home)];

	if (dir_Diff > 0){
		cur_Dir = &cwd[strlen(home)];
	} else if (dir_Diff < 0){
		//dir = &home[strlen(home)+dir_Diff];
		// TODO: FINISH
	}

	return cur_Dir;
}

char *icsh_getline(){
	char *line = NULL;
	size_t buffersize = 0;
	getline(&line, &buffersize, stdin);
	return line;
}

char **icsh_parse_line(char *line){
	int totalArgs = 0;
	int max_argLen = 0;

	char *command = (char *)malloc(strlen(line) * sizeof(char));
	char *lineTemp = (char *)malloc(strlen(line) * sizeof(char));
	strcpy(command, line);
	strcpy(lineTemp, line);

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

	free(command);
	free(lineTemp);

	return token;
}