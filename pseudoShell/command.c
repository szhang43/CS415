/*
 * command.c
 *
 *  Created on: October 07, 2024
 *      Author: Sophia Zhang 
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include "command.h"
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <errno.h>

void listDir(){

    size_t size;
    char cwd[1024];
    getcwd(cwd, sizeof(cwd));

    DIR *dir;
    dir = opendir(cwd);

    struct dirent *read_file; 

    while((read_file = readdir(dir)) != NULL){
        write(STDOUT_FILENO, read_file->d_name, strlen(read_file->d_name));
        write(STDOUT_FILENO, "   ", 3);

    }
    write(STDOUT_FILENO, "\n", 1);
    closedir(dir);   

}; /*for the ls command*/

void showCurrentDir(){
    size_t size; 
    char cwd[1024];
    char *dir;
    dir = getcwd(cwd, sizeof(cwd));
    write(STDOUT_FILENO, dir, strlen(dir));
    write(STDOUT_FILENO, "\n", 1);

}; /*for the pwd command*/

void makeDir(char *dirName){
    const char *errorMsg = "mkdir: cannot create directory '";
    const char *errorEnd = "': File exists\n";

    mode_t mode = 0755;
    int result = mkdir(dirName, mode);
    if(errno == EEXIST){
        char buffer[1024];
        strcpy(buffer, errorMsg);          
        strcat(buffer, dirName);          
        strcat(buffer, errorEnd);
        write(STDOUT_FILENO, buffer, strlen(buffer));
    }
}; /*for the mkdir command*/

void changeDir(char *dirName){
    chdir(dirName);
}; /*for the cd command*/

void copyFile(char *sourcePath, char *destinationPath){

}; /*for the cp command*/

void moveFile(char *sourcePath, char *destinationPath){

}; /*for the mv command*/

void deleteFile(char *filename){
    remove(filename);
}; /*for the rm command*/

void displayFile(char *filename){

}; /*for the cat command*/
