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
int cycle_count = 0; 
sigset_t sigset;
int sig;
#define MAX_PROC_NAME_LEN 256

void get_process_info(int pid) {
    char path[MAX_PROC_NAME_LEN];
    FILE *fp;
    char buf[1024];
    long utime, stime, nice, virt_mem;
    int process_pid;

    // Get the number of clock ticks per second
    long clk_tck = sysconf(_SC_CLK_TCK);
    if (clk_tck < 0) {
        perror("sysconf");
        return;
    }

    // Create the path to the stat file
    snprintf(path, sizeof(path), "/proc/%d/stat", pid);

    // Open the stat file
    fp = fopen(path, "r");
    if (fp == NULL) {
        perror("fopen");
        return;
    }

    // Read the data from the stat file
    if (fgets(buf, sizeof(buf), fp) == NULL) {
        perror("fgets");
        fclose(fp);
        return;
    }

    // printf("Buffer : %s\n", buf);
    // Parse the stat file (skip the command name)
    sscanf(buf, "%d %*s %*c %*d %*d %*d %*d %*d %*u %*lu %*lu %*lu %*lu %lu %lu %*ld %*ld %*ld %ld %*ld %*ld %*llu %lu", 
       &process_pid, &utime, &stime, &nice, &virt_mem);

    // sscanf(buf, "%d %*s %*c %*d %*d %*d %*d %*d %*d %ld %ld %ld %*ld %*d %ld %ld %*ld %*d %ld %*d %ld %*d %ld",
    //        &process_pid, &utime, &stime, &time, &nice, &virt_mem);

    // Close the file
    fclose(fp);

    // Print the information with full precision
    printf("%d %.6f %.6f %.6f   %d   %lu\n",
           process_pid, 
           (float)utime / clk_tck,    // Convert utime to seconds
           (float)stime / clk_tck,    // Convert stime to seconds
           (float)(utime + stime) / clk_tck,  // Convert total time to seconds
           nice, 
           virt_mem);
}

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
    // printf("Scheduling Process: PID: %d\n", pid_array[current_process]);

    cycle_count++;
    if (cycle_count % 2 == 0) { // Full cycle completed
        printf("\nPID\tutime\tstime\ttime\tnice\tvirt mem\n");
        for (int i = 0; i < pids; i++) {
            if (pid_array[i] != -1) {
                get_process_info(pid_array[i]);
            }
        }
    }
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
    if (argc != 3) {
        printf("Wrong number of arguments\n");
        exit(1);
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
            fclose(file);
            free(pid_array);
            exit(1);
        }

        args[i] = NULL; // terminates args array
        pid_t pid = fork(); // create a child process for each command from the input file
        
        if (pid < 0) { // fork failed
            perror("Fork failed");
            free(pid_array);
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
