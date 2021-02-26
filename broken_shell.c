#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/wait.h>
#include <stdbool.h>

// file redirection:
#include <sys/stat.h>
#include <fcntl.h>

/***** local #includes *****/
#include "util.h"

/***** environment variables *****/
#define MAX_DIR_LEN 1024 // maximum length for a directory path
char workingDirectory[MAX_DIR_LEN]; // stores the current working directory
char originalDirectory[MAX_DIR_LEN]; // stores the shell's orignial directory 
bool outputRedirected = false; // set to true when the tokens > or >> are found
int savedfd; // stores STDOUT_FILENO so it can be restored after redirection
bool piping = false; // true if "|" found in tokens
bool exitFlag = false; // set to true in order to break out of main loop

/***** helper functions *****/

/* int cd(char* newDirectory); 
 *
 * cd checks if newDirectory is a valid directory using the return value from
 * chdir. If invalid, returns -1, and current directory remains unchanged. If
 * valid, the working directory of the shell is changed to newDirectory via 
 * chdir, and 0 is returned.
 */
int cd(char* newDirectory){
  int retval = 0; 
  char buf[MAX_DIR_LEN];
  if(chdir(newDirectory) == -1){
    printf("Directory not found\n");
    retval = -1;
  } else {
    printf("%s\n", getcwd(buf, MAX_DIR_LEN));
  }
  return(retval);
}

/* int redirect(char * tokens[]);
 *
 * redirect is a function which checks a list of tokens
 * for the symbols ">" or ">>." If one of these tokens is found, redirect 
 * opens a file according to the token, and redirects stdout to that
 * file, storing the file descriptor for stdout in the environment
 * variable "savedfd." 
 *
 * Once it has redirected output, redirect replaces the token > or >> with 
 * NULL so that calls to exec don't treat the redirection token as an argument.
 *
 * If a redirect token was found, redirect will set the environment
 * variable "outputRedirected" to true. This variable is used in main to 
 * determine if output needs to be restored to stdout after a redirection.
 * 
 * If no redirection tokens are found, redirect returns -1. Otherwise, it 
 * returns the file descriptor for the file it has opened so that the file 
 * can later be closed.
 */
int redirect(char* tokens[]){
  int i = 0;
  int fd = -1;
  bool tokenFound = false;
  while(tokens[i] != NULL) {
    if (tokens[i+1] != NULL){ 
      if (strncmp(tokens[i], ">", 2) == 0){
	fd = open(tokens[i+1], O_CREAT | O_WRONLY, 0666);
	if (fd < 0) fprintf(stderr, "error opening file\n");
	tokenFound = true;
	break;
      } else if (strncmp(tokens[i], ">>", 3) == 0){
	fd = open(tokens[i+1], O_APPEND | O_WRONLY, 0666);
	if (fd < 0) fprintf(stderr, "error opening file\n");
	tokenFound = true;
	break;
      }
    }
    i++;
  } // end of while loop
  if (tokenFound){
    savedfd = dup(STDOUT_FILENO);
    if (dup2(fd, STDOUT_FILENO) == -1){
      fprintf(stderr, "error redirecting to file\n");
    } else {
      outputRedirected = true;
      tokens[i] = NULL;
    }
  }
  return fd;
}

char** check_piping(char* tokens[]){
  int i = 0;
  int j = 0;
  int lenOT = 0; // length of otherTokens[]
  char ** otherTokens = NULL; 
  while (tokens[i] != NULL){
    if (strcmp(tokens[i], "|") == 0){
      piping = true;
      j = i + 1;
      while(tokens[j] != NULL){
	lenOT++;
	j++;
      }
      otherTokens = (char **) malloc(sizeof(char*) * lenOT);
      for (int k = 0; k < lenOT; k++){
	otherTokens[k] = (char*) malloc(sizeof(tokens[i + 1 + k]));
	strncpy(otherTokens[k], tokens[i + 1 + k], sizeof(tokens[i + 1 + k]));
      }
      tokens[i] = NULL;
      break;
    }
    i++;
  }
  return otherTokens;
}

int execute(enum command_type cmd, char** toks, int numTokens){
  pid_t pid; // holds process id for forked processes
  int errorStatus = 0; // Will be set to 1 if any error is detected
  switch (cmd){
    
    /***** LS or WC ******/
  case 0:
  case 1:
    pid = fork();
    if (pid == 0){
      char callingDirectory[MAX_DIR_LEN];
      strncpy(callingDirectory, originalDirectory, MAX_DIR_LEN);
      strcat(callingDirectory, "/");
      strncat(callingDirectory, toks[0], sizeof(toks[0]));
      execv(callingDirectory, toks);
    } else {
      wait(NULL);
    }
    break;

    /***** CD ******/
  case 2:
    if (numTokens >= 2){
      if (cd(toks[1]) == 0){
	getcwd(workingDirectory, MAX_DIR_LEN);
      }
    }
    break;

    /***** EXIT *****/
  case 3:
    printf("exiting\n");
    exitFlag = true;
    break;

    /***** UNKNOWN COMMAND *****/
  default:
    pid = fork();
    char* linuxCmd = malloc(sizeof(toks[0]) + 6);
    if (pid == 0){
      strcpy(linuxCmd, "/bin/");
      strncat(linuxCmd, toks[0], sizeof(toks[0]));
      execv(linuxCmd, toks);
      fprintf(stderr, "Command error\n");
      errorStatus = 1;
      kill(getpid(), SIGTERM);
    } else {
      wait(NULL);
    }
    free(linuxCmd);
    break;
  } 
  return errorStatus;
}

/***** main function *****/
int main(){

  /***** local variables *****/
  char line[MAX_CMD_LEN+1]; // buffer to hold each line read from stdin
  char* tokens[MAX_TOKENS]; // tokenized version of line
  enum command_type cmd; // enumeration of command used in switch block
  int numTokens = 0; // used to determine success of parsing operation
  pid_t pipePID = -1;
  int output_fd; // stores file descriptor so that it can be closed
  int pipe_fd[2];
  char** toks;
  char** otherTokens;
  int errorStatus;
 
 
  // store original directory before main loop starts. This is used to
  // specify the path of our executables ls and wc in case the shell
  // has changed directories.
  getcwd(originalDirectory, MAX_DIR_LEN);

  /***** main loop *****/
  while(!exitFlag){
    if (outputRedirected){ // check if output was previously redirected
      dup2(savedfd, STDOUT_FILENO); // restore stdout using env variable
      outputRedirected = false; 
    }
    getcwd(workingDirectory, MAX_DIR_LEN); // update working directory
    printf("[4061 shell]%s $ ", workingDirectory); // print prompt
    fgets(line, MAX_CMD_LEN, stdin); // read from stdin
    strncpy(line, trimwhitespace(line), MAX_CMD_LEN); // remove whitespace
    numTokens = parse_line(line, toks, " "); // parse line into tokens
    if (numTokens > 0){ // if user only hits enter, just loop
      output_fd = redirect(tokens); // check for redirection tokens
      cmd = get_command_type(tokens[0]);
      toks = tokens;
      otherTokens = check_piping(tokens);
      if (otherTokens != NULL){
	if (pipe(pipe_fd) < 0) {
	  fprintf(stderr, "pipe creation failed.\n");
	  break;
	}
	pid_t pipePID = fork();
	if (pipePID < 0){
	  fprintf(stderr,"fork error\n");
	} else if (pipePID > 0){ // parent
	  toks = otherTokens;
	  cmd = get_command_type(otherTokens[0]);
	  savedfd = dup(STDIN_FILENO);  // save stdin so it can be restored
	  dup2(pipe_fd[0], STDIN_FILENO); // read from pipe
	  close(pipe_fd[1]); // close write end
	  wait(NULL);
	  printf("parent has finished waiting.\n");
	} else { // child
	  savedfd = dup(STDOUT_FILENO);
	  dup2(pipe_fd[1], STDOUT_FILENO);  // write to pipe
	  close(pipe_fd[0]); // close read end
	}
      }
      execute(cmd, toks, numTokens);
      if (output_fd != -1){
	close(output_fd);
      }
      if (pipePID > 0) { // parent
	dup2(savedfd, STDIN_FILENO);
	close(pipe_fd[0]);  // close read end
      } else if (pipePID == 0){ // child
	dup2(savedfd, STDOUT_FILENO);
	close (pipe_fd[1]);
	return 0;
      }
    } // end of if (numTokens > 0)
  } // end of main loop
  return errorStatus;
}
