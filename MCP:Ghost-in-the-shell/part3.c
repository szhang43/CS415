#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>
#include <errno.h>

/* Global Variables*/
pid_t *pid_array;
int count = 0; // how many commands in a file
int pids = 0; // how many processes in pid_array
int current_process = 0;
sigset_t sigset;
int sig;

void waiting(){
    for (int i = 0; i < pids; i++) {
        if (pid_array[i] != -1) {
            int status;
            pid_t result = waitpid(pid_array[i], &status, 0); // Wait for process to exit

            if (result > 0) {  // A child process has changed state
                if (WIFEXITED(status)) {
                    pid_array[i] = -1;  // Mark process as terminated
                }
            } else if (result < 0) {
                if (errno != ECHILD) {  // Ignore "No child processes" errors here
                    perror("waitpid error");
                }
            }
        }
    }   
}

/* Round Robin Scheduler: Switches to Next Process Each Time Slice */
void round_robin(int sig) {
    // Stop the currently running process, if active
    if (pid_array[current_process] != -1) {
        kill(pid_array[current_process], SIGSTOP);
    }
    
    // Move to the next process
    current_process = (current_process + 1) % pids;
    while (pid_array[current_process] == -1) { // Skip terminated processes
        current_process = (current_process + 1) % pids;
    }

    // // Continue the next process
    kill(pid_array[current_process], SIGCONT);
    printf("Scheduling Process: PID: %d\n", pid_array[current_process]);

    // Set the alarm for the next time slice
    alarm(1);
}

void scheduler() {
    // printf("Starting Wrong Robin\n");
    signal(SIGALRM, round_robin);
    alarm(1);
}

int countLine(char *filename){
    int totalLines = 0;
    char lines[1024];
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error reading file!");
        fclose(file);
        exit(1);
    }
    while(fgets(lines, sizeof(lines), file)){
        totalLines++;
    }
    fclose(file);
    return totalLines;

}

int main(int argc, char *argv[]) {
    if (argc > 3) {
        printf("Wrong number of arguments\n");
        exit(0);
    }

    if (strcmp(argv[1], "-f") != 0) {
        printf("Include -f flag to use filemode!\n");
        exit(1);
    }

    // Open file

    FILE *file = fopen(argv[2], "r");
    if (file == NULL) {
        perror("Error reading file!");
        fclose(file);
        exit(1);
    }

    char readCommand[1024]; // single command line from input file
    int commandLine = countLine(argv[2]);
    pid_array = malloc(commandLine * sizeof(pid_t));
    if (pid_array == NULL) {
        perror("Memory allocation failed");
        free(pid_array);
        fclose(file);
        exit(1);
    }

    sigemptyset(&sigset);
    // Add CONT to the signal set
    sigaddset(&sigset, SIGCONT);
    // Block SIGALRM signal
    sigprocmask(SIG_BLOCK, &sigset, NULL);

    while (fgets(readCommand, sizeof(readCommand), file)) { // reads each line from the input until NULL
        size_t len = strlen(readCommand); // This block removes the newline character
        if (len > 0 && readCommand[len - 1] == '\n') {
            readCommand[len - 1] = '\0';
        }

        char *command = strtok(readCommand, " "); // gets the command from the read line
        char *args[1024]; // create an array to store the individual commands plus arguments
        int i = 1; // increment by 1 since strtok skips the first one
        count++; // count how many flags and parameters are there
        if (command != NULL) { // check if command is null
            args[0] = command; // Store the command in args[0]
            while (command != NULL) { // run until it has reached the end of the line
                command = strtok(NULL, " ");
                if (command != NULL) {
                    args[i] = command; 
                    i++;
                }
            }
        } else {
            printf("Failed to read commands from input file!\n");
            free(pid_array);
            fclose(file);
            exit(1);
        }

        args[i] = NULL; // terminates args array
        pid_t pid = fork(); // create a child process for each command from the input file
        
        if (pid < 0) { // fork failed
            perror("Fork failed");
            fclose(file);
            exit(1);
        } else if (pid == 0) { // child process
            if (sigwait(&sigset, &sig) == 0) {
                if (execvp(args[0], args) == -1) {
                    perror("Execvp Failed");
                    free(pid_array);
                    fclose(file);
                    exit(1);
                }
            } else {
                perror("sigwait failed");
                free(pid_array);
                fclose(file);
                exit(1);
            }
        } else { // parent process
            pid_array[pids++] = pid;
        }
    }
    scheduler();
    waiting();

    free(pid_array);
    
    fclose(file);

    printf("All processes have completed.\n");
    return 0;
}
