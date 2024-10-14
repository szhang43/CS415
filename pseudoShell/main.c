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


int main(int argc, char *argv[]){
    char *input = NULL;          // Pointer for storing the input
    size_t storedInput = 0;      // Size of the input buffer
    size_t lineSize;            // Return value of getline

    while(1){
        printf(">>> ");
        
        // Get the line of input
        lineSize = getline(&input, &storedInput, stdin);
        
        // If getline encounters an error (returns -1), exit the loop
        if (lineSize == -1) {
            printf("Error reading input");
            break;
        }

        printf("%s", input);

        // You can add your command processing logic here...
        // For example, you can break the loop if the user types "exit"
        if (strcmp(input, "exit\n") == 0) {
            break;
        }
    }

    // Free dynamically allocated memory
    free(input);

    return 0;
}