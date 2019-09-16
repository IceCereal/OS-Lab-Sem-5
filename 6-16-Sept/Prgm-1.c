/*
	Lab 6: 2019-Sept-16
	Due: Sept 25th, 11.59 pm (no extensions)

	`Project-1`
	Expanding on Labs 2-5:
		Adding I/O redirect capability to the interactive shell.
		"Expanding" means all the previous functionality must still be supported.

	PLEASE READ THE ENTIRE PROJECT CAREFULLY. MAKE SURE YOUR DESIGN IS ON PAPER BEFORE YOU BEGIN CODING.

	In this lab task, you will build on the Unix type shell you developed during the last lab. You will support the following functionality:
		1. Input/Output redirect with "<" and ">"
		2. Pipes
		3. Combination of both.

	Project Specifications.
		Specification -1 : Output redirection with ">"
			<manish@research:~>wc temp.c > output.txt
			<manish@research:~>cat output.txt
			xx xx xx
			<manish@research:~>

		Specification -2 : Input redirection with "<"
			<manish@research:~>wc < temp.c 
			xx xx xx

		Specification -3 : Input/Output redirection with "<" ">"
			<manish@research:~>wc < temp.c > output.txt
			<manish@research:~>cat output.txt
			xx xx xx

		Specification -4: Redirection with pipes (multiple pipes should be supported
			<manish@research:~>cat temp.c | wc
			xx xx xx

		Specification -5: Redirection with pipes and ">"
			<manish@research:~>cat temp.c | wc > output.txt
			<manish@research:~>cat output.txt
			xx xx xx

	Constraints:
		1. You cannot use Pipes to implement the Input/Output redirection. They have to be implemented independently.
		2. Should not use the "system" command for this task/project.
		3. You should not execute ANY command more than once. This will be strictly penalized.
		4. For implementing multiple pipes, you are likely to search online and get lots of code. I will allow this up to some extent, but I would suggest that you try to do this from first principles before trying someone else's code.
		5. Extra marks are available for code that is well commented and well organized (easy to read and debug).

	General notes to assist you
		1. You can use : uname, hostname, whomai commands to get the shell display working.  The following man pages will be useful.
			http://man7.org/linux/man-pages/man3/getlogin.3.html
			https://pubs.opengroup.org/onlinepubs/9699919799/functions/uname.html
			https://pubs.opengroup.org/onlinepubs/009695399/basedefs/unistd.h.html

		2. The user can type the command anywhere in the command line i.e., by giving spaces, tabs etc. Your shell should be able to handle such scenarios appropriately. 
		3. Segmentation faults at the time of grading will be penalized.
		4. Zero tolerance for plagiarism. You can discuss but should NOT share code or copy from each other.
*/