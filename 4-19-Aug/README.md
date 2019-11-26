# Lab 4: 2019-19-Aug
	
	History of commands
	
	Relevant topics/commands: `files`, `pipes`, `fork`, `malloc`, `free` and `inter-process communication`.
	
	Specification (To be implemented and shown in lab)
	
	Extend the program written for Lab task 2 to read multiple file names from the command prompt 
	and to be able to read interactively from the user.
	Each file contains a list of commands, one per each line, with variable number of arguments.
	Write a C program to read each command and its arguments and execute them. Once the program doesn't
	have any more lines to read from a file, it will process the next file and so on.
	For this lab, you may assume that no files have overlapping commands.
	Once there are no more files to read, the program will move to an interactive mode and start reading user inputs.
	
	You are to implement the following two features.
	
	In the interactive mode, the program should prompt user for inputs as follows: "Please enter your command:"
 	
	The following commands should be supported:
		1. When the user types command "HISTORY BRIEF", the program should list out all the commands 
		(no need for file names) that were executed so far without the respective arguments. 
		The display should show one command per line.
	
		2. When the user types command "HISTORY FULL", the program should list all the commands so far with their 
		respective arguments. Each command should have an INDEX number associated with it and it should be displayed as well.
		
		3. If the user types "EXEC <COMMAND_NAME>", your program should execute that command with its arguments.
		
		4. When if the user types "EXEC <COMMAND_INDEX_NUMBER>", your program should execute that command with its arguments.
		
		5. The program should exit when the user types a special command "STOP".
		
		6. The program should handle following border conditions:
			a. At least one file name should be supplied when the program starts.
			b. For unrecognized commands (not in history), the program should type appropriate message.
			c. Avoid memory leaks and segmentation faults. Don't allocate memory where not required and as far 
			as possible free memory.

To compile:<br>
```gcc -o output Prgm-1.c -Wall```

To run:<br>
```./output test-commands test-commands-1```

Note: There should be a `\n`(newline, not literally) at the end of the file:

>		FILE_NAME
>		    ls -al
>		    ping
>		    whoami
>		
>		END
for the program to work
