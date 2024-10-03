/*
 * string_parser.c
 *
 *  Created on: Nov 25, 2020
 *      Author: gguan, Monil
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "string_parser.h"

#define _GUN_SOURCE

int count_token (char* buf, const char* delim)
{
	//TODO：
	/*
	*	#1.	Check for NULL string
	*	#2.	iterate through string counting tokens
	*		Cases to watchout for
	*			a.	string start with delimeter
	*			b. 	string end with delimeter
	*			c.	account NULL for the last token
	*	#3. return the number of token (note not number of delimeter)
	*/

	int counter = 1; 
	char *savePtr, *token, *copy;
	copy = strdup(buf);

	strtok_r(copy, "\n", &savePtr);
	while(token != NULL){
		token = strtok_r(NULL, delim, &savePtr);
		counter++;
	}
	free(copy);
	return counter;
}

command_line str_filler (char* buf, const char* delim)
{
	//TODO：
	/*
	*	#1.	create command_line variable to be filled and returned
	*	#2.	count the number of tokens with count_token function, set num_token. 
    *           one can use strtok_r to remove the \n at the end of the line.
	*	#3. malloc memory for token array inside command_line variable
	*			based on the number of tokens.
	*	#4.	use function strtok_r to find out the tokens 
    *   #5. malloc each index of the array with the length of tokens,
	*			fill command_list array with tokens, and fill last spot with NULL.
	*	#6. return the variable.
	*/
	command_line cmd; 
	char *token, *savePtr, *copy; 
	copy = strdup(buf);

	cmd.num_token = count_token(copy, delim);
	cmd.command_list = (char**)malloc(sizeof(char *) * cmd.num_token);

	int i = 0;

	token = strtok_r(copy, delim, &savePtr);
	while(token != NULL){
		cmd.command_list[i] = strdup(token);
		i++; 
		token = strtok_r(NULL, delim, &savePtr);
	}
	cmd.command_list[i] = NULL;
	free(copy);
	return cmd;
}


void free_command_line(command_line* command)
{
	//TODO：
	/*
	*	#1.	free the array base num_token
	*/
	int ptr = 0;
	while(ptr < command->num_token){
		free(command->command_list[ptr]);
		ptr++;
	}
	free(command->command_list);
}
