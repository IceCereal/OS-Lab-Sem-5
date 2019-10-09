// IceCereal-Shell
// ICShell

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/utsname.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <sys/wait.h>

#include "icsh.h"

char home[BUFSIZE], file_pid_all[BUFSIZE], file_pid_current[BUFSIZE],
	file_history[BUFSIZE];

int main(int argc, char *argv[]){
	char username[BUFSIZE];
	getlogin_r(username, BUFSIZE);

	struct utsname uname_data;
	uname(&uname_data);
	char *nodename = uname_data.nodename;

	getcwd(home, BUFSIZE);

	strcpy(file_pid_all, "");
	strcpy(file_pid_current, "");
	strcpy(file_history, "");

	strcat(file_pid_all, home);
	strcat(file_pid_current, home);
	strcat(file_history, home);

	strcat(file_pid_all, "/._icsh_pid_all");
	strcat(file_pid_current, "/._icsh_pid_current");
	strcat(file_history, "/._icsh_history");

	char *inputLine;
	char **args;
	int statusFlag = 1;

	FILE *file_ptr_pid_all = fopen("._icsh_pid_all", "w");
	FILE *file_ptr_pid_cur = fopen("._icsh_pid_current", "w");
	FILE *file_ptr_hist = fopen("._icsh_history", "w");

	fclose(file_ptr_pid_all);
	fclose(file_ptr_pid_cur);
	fclose(file_ptr_hist);

	do{
		printf("%s@%s:~%s/ ", username, nodename, disp_dir());

		inputLine = icsh_getline();
		args = icsh_parse_line(inputLine);
		statusFlag = icsh_execute_input(args, inputLine);

	} while(statusFlag);

	free(inputLine);
	free(args);

	printf("Cleaning up...\n");

	icsh_clean_up();

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

	icsh_log_history(line);

	return line;
}

int icsh_log_history(char *line){
	FILE *file_ptr_hist = fopen(file_history, "a");

	fprintf(file_ptr_hist, line, strlen(line));

	fclose(file_ptr_hist);
	return 1;
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

int icsh_cd(char **);
int icsh_exit(char **);
int icsh_pid(char **);

char *icsh_builtin_str[] = {
	"cd",
	"exit",
	"pid",
	"hist",
	"histn",
};

int (*icsh_builtin_func[]) (char **) = {
	&icsh_cd,
	&icsh_exit,
	&icsh_pid,
	&icsh_hist,
	&icsh_histn
};

int icsh_num_builtin(){
	return (sizeof(icsh_builtin_str) / sizeof(char *));
}

int icsh_cd(char **args){
	if (args[1] == NULL){
		chdir(home);
	} else {
		if (chdir(args[1]) != 0)
			printf("Couldn't cd\n");
	}

	return 1;
}

int icsh_exit(char **args){
	return 0;
}

int icsh_execute_command(char **args, char *line){
	pid_t pid = fork();

	if (pid == -1){
		printf("ERROR!\n");
	} else if (pid == 0){
		if (execvp(args[0], args) == -1)
			printf("ERROR\n");
	} else{
		wait(NULL);
	}

	return 1;

}

int icsh_execute_input(char **args, char *line){
	if (args[0] == NULL)
		return 1;

	int num_builtin = icsh_num_builtin();

	for (int i = 0; i < num_builtin; ++i){
		if (strcmp(args[0], icsh_builtin_str[i]) == 0)
			return (*icsh_builtin_func[i])(args);
	}

	return icsh_execute_command(args, line);
}

int icsh_pid(char **args){
	if (args[1] == NULL){
		printf("%d\n", getpid());
	} else if (strncmp(args[1], "current", strlen("current")) == 0){
		// FILE *
	}

	return 1;
}

int icsh_hist(char **args){
	if (args[1] == NULL){
		/* History - All */
		FILE *file_ptr_hist = fopen(file_history, "r");

		char *buffer;
		size_t buffer_size;
		ssize_t line_size;

		int line_counter = 0;

		printf("  History\n");

		line_size = getline(&buffer, &buffer_size, file_ptr_hist);

		while (line_size >= 0){
			line_counter++;
			printf("  %d  %s", line_counter, buffer);
			line_size = getline(&buffer, &buffer_size,
				file_ptr_hist);
		}

		fclose(file_ptr_hist);
	} else{
		printf("hist: hist does not take any additional arguments\n");
	}

	return 1;
}

int icsh_histn(char **args){
	int line_limit = atoi(args[1]);

	if (args[2] == NULL){
		/* History - n */
		FILE *file_ptr_hist = fopen(file_history, "r");

		char *buffer;
		size_t buffer_size;
		ssize_t line_size;

		int line_counter = 0;

		printf("  History\n");

		line_size = getline(&buffer, &buffer_size, file_ptr_hist);

		while (line_size >= 0){
			line_counter++;
			printf("  %d  %s", line_counter, buffer);
			line_size = getline(&buffer, &buffer_size,
				file_ptr_hist);
			if (line_counter == line_limit)
				break;
		}

		fclose(file_ptr_hist);
	}

	return 1;
}

int icsh_clean_up(){
	remove(file_pid_all);
	remove(file_pid_current);
	remove(file_history);

	return 1;
}