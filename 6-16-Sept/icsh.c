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
#include <signal.h>

#include "icsh.h"

char home[BUFSIZE], file_pid_all[BUFSIZE], file_history[BUFSIZE];
char username[BUFSIZE], *nodename;
struct current_pids *head_pid;

int main(int argc, char *argv[]){
	getlogin_r(username, BUFSIZE);

	struct utsname uname_data;
	uname(&uname_data);
	nodename = uname_data.nodename;

	getcwd(home, BUFSIZE);

	strcpy(file_pid_all, "");
	strcpy(file_history, "");

	strcat(file_pid_all, home);
	strcat(file_history, home);

	strcat(file_pid_all, "/._icsh_pid_all");
	strcat(file_history, "/._icsh_history");

	char *inputLine;
	char **args;
	int statusFlag = 1;

	FILE *file_ptr_pid_all = fopen(file_pid_all, "w");
	FILE *file_ptr_hist = fopen(file_history, "w");

	fclose(file_ptr_pid_all);
	fclose(file_ptr_hist);

	head_pid = (struct current_pids *)malloc(sizeof(struct current_pids));
	head_pid->pid = getpid();
	head_pid->next = NULL;

	signal(SIGCHLD, sigchld_handler);

	do{
		printf("%s@%s:~%s/ ", username, nodename, disp_dir());

		inputLine = icsh_getline();
		args = icsh_parse_line(inputLine);
		statusFlag = icsh_execute_input(args, inputLine);

	} while(statusFlag);

	free(inputLine);
	free(args);
	free(head_pid);


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
	char* tok = strtok(lineTemp, " \t\n");
	while(tok != NULL){
		totalArgs++;

		int tempLen = strlen(tok);

		if (tempLen > max_argLen)
			max_argLen = tempLen;

		tok = strtok(NULL, " \t\n");
	}

	// Allocate space for the tokens
	char **token = (char **)malloc((totalArgs + 1) * sizeof(char *));
	for (int i = 0; i < totalArgs + 1; ++i)
		token[i] = (char *)malloc((max_argLen) * sizeof(char));

	int argv_counter = 0;
	tok = strtok(command, " \t\n");
	while (tok != NULL){
		strcpy(token[argv_counter++], tok);
		tok = strtok(NULL, " \t\n");
	}

	token[argv_counter] = NULL;

	free(command);
	free(lineTemp);

	return token;
}

int icsh_cd(char **);
int icsh_exit(char **);
int icsh_pid(char **);
int icsh_hist(char **);
int icsh_histn(char **);
int icsh_exec_line(char *);
int icsh_exec_hist(char **);

char *icsh_builtin_str[] = {
	"cd",
	"exit",
	"pid",
	"hist",
	"histn",
	"!histn"
};

int (*icsh_builtin_func[]) (char **) = {
	&icsh_cd,
	&icsh_exit,
	&icsh_pid,
	&icsh_hist,
	&icsh_histn,
	&icsh_exec_hist
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

int icsh_execute_command(char **args){
	// Check background
	int background = 0, count = 0;

	while (args[count] != NULL){
		if ((strncmp(args[count], "&", strlen("&")) == 0) && (args[count + 1] == NULL)){
			args[count] = NULL;
			background = 1;
			break;
		}
	
		count++;
	}

	pid_t pid = fork();

	if (pid == -1){
		printf("ERROR!\n");
	} else if (pid == 0){
		if (execvp(args[0], args) == -1){
			printf("ERROR\n");
			exit(0);
		}
		exit(0);
	} else{
		char pid_str[10];
		snprintf(pid_str, 10, "%d\n", pid);

		FILE *file_ptr_pid_all = fopen(file_pid_all, "a");
		fprintf(file_ptr_pid_all, pid_str, strlen(pid_str));
		fclose(file_ptr_pid_all);

		if (!background){
			wait(NULL);
		} else{
			printf("  [BG: %d]\n", pid);
			insert_pid(pid);
		}
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

	return icsh_execute_command(args);
}

int insert_pid(int pid){
	struct current_pids *node = (struct current_pids *)malloc(
		sizeof(struct current_pids));
	struct current_pids *loop_node;

	node->pid = pid;
	node->next = NULL;

	/* Go through everything */
	loop_node = head_pid;

	while (loop_node->next != NULL){
		loop_node = loop_node->next;
	}

	loop_node->next = node;

	return 1;
}

int delete_pid(int pid){
	struct current_pids *loop_node;
	struct current_pids *prev;

	loop_node = head_pid;
	prev = head_pid;

	while(loop_node != NULL){

		if (loop_node->pid == pid){
			prev->next = loop_node->next;
			free(loop_node);
			break;
		}

		prev = loop_node;
		loop_node = loop_node->next;
	}

	return 1;
}


int icsh_pid(char **args){
	if (args[1] == NULL){
		printf("%d\n", getpid());
	} else if (strncmp(args[1], "current", strlen("current")) == 0){
		struct current_pids *loop_node;

		loop_node = head_pid;

		while(loop_node != NULL){
			printf("PID:\t%d\n", loop_node->pid);
			loop_node = loop_node->next;
		}
	} else if (strncmp(args[1], "all", strlen("all")) == 0){
		FILE *file_ptr_pidAll = fopen(file_pid_all, "r");

		if (file_ptr_pidAll == NULL){
			printf("couldn't open %s\n", file_pid_all);
			return -1;
		}

		printf("  PID\n");

		char pid[10];

		while(fgets(pid, sizeof(pid), file_ptr_pidAll) != NULL){
			printf("  %s", pid);
		}

		fclose(file_ptr_pidAll);
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
	} else{
		printf("histn: histn does not take any additional arguments\n");
		printf("USAGE: histn line_number\n");
	}

	return 1;
}

int icsh_exec_line(char *line){
	char **args_NewLine;

	args_NewLine = icsh_parse_line(line);

	icsh_execute_command(args_NewLine);

	return 1;
}

int icsh_exec_hist(char **args){
	int line_execute = atoi(args[1]);

	if (args[2] == NULL){
		/* Execute line number n */
		FILE *file_ptr_hist = fopen(file_history, "r");

		char *buffer;
		size_t buffer_size;
		ssize_t line_size;

		int line_counter = 0;
		int flag_FoundLine = 0;

		line_size = getline(&buffer, &buffer_size, file_ptr_hist);

		while (line_size >= 0){
			line_counter++;

			if (line_counter == line_execute){
				flag_FoundLine = 1;
				break;
			}

			line_size = getline(&buffer, &buffer_size,
				file_ptr_hist);
		}

		if (flag_FoundLine){
			icsh_exec_line(buffer);
		} else {
			printf("line_number provided is greater than total \
			number of lines in history\n");
			printf("USAGE: !histn line_number\n");
		}

		fclose(file_ptr_hist);
	} else{
		printf("histn: !histn does not take any additional arguments\n");
		printf("USAGE: !histn line_number\n");
	}
	return 1;
}

void sigchld_handler(int signum){
	pid_t pid;

	if ( (pid = waitpid(-1, NULL, WNOHANG)) != -1){
		delete_pid(pid);
		// printf("%s@%s:~%s/ ", username, nodename, disp_dir());
		return;
	}
	return;
}

int icsh_clean_up(){
	printf("Cleaning up...\n");

	remove(file_pid_all);
	remove(file_history);

	struct current_pids *loop_node;
	struct current_pids *next;

	loop_node = head_pid;

	while(loop_node != NULL){
		next = loop_node->next;
		free(loop_node);
		loop_node = next;
	}

	return 1;
}