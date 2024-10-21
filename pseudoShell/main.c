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
#include <fcntl.h>
#include "command.h"
#include <sys/types.h>
#include <unistd.h>
#include "string_parser.h"


int main(int argc, char *argv[]){
    
    char *input = NULL;          // Pointer for storing the input
    size_t storedInput = 0;      // Size of the input buffer
    size_t lineSize = 0;            // Return value of getline
    FILE *fd;
    FILE *outFd;
    FILE *output;

    if(argc >= 3){
        if(strcmp(argv[1], "-f") == 0){

            char buffer[1024];
            ssize_t bytes;

            fd = fopen(argv[2], "r"); 
            if (fd == NULL) {
                perror("Error opening file!\n");
                fclose(fd);
            }
            outFd = fopen("output.txt", "w");
            if (output == NULL) {
                perror("Error redirecting output to file");
                return 1;
            }
            output = freopen("output.txt", "w", stdout);
        }   
    } 

    while(1){

        if(argc >= 3 && strcmp(argv[1], "-f") == 0){
            lineSize = getline(&input, &storedInput, fd);
            if(lineSize == -1) {
                break;
            }
        } else {
            printf(">>> ");
            lineSize = getline(&input, &storedInput, stdin);
            if(strcmp(input, "exit\n") == 0){
                break;
            }
        }
        
        command_line wholeInput = str_filler(input, ";"); 
        for(int i = 0; wholeInput.command_list[i] != NULL; i++){
            command_line cmd = str_filler(wholeInput.command_list[i], " ");
            cmd.num_token -= 1;
            char *cmdInput = cmd.command_list[0];
            

            if(cmd.command_list == NULL){
                continue;
            }

            if(strcmp(cmdInput, "ls") == 0){
                if(cmd.num_token > 1){
                    for(int i = 0; cmd.command_list[i] != NULL; i++){
                    }
                    printf("Error! Unsupported paramters for command: %s\n", cmdInput);
                } else {
                    listDir();
                }
            } 
            else if(strcmp(cmdInput, "pwd") == 0){
                if(cmd.num_token != 1){
                    printf("Error! Unsupported paramters for command: %s\n", cmdInput);
                } else {
                    showCurrentDir();
                }
            } 
            else if(strcmp(cmdInput, "mkdir") == 0){
                if(strcmp(cmdInput, "mkdir") == 0){
                    if(cmd.num_token != 2){
                        printf("Error: Too few or too many arguments for command : %s\n", cmdInput);
                    }
                    for(int i = 1; i < cmd.num_token; i++){
                        if (cmd.command_list[i] == NULL) {
                            printf("Error: Directory name cannot be NULL\n");
                            continue;
                        }
                        makeDir(cmd.command_list[i]);
                    }
                }
            }
            else if(strcmp(cmdInput, "cd") == 0){
                if(cmd.num_token != 2){
                    printf("Error: Too few or too many arguments for command : %s\n", cmdInput);
                    continue;
                } else {
                    changeDir(cmd.command_list[1]);
                }
            }
            else if(strcmp(cmdInput, "rm") == 0){
                if(cmd.num_token != 2) {
                    printf("Error: Too few or too many arguments for command : %s\n", cmdInput);
                } else {
                    deleteFile(cmd.command_list[1]);
                }
            }
            else if(strcmp(cmdInput, "cat") == 0){
                if(cmd.num_token != 2) {
                    printf("Error: Too few or too many arguments for command : %s\n", cmdInput);

                } else {
                    displayFile(cmd.command_list[1]);
                }
            }
            else if(strcmp(cmdInput, "cp") == 0){
                if(cmd.num_token != 3){
                    printf("Error: Too few or too many arguments for command : %s\n", cmdInput);

                } else {
                    copyFile(cmd.command_list[1], cmd.command_list[2]);
                }
            }
            else if(strcmp(cmdInput, "mv") == 0){
                if(cmd.num_token != 3){ 
                    printf("Error: Too few or too many arguments for command : %s\n", cmdInput);
                } else {
                    moveFile(cmd.command_list[1], cmd.command_list[2]);
                }
            }
            else{
                printf("Command not found!\n");
            }

            free_command_line(&cmd);
        }
        free_command_line(&wholeInput);
    }

    if(argv[1]){
        fclose(fd);
        fclose(outFd);
        fclose(output);
    }

    return 0;
}
