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
        int token = cmd.num_token;

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
            printf("token : %d\n", cmd.num_token);
            for(int i = 1; i < cmd.num_token; i++){
                makeDir(cmd.command_list[i]);
            }
        }
        else if(strcmp(cmdInput, "cd") == 0){
            printf("token : %d\n", cmd.num_token);
            if(cmd.num_token == 1){
                printf("Error: Too few arguments!");
            }
            changeDir(cmd.command_list[1]);
        }
        else if(strcmp(cmdInput, "rm") == 0){
            deleteFile(cmd.command_list[1]);
        }
        else if(strcmp(cmdInput, "cat") == 0){
            displayFile(cmd.command_list[1]);
        }
        else if(strcmp(cmdInput, "cp") == 0){
            if(cmd.num_token >= 5){
                printf("intput: %s\n", input);
                printf("token : %d\n", cmd.num_token);
                printf("Too many arguments!\n");
            } else {
                copyFile(cmd.command_list[1], cmd.command_list[2]);
            }
        }
        else{
            printf("Command not found!\n");
        }

        if (strcmp(input, "exit\n") == 0) {
            break;
        }
    }

    // Free dynamically allocated memory
    free(input);

    return 0;
}