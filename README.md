README for 4061 shell

TEAM:
Parker Patterson  patte591
Brandon Schenk	  schen259
Isaac Glover 	  glove080


CONTRIBUTIONS:
The code for ls was written by Isaac, wc was implemented by Brandon; cd and
exit were implemented by Parker. Parker was responsible for the initial
implementation of shell.c, including execution logic and parsing, but several
major bugs in the implementation were fixed by Isaac. Parker also implemented 
file redirection, pipes, and wrote this README.


HOW TO COMPILE:
To compile our shell, navigate to the directory containing this README and type
make


TEST CASES:
We tested our code using the following inputs. All inputs were typed at the
4061 shell command line. 

TEST CASES TAKEN FROM THE ASSIGNMENT PDF:
./cd ..
./cd <path>
./ls ..
./ls -R ..
./wc testwc.txt
./wc -c testwc.txt
./wc -w testwc.txt
./wc -l testwc.txt
ls > out.txt
cat out.txt | grep util
ls | wc > out.txt

OTHER TEST CASES:
ls | wc
ls >> out.txt
cat out.txt | grep util > out2.txt


USAGE AND DESCRIPTION OF FEATURES

SHELL

USAGE:
To run our shell, navigate to the directory containing this README and type
./shell

DESCRIPTION:
Once inside the shell, any Unix command may be executed, but we have created our
own implementations of the commands "ls" "wc" "cd" and "exit." These commands
can be executed by typing their usual Unix names into the 4061 shell's command
prompt. Our shell also supports file redirection and piping, as described below.


FILE REDIRECTION

USAGE:
ls > <filename>
-- or --
ls >> <filename>

DESCRIPTION:
Our shell supports file redirection. Use the character '>' to redirect output
to a file, and '>>' to append output to the end of a file.


PIPING

USAGE:
cat <filename> | grep <string>

DESCRIPTION:
At this time, our shell only supports piping between two commands. Use the
character '|' to separate the two commands for which piping is desired.
Output for the command on the left side of the pipe character will be passed 
as input to the command on the right side of the pipe character.


LS

USAGE:
ls <OPTIONAL flag> <OPTIONAL directory>

DESCRIPTION:
ls can be called without any arguments, in which case it displays the content of
the 4061 shell's current working directory. If ls is passed an argument, it
interprets the argument as a directory path and, if the path is valid, that
directory's contents will be printed. If ls is called with the -r flag, ls will
recursively display the contents of any directories within the current working
directory as well as the contents of the working directory itself.


WC

USAGE:
wc <OPTIONAL filename>

DESCRIPTION:
wc displays information about the number of characters, words, and lines in a
given file. wc must be passed a valid filename as an argument. If wc is passed
no arguments, it will read from stdin. The flag -l will cause wc to print only 
the number of lines in the input file. The -w flag will cause wc to print only 
the number of words, and the -c flag instructs wc to print only the number of 
characters. Specifying no flag will result in line-, character-, and word-counts 
for the input file to be displayed.


CD

USAGE:
cd <path>

DESCRIPTION:
cd switches the working directory of the shell process to the specified 
directory, if possible; otherwise, it prints an error message and the working
directory of the shell process remains unchanged. cd is implemented within
the file shell.c.


EXIT

USAGE:
exit

DESCRIPTION:
Type 'exit' at the command line of the 4061 shell in order to exit the shell.