/*
	Lab 2: August-5

	Question:

	A file contains a  list of commands, one per each line, with variable number of arguments.
	Write a C program to read each command and its arguments and execute them.
	The program terminates when all the commands are completed.

	E.g., Sample Commands in the file:

	ls -al
	wc prog.c (replace with your program name)
	ping www.google.com
	whoami
	touch newfile.c
	cp prog.c newfile.c
	diff prog.c newfile.c
	rm prog.c
	gcc newfile.c -o newfile.out
*/
/*
	Okay, so here's the logic. We start from main. We read the file.
	We create a child process. We execute the child process via execvp().
	Parent is to now wait till child is done. Once the child is done, it
	continues.

		The End.
*/

