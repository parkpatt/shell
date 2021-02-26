#ifndef UTIL_H_
#define UTIL_H_

#define MAX_CMD_LEN 256
#define MAX_TOKENS 100


enum command_type {LS=0, WC=1, CD=2, EXIT=3, ERROR=4};

/*
 * Parses the input string passed to it
 * Takes in a pointer to the array of tokens
 * returns the total number of tokens read
 */
int parse_line(char * input, char * tokens[], char * delim);

/* Returns the command type of the command passed to it */
enum command_type get_command_type(char *command);

/* Removes the leading and trailing white spaces in the give string.
 * This function modifies the given string.
 */
char *trimwhitespace(char *str);

#endif 
