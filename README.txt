P1 - SSI 
Jake Houston - V00980680

Basic Functions
- ssi.c successfully compiles by the make file with the command make which creates the executable ssi
- ssi can be executed without additional parameters and prompts the user for input
- the prompt is in the correct format 

Foreground execution
- ssi can execute commands in the foreground with or without additional parameters within the current directory 
- ssi can execute long running programs like ping and use Ctrl-C to stop the program 
- ssi can execute arbitrary external programs, and prints the name of the command inputted followed by the error, ex. <name>: No such file or directory

Changing directories
- ssi can print the current working directory by pwd 
- ssi can change to directories by specifying absolute directory paths, handle relative paths, and go to home directory using cd or cd ~
- ssi always prints the current working directory correctly 

Background execution
- ssi can use bg to execute programs in the background and still accept user input in the foreground 
- bglist properly lists the processes running in the background in the correct format 
- when a background process has finished it will print in the correct format when processing the next user input 
- ssi properly updates the background jobs by removing them from the list when they terminate