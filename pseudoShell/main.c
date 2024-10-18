/*
 * main.c
 *
 *  Created on: October 07, 2024
 *      Author: Sophia Zhang 
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "command.h"
#include "string_parser.h"


int main(int argc, char *argv[]){
    char *input = NULL;          // Pointer for storing the input
    size_t storedInput = 0;      // Size of the input buffer
    size_t lineSize;            // Return value of getline

    while(lineSize > 0 ){
        printf(">>> ");
        
        // Get the line of input
        lineSize = getline(&input, &storedInput, stdin);
        
        command_line cmd = str_filler(input, " ");
        char *cmdInput = cmd.command_list[0];
        int token = count_token(input, " ");

        // If getline encounters an error (returns -1), exit the loop
        if (lineSize == -1) {
            printf("Error reading input");
            break;
        }

        if (input[lineSize - 1] == '\n') {
            input[lineSize - 1] = '\0';
        }

        // printf("strcmp mkdir: %d \n", strcmp(input, cmd.command_list[0]));
        // printf("input: %s\n", input);

        if(strcmp(input, "ls") == 0){
            listDir();
        } 
        else if(strcmp(input, "pwd") == 0){
            showCurrentDir();
        } 
        else if(strcmp(cmdInput, "mkdir") == 0){
            for(int i = 1; i < token - 1; i++){
                makeDir(cmd.command_list[i]);
            }
        }
        else if(strcmp(cmdInput, "cd") == 0){
            printf("%s\n", cmd.command_list[1]);
            if(cmd.command_list[1] == NULL){
               continue;
               // TODO: cd back to home
            }
            changeDir(cmd.command_list[1]);
        }
        else if(strcmp(cmdInput, "rm")){
            deleteFile(cmd.command_list[1]);
        }
        else{
            printf("Commasnd not found!\n");
        }

        if (strcmp(input, "exit\n") == 0) {
            break;
        }
    }

    // Free dynamically allocated memory
    free(input);

    return 0;
}