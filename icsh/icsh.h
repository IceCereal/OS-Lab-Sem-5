#ifndef ICSH_H_
#define ICSH_H_

#define BUFSIZE 1024

struct current_pids{
	int pid;
	struct current_pids *next;
};

/*
disp_dir

This is to compute what goes after the ~ in <user@host>:~
It computes the "effective" directory

params: None
returns: *effective_directory
*/
char *disp_dir();

/*
icsh_getline

This gets the input from the user.

params: None
returns: *line
*/
char *icsh_getline();

/*
icsh_log_history

This stores the entered command to .icsh_history

params: line
returns: 1 (on success)
*/
int icsh_log_history(char *);

/*
icsh_parse_line

This parses the line into arguments (like char *argv[])

params: *line
returns: **args
*/
char **icsh_parse_line(char *);

/*
icsh_execute_input

This is the primitive "executor". It has a check to see if anything built-in
matches args[0] and accordingly executes the command.

input: **args
input: *line
returns: statusFlag (for main loop)
*/
int icsh_execute_input(char **, char *);

/*
icsh_execute_command

This is when the input is *not* a built-in function and instead is a normal
executable command.


input: **args
input: *line
returns: 1 (for the statusFlag (for main loop))
*/
int icsh_execute_command(char **);

/*
insert_pid
delete_pid

Util functions for linked list [struct current_pids]
input: int pid
returns: 1 (for success)
*/
int insert_pid(int);
int delete_pid(int);

/*
*/
void sigchld_handler(int signum);

/*
icsh_clean_up

This is to remove the locally created files:
._icsh_pid_all
._icsh_pid_current
._icsh_history

returns: 1 on success
*/
int icsh_clean_up();

#endif