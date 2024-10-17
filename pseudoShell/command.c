/*
 * string_parser.c
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

void listDir(){

    size_t size;
    char cwd[1024];
    getcwd(cwd, sizeof(cwd));

    DIR *dir;
    dir = opendir(cwd);

    struct dirent *read_file; 

    while((read_file = readdir(dir)) != NULL){
        printf("%s  ", read_file->d_name);

    }
    printf("\n");
    closedir(dir);   

}; /*for the ls command*/

void showCurrentDir(){
    size_t size; 
    char cwd[1024];
    char *dir;
    dir = getcwd(cwd, sizeof(cwd));
    printf("%s\n", dir);

}; /*for the pwd command*/

void makeDir(char *dirName){
    mode_t mode = 0755;
    mkdir(dirName, mode);
}; /*for the mkdir command*/

void changeDir(char *dirName){

}; /*for the cd command*/

void copyFile(char *sourcePath, char *destinationPath){

}; /*for the cp command*/

void moveFile(char *sourcePath, char *destinationPath){

}; /*for the mv command*/

void deleteFile(char *filename){

}; /*for the rm command*/

void displayFile(char *filename){

}; /*for the cat command*/
