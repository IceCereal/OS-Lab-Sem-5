#ifndef ICSH_H_
#define ICSH_H_

/*
	Lab 5: 2019-Sept-05

	Expanding on Labs 2-4:
		Creating an interactive shell with signal handling capability.

	In this lab task, unix type shell which will read commands from the user and execute them appropriately. 
	There are two types of commands that can be expected from the user: 
		a user-defined command and a system-command. 
		
	The following are the specifications for the project.
		Specification 1 : Display requirement
			When you execute your code a shell prompt of the following form must appear:
				<username@system_name:curr_dir>.
			The directory from which the shell is invoked will be the home directory of the shell and should be indicated by "~"
			If the user executes "cd" change dir then the corresponding change must be reflected in the shell as well.

		Specification 2 :  User-defined commands
			The following commands must be supported by the shell
				pid : prints the process id of your shell program
				pid current : prints the list of the process ids of the processes that are created by the shell
					and currently active
				pid all : prints the list of process ids of all commands that were executed so far by the shell
					(including the currently executing processes)
				hist : list of all commands, in chronological order, executed by the shell so far
				histn: list of last "n" commands executed by the shell. If the number of commands is less than "n"
					then the shell should print only the available number of commands.
				!histn : execute history command number "n" (assuming the first command is numbered 1)

		Specification 3: System commands with and without arguments
			All other commands are treated as system commands like : ls, emacs, vi and so on. The shell must be able
				to execute them either in the backgroud or in the foreground.
			--Foreground processes:
				For example, executing a "vi" command in the foreground implies that your shell will wait
				for this process to complete and regain control when this process exits.
			--Background processes:
				Any command invoked with "&" is treated as background command. This implies that your shell will
				spawn that process and doesn't wait for the process to exit. It will keep taking user commands.
				If the background process exits then the shell must display the appropriate message to the user.

		Specification 3: General notes
			1. Use exec family of commands to execute system commands. If the command cannot be run or returns an error
				it should be handled approiately. Look at perror.h for appropriate routines to handle errors.
			2. Use fork() for creating child processes where needed and wait() for handling child processes.
			3. Use signal handlers to process signals from exiting background processes. Marks will be deducted if
				proper signal handling is not done.
			4. You can use : uname, hostname, whomai commands to get the shell display working.  For respective header
				libraries use: man <cmd>
			5. The user can type the command anywhere in the command line i.e., by giving spaces, tabs etc. Your shell
				should be able to handle such scenarios appropriately. 
			6. Segmentation faults at the time of grading will be penalized.
*/

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
icsh_clean_up

This is to remove the locally created files:
._icsh_pid_all
._icsh_pid_current
._icsh_history

returns: 1 on success
*/

void sigchld_handler(int);

int icsh_clean_up();

#endif