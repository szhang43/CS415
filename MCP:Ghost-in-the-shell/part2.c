#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>

void signaler(pid_t* pid_ary, int size, int signal)
{
	// sleep for three seconds

	sleep(1);
    const char *signal_name = strsignal(signal);
	for(int i = 0; i < size; i++)
	{
		// print: Parent process: <pid> - Sending signal: <signal> to child process: <pid>
		// send the signal
		 printf("Parent process: %d - Sending signal: %s to child process: %d\n", 
         getpid(), signal_name, pid_ary[i]);

		if(kill(pid_ary[i], signal) == -1){
            perror("Failed to send signal!");
        };
	}
}

int countLine(char *filename){
    int totalLines = 0;
    char lines[1024];
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error reading file!");
        exit(1);
    }
    while(fgets(lines, sizeof(lines), file)){
        totalLines++;
    }
    fclose(file);
    return totalLines;

}

int main(int argc,char*argv[])
{
	if (argc > 3)
	{
		printf ("Wrong number of arguments\n");
		exit (0);
	}
	int count = 0;
	int pids = 0;

    if(strcmp(argv[1], "-f") != 0){
		printf("Include -f flag to use filemode!\n");
		exit(1);
	}

    // Open file
    FILE *file = fopen(argv[2], "r");
    if(file == NULL){
        perror("Error reading file!");
        exit(1);
    }
    char readCommand[1024]; //single command line from input file
    int commandLine = countLine(argv[2]);
	pid_t *pid_array = malloc(commandLine * sizeof(pid_t));

	sigset_t sigset;
	int sig;
    sigemptyset(&sigset);

	// use sigaddset() to add the SIGUSR1 signal to the set
	sigaddset(&sigset, SIGUSR1);

	// use sigprocmask() to add the signal set in the sigset for blocking
	sigprocmask(SIG_BLOCK, &sigset, NULL);	

	// printf("-----------------------------------------\n");
    while(fgets(readCommand, sizeof(readCommand), file)){ // reads each line from the input until NULL
		size_t len = strlen(readCommand); // This block removed the newline character
        if (len > 0 && readCommand[len - 1] == '\n') {
            readCommand[len - 1] = '\0';
        }
		
        char *command = strtok(readCommand, " "); // gets the command from the read line
		char *args[1024]; // create an array to store the individual commands plus arguments
        int i = 1; // increment by 1 since strtok skips the first one
		count++; // count how many flags and parameters are there
		if (command != NULL) { // check if command is null
			args[0] = command; // Store the command in args[0]
			// printf("Command(args[0]) : %s\n", args[0]);
			while(command != NULL){ // run until it has reached the end of the line
				command = strtok(NULL, " ");
				if(command != NULL){
					args[i] = command; 
					// printf("args[%d]: %s\n", i, args[i]);
					i++;
				} 
			}
		} else {
			printf("Failed to read commands from input file!\n");
			exit(1);
		}

		// printf("-----------------------------------------\n");
		args[i] = NULL; // terminates args array

		pid_t pid = fork(); // create a child process for each command from input file
		if (pid < 0) { // fork failed in some way...
			printf("Failed to allocate memory for pid array\n");
			fclose(file);
			exit(1);
		} else if(pid == 0){ // fork has successfully created a child process
            // send SIGUSR1 
            printf("Child Process: %d - Waiting for SIGUSR1...\n", getpid());

			// wait for the signal
			if(sigwait(&sigset, &sig)){
                perror("Sigwait failed!");
                fclose(file);
                exit(1);
            };

			// print: Child Process: <pid> - Received signal: SIGUSR1 - Calling exec().
			printf("Child Process: %d - Received signal: SIGUSR1 - Calling exec().\n", getpid());
			
            if(execvp(args[0], args)){
				printf("Execvp Failed for command : %s\n", args[0]);
                if(pid_array) free(pid_array);
				fclose(file);
				exit(1);	
			}
		} else { // I don't really understand how this works actually...
			pid_array[pids++] = pid;
		}
	}
	// send SIGUSR1 
	signaler(pid_array, pids, SIGUSR1);

	// send SIGSTOP 
	signaler(pid_array, pids, SIGSTOP);

	// send SIGCONT
	signaler(pid_array, pids, SIGCONT);

	for(int i = 0; i < pids; i++){
		if(waitpid(pid_array[i], NULL, 0) == -1){
            perror("Error waiting for child processes!");
            exit(1);
        };
	}

	free(pid_array);
	fclose(file);
	return 0;
}



