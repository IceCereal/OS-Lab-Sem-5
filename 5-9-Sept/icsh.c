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