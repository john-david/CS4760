

/* 
 *
 * John David
 * CS 4760
 * Assignment 1
 *
*/

/* testsim.c */

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char *argv[]){


	int sleep_timer;
	int repeat;

	sleep, repeat = 0;

	if(argc != 3){

		fprintf(stderr, "\nUSAGE ERROR: %s sleep repeat \n", argv[0]);
		perror("\ntestsim: error: improper usage exiting program");
		return 1;

	}	

	if(argc == 3){

		sleep_timer = atoi(argv[1]);
		repeat = atoi(argv[2]);

		fprintf(stderr, "\ntestsim called with %s %d %d",
			argv[0], sleep_timer, repeat);


	}

	if(sleep < 0){

		fprintf(stderr, "\nUSAGE ERROR: %d sleep is invalid\n", sleep_timer); 
		perror("\ntestsim: error: improper usage exiting program");
		return 1;
	}

	do{

		sleep(sleep_timer);
		fprintf(stderr, "\ntestsim: PID: %ld %d \n", (long)getpid(), repeat);
		
		repeat--; 		


	}while(repeat > 0);


	fprintf(stderr, "\nEnding PID: %ld \n", (long)getpid());

	return 0;

}

