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