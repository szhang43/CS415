#include<stdio.h>
#include <sys/types.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/wait.h>

void script_print (pid_t* pid_ary, int size);

int main(int argc,char*argv[])
{
	if (argc != 2)
	{
		printf ("Wrong number of arguments\n");
		exit (0);
	}

	int count = atoi(argv[1]);
	pid_t *pid_array = malloc(count * sizeof(pid_t));

	if (pid_array == NULL) {
		printf("Failed to allocate memory for pid array\n");
		exit(0);
	}

	for(int i = 0; i < count; i++){
		pid_t pid = fork();
		pid_array[i] = pid;
		if(pid == -1){
			printf("Fork failed\n");
			exit(0);
		}else if(pid == 0){
			char *args[] = {"./iobound", "-seconds", "5", NULL};
			execvp(args[0], args);

			exit(0);
		}
	}

	script_print(pid_array, count);

	for(int i = 0; i < count; i++){
		waitpid(pid_array[i], NULL, 0);
	}

	free(pid_array);
	/*
	*	TODO
	*	#1	declare child process pool
	*	#2 	spawn n new processes
	*		first create the argument needed for the processes
	*		for example "./iobound -seconds 10"
	*	#3	call script_print
	*	#4	wait for children processes to finish
	*	#5	free any dynamic memory
	*/

	return 0;
}


void script_print (pid_t* pid_ary, int size)
{
	FILE* fout;
	fout = fopen ("top_script.sh", "w");
	fprintf(fout, "#!/bin/bash\ntop");
	for (int i = 0; i < size; i++)
	{
		fprintf(fout, " -p %d", (int)(pid_ary[i]));
	}
	fprintf(fout, "\n");
	fclose (fout);

	char* top_arg[] = {"gnome-terminal", "--", "bash", "top_script.sh", NULL};
	pid_t top_pid;

	top_pid = fork();
	{
		if (top_pid == 0)
		{
			if(execvp(top_arg[0], top_arg) == -1)
			{
				perror ("top command: ");
			}
			exit(0);
		}
	}
}

