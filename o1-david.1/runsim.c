

/*

John David
CS 4760
Assignment 1

runsim program implementation for 3.9 Exercise Process Fans

testing complete, works with commands "proc_fan -n 5" and "proc_fan -n 5 -f testing.data"

*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_CANON 1024

int makeargv(const char *program, const char *delimiters, char ***argvp);

int main(int argc, char *argv[]) {

	/* variables 
 *
 * 	child_pid to track children
 * 	**newargv tracks new arguments for children/array
 *	delimiters space, tab, newline
 *	line_commands for data from testing.data file
 	
	*/

	pid_t child_pid;
	char **newargv;
	char delimiters[] = " \t\n";
	char readline[MAX_CANON];

	/* pr_limit, pr_count per exercise instructs */

	int pr_limit = 0;
	int pr_count = 0;
	
	/* check for one argument, exit w/ error if not */
	if(argc != 2){

		fprintf(stderr, "Usage: %s number\n", argv[0]);
		return 1;

	}

	/* set pr_limit to command line argument */
	pr_limit = atoi(argv[1]);

	fprintf(stderr, "\npr_limit: %d", pr_limit);

	if(pr_limit < 0){
	
		/* negative values are not allowed */

		fprintf(stderr, "error: pr_limit = %d  must be positive, check file input",
			pr_limit);
		perror("Error, stopping program");
		return 1;

	}

	/* main loop */

	while ((fgets(readline, MAX_CANON, stdin)) != NULL) {


		fprintf(stderr, "\nreading in: %s\n", readline);

		/* part a) if pr_count is pr_limit */
		/* wait for child to finish, decrement pr_count */
		if(pr_count >= pr_limit) {

			wait(NULL);
			pr_count--;

		}

		/* if child failed to fork, exit program w/ error */
		/* increment pr_count */
		pr_count ++;

		if(child_pid = fork() < 0) {

			perror("Error: fork has failed");
			return 1;

		}
		/* part b) read a line and exec a program */
		
		if(child_pid == 0) {
			
			/* if makeargv fails exit with error */
			if(makeargv(readline, delimiters, &newargv) < 0){

				fprintf(stderr, "error on pid:%ld\n",
					(long)getpid());
				perror("Error: the child failed to construct an argument");
					return 1;
			}
			
			execvp(*newargv, newargv);
			fprintf(stderr, "execvp failed! pid:%ld\n",
				(long)getpid()); 
			perror("Error: the child's exec call has failed");
			
		}
		else{
		/* part c) increment pr_count to track the number of active children */
			pr_count++;
			while ((child_pid = waitpid(-1, NULL, WNOHANG)))
				if(child_pid == -1)
					fprintf(stderr, "\n ending PID: %ld ", child_pid);
					break;

		}

	
	}

	do{
	
		child_pid = wait(NULL);
		
		if(child_pid == -1)
			break;
	

	}while(1);

	/* end of program */
	return 0;
}
