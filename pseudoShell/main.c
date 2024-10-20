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
        
        command_line wholeInput = str_filler(input, ";"); 

        if (input[lineSize - 1] == '\n') {
            input[lineSize - 1] = '\0';
        }
        if(strcmp(input, "exit") == 0){
            break;
        }
        for(int i = 0; wholeInput.command_list[i] != NULL; i++){
            command_line cmd = str_filler(wholeInput.command_list[i], " ");
   
            char *cmdInput = cmd.command_list[0];
            int token = cmd.num_token - 1;

            if(cmd.command_list == NULL){
                continue;
            }

            if(strcmp(cmdInput, "ls") == 0){
                if(token > 1){
                    printf("Error! Unsupported paramters for command: %s\n", cmdInput);
                } else {
                    listDir();
                }
            } 
            else if(strcmp(cmdInput, "pwd") == 0){
                if(token > 1){
                    printf("Error! Unsupported paramters for command: %s\n", cmdInput);
                } else {
                    showCurrentDir();
                }
            } 
            else if(strcmp(cmdInput, "mkdir") == 0){
                if(token < 2){
                    printf("Error! Too few parameters for command: %s\n", cmdInput);
                }
                for(int i = 1; i < token; i++){
                    makeDir(cmd.command_list[i]);
                }
            }
            else if(strcmp(cmdInput, "cd") == 0){
                // printf("token : %d\n", cmd.num_token);
                if(cmd.num_token < 2){
                    printf("Error: Too few arguments!\n");
                } else {
                    changeDir(cmd.command_list[1]);
                }
            }
            else if(strcmp(cmdInput, "rm") == 0){
                if(token >= 3) {
                    printf("Error: Too many arguments!\n");
                } else {
                    deleteFile(cmd.command_list[1]);
                }
            }
            else if(strcmp(cmdInput, "cat") == 0){
                if(token >= 3) {
                    printf("Error: Too many arguments!\n");
                } else {
                    displayFile(cmd.command_list[1]);
                }
            }
            else if(strcmp(cmdInput, "cp") == 0){
                if(token >= 3){
                    printf("Error: Too many arguments!\n");

                } else {
                    copyFile(cmd.command_list[1], cmd.command_list[2]);
                }
            }
            else if(strcmp(cmdInput, "mv") == 0){
                // printf("source: %s\n dest: %s\n", cmd.command_list[1], cmd.command_list[2]);
                if(token >= 3){ 
                    printf("Error: Too many arguments!\n");
                } else {
                    moveFile(cmd.command_list[1], cmd.command_list[2]);
                }
            }
            else{
                printf("Command not found!\n");
            }

        }


        // printf("strcmp mkdir: %d \n", strcmp(input, cmd.command_list[0]));
        // printf("input: %s\n", input);

    }

    // Free dynamically allocated memory
    free(input);

    return 0;
}