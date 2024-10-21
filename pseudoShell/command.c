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
#include <fcntl.h>
#include <errno.h>  
#include <libgen.h>

void listDir(){

    size_t size;
    char cwd[1024];
    getcwd(cwd, sizeof(cwd));

    DIR *dir;
    dir = opendir(cwd);

    struct dirent *read_file; 

    while((read_file = readdir(dir)) != NULL){
        write(STDOUT_FILENO, read_file->d_name, strlen(read_file->d_name));
        write(STDOUT_FILENO, " ", 1);

    }
    closedir(dir);   
    write(STDOUT_FILENO, "\n", 1);
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
    char *sourceError = "Source path does not exist!\n";
    char *destError = "Destination path does not exist!\n";
    char otherError[1024] = "Rename Operation Failed\n";
    int sourceFd, destFd; 
    ssize_t bytes;
    char buffer[1024];

    struct stat sourceStat, destStat;

    if (stat(destinationPath, &destStat) == 0 && S_ISDIR(destStat.st_mode)) {
        char destFilePath[1024];
        strcpy(destFilePath, destinationPath);  
        strcat(destFilePath, "/");              
        strcat(destFilePath, basename(sourcePath)); 
   
        destinationPath = destFilePath;
    }

    sourceFd = open(sourcePath, O_RDONLY);
    destFd = open(destinationPath, O_CREAT | O_WRONLY | O_TRUNC, 0755);

    if(sourceFd == -1){
        write(STDERR_FILENO, sourceError, strlen(sourceError));
    }
    if(destFd == -1){
        write(STDERR_FILENO, destError, strlen(destError));
    }
    
    while((bytes = read(sourceFd, buffer, sizeof(buffer))) > 0){
        write(destFd, buffer, bytes);
    }
    close(sourceFd);
    close(destFd);
}; /*for the cp command*/

void moveFile(char *sourcePath, char *destinationPath){
    char *sourceError = "Source path does not exist!\n";
    char *destError = "Destination path does not exist!\n";
    char otherError[1024] = "Rename Operation Failed\n";

    struct stat sourceStat, destStat; 
    if (stat(destinationPath, &destStat) == 0 && S_ISDIR(destStat.st_mode)) {
        char newDestPath[1024];
        strcpy(newDestPath, destinationPath);
        strcat(newDestPath, "/");
        strcat(newDestPath, basename(sourcePath)); 
        destinationPath = newDestPath;
    }

    if(rename(sourcePath, destinationPath) != 0){
        if (stat(sourcePath, &sourceStat) != 0){
            write(STDERR_FILENO, sourceError, strlen(sourceError));
        } else if(stat(destinationPath, &destStat) != 0){
            write(STDERR_FILENO, destError, strlen(destError));
        } else {
            write(STDERR_FILENO, otherError, strlen(otherError));
        }
    }
   
}; /*for the mv command*/

void deleteFile(char *filename){
    char *msg = "Error! File does not exist!\n";
    if(remove(filename) != 0){
        write(1, msg, strlen(msg));
    };
}; /*for the rm command*/

void displayFile(char *filename){
    int fd; 
    fd = open(filename, O_RDONLY); 
    char buffer[1024];
    ssize_t bytes;

    while((bytes = read(fd, buffer, sizeof(buffer))) > 0){
        write(1, buffer, bytes);
    }
    // write(1, "\n", strlen("\n"));
    close(fd);
}; /*for the cat command*/
