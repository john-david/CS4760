

/*
 * John David
 * CS 4760
 * Assignment 2
*/


#include "utility.h"

/*

#include <sys/mman.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/wait.h>

*/

/* variables to keep track of consumers */

#define MAX_PIDS 20
//#define KEY_CODE 1234

static int num_consumers;

volatile pid_t *pids;
volatile pid_t producer_pid;

static void  INThandler(int sig)
{
	int signal_check;
	fprintf(stderr, "\nctrl^c signal received. exiting program. \n");
	
	/* code to clean up */
	

	/* killing the producer */
	signal_check = kill(producer_pid, SIGINT);
	
	if(signal_check == 0)
		fprintf(stderr, "The producer is terminating via kill..\n");

	int c;
	/* killing the children */
	for (c = 0; c < num_consumers; c++) {
   		fprintf(stderr, "killing child number %d PID: %d \n", c + 1, pids[c]);
		signal_check = kill(pids[c], SIGINT);
        
		while(signal_check != 0){
			fprintf(stderr, "waiting on child number %d PID: %d \n", c + 1, pids[c]);
			waitpid(-1, NULL, WNOHANG);
		}

        	if(signal_check == 0)
                	fprintf(stderr, "finished killing child number %d PID: %d \n", c + 1, pids[c]);
  	}

	exit(EXIT_SUCCESS);
}

static void ALARMhandler(int sig)
{
    	fprintf(stderr, "\n100 Seconds has expired. \n");

	/* code to clean up */

	int signal_check;
        /* killing the producer */
        signal_check = kill(producer_pid, SIGTERM);

        if(signal_check == 0)
                fprintf(stderr, "The producer is closing normally..\n");

        int c;
        /* killing the children */
        for (c = 0; c < num_consumers; c++) {
                fprintf(stderr, "killing child number %d PID: %d \n", c + 1, pids[c]);
                signal_check = kill(pids[c], SIGTERM);
		
		while(signal_check != 0){
                        fprintf(stderr, "waiting on child number %d PID: %d \n", c + 1, pids[c]);
                        waitpid(-1, NULL, WNOHANG);
                }

		
                if(signal_check == 0)
                        fprintf(stderr, "finished killing child number %d PID: %d \n", c + 1, pids[c]);
        }




    	exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[]){

	/* shared memory section for bounded buffer array, per assignment using shmget */

	char my_string[200];


	/* shared memory section for multiple consumers, for variety using new mmap std  */


	int smem_id;
	key_t smem_key = ftok("data.input", 1);
	if ( 0 > smem_key ){
       		perror("ftok"); /*Displays the error message*/
       		/*Error handling. Return with some error code*/
    	}
    	else 
	/* This is not needed, just for success message*/
    	
       		printf("ftok success\n");
    	}



    	//key_t smem_key = KEY_CODE;
    	void *temp_ptr = (void *)0;
    	struct shared_mem_struct *shared_mem;

	if((smem_id = shmget(smem_key, sizeof(struct shared_mem_struct), IPC_CREAT | 0777))<0){   

		// if shmget fails
	        perror("shmget");
	        exit(EXIT_FAILURE);
	}

	/* Attaching the shared memory to data space */
	if((temp_ptr = shmat(smem_id, (void *)0, 0)) == (void *)-1){   
		
	        perror("shmat");
	        exit(EXIT_FAILURE);
	}
	else if (shmctl(smem_id , IPC_RMID , 0) == -1)
    	{
        	perror("smem_id");
    	}
		
	shared_mem = (struct shared_mem_struct *) temp_ptr;
        
	//shared_mem->proc_id = 0;  	
	//shared_mem->flag[0] = WANT_IN;

	//fprintf(stderr, "\n\nproc_id: %d flag %d", shared_mem->proc_id, shared_mem->flag[0] );
	//strcpy(shared_mem->bounded_buffer[2], "\nhello, I'm a string");

	

	fprintf(stderr, "\nbuffer: %s ", shared_mem->bounded_buffer[2]);	
	
	int consumers;

	pid_t pid;
	
	pids = mmap(0, MAX_PIDS*sizeof(pid_t), PROT_READ|PROT_WRITE,
              MAP_SHARED | MAP_ANONYMOUS, -1, 0);
  	
	if (!pids) {
    		perror("mmap failed");
    		exit(1);
	}
	memset((void *)pids, 0, MAX_PIDS*sizeof(pid_t));


	if (argc != 2){

		fprintf(stderr, "Usage: %s number_of_consumers\n", argv[0]);
		perror("Error: improper usage >> exiting program >>\n");
		exit(EXIT_SUCCESS);
	}

	consumers = atoi(argv[1]);
	//num_consumers = 0;
	num_consumers = consumers;	

	if (consumers >= 20){
		fprintf(stderr, "The number of consumers cannot exceed 20.\n");
		perror("Error: improper usage >> exiting program >> \n");
		exit(EXIT_SUCCESS);
	}	


	/* setting up signal handler to exit upon receipt of ctrl^c */
	signal(SIGINT, INThandler);

	/* start producer */

	int z;


	for(z = 0; z < 21; z++){
                //fprintf(stderr, "\n %d setting flags to IDLE ", z);
                shared_mem->flag[z] = IDLE;
        }

	sprintf(my_string, "empty");

	for(z= 0; z < 5; z++){
		fprintf(stderr, "\n %d setting bounded buffer ", z);
		memcpy(shared_mem->bounded_buffer[z], "", strlen(""));
		fprintf(stderr, "\n %s \n", shared_mem->bounded_buffer[z]);
	}	

	char parameter[32];
	char parameter2[32];
	char parameter3[32];
	char parameter4[32];
	
	int total_processes = num_consumers + 1;

	sprintf(parameter4, "%d", total_processes);
	sprintf(parameter, "%d", num_consumers + 1);
	sprintf(parameter2," %d", smem_key);
	sprintf(parameter3," %d", smem_id);

	pid = fork();
	
	if(pid == 0){

		char *args[] = {"./producer", parameter, parameter2, parameter3, NULL}; /* each element represents a command line argument */

                fprintf(stderr, "About to start the producer \n");
                execvp(args[0], args);
                /* if we reach this point, exec has failed */
                perror("execvp failed\n");
                exit(1);

	} else if (pid < 0) {
              	perror("fork failed\n");
        } else {
                producer_pid = pid;
                sleep(1);
        }



        //char parameter[32];
        //sprintf(parameter, "%d", i);	

	/* start consumers */
	int c;
	for (c = 0; c < consumers; c++) {
    		
		pid = fork();
    		
		if (pid == 0) {

			sprintf(parameter, "%d", c + 1);			

			char *args[] = {"./consumer", parameter, parameter2, parameter3, parameter4,  NULL};	/* each element represents a command line argument */

			fprintf(stderr, "About to start consumer # %d \n", c + 1);
			execvp(args[0], args);
			/* if we reach this point, exec has failed */
			perror("execvp failed\n");		
			exit(1);	
      			
    		} else if (pid < 0) {
      			perror("fork failed\n");
    		} else {
			pids[c] = pid;
      			sleep(1);
    		}
  	}


	/* setting up 100 second timer */
        fprintf(stderr, "\nMaster: starting 100 second countdown clock\n");
        signal(SIGALRM, ALARMhandler);

	alarm(20);
	int counter = 100;
	
	pid_t ppid;
	ppid = getpid();

	/* place holder for while loop with sleep(1) counting down */
	
	fprintf(stderr, "\ni'm the parent process, my pid is %d", ppid);

	// wait(0); 
	while (1) {
		int status;
		pid_t done = wait( &status );
		if ( done == -1 )
			break;
	}



	/* remove shared memory and clean up */
	
	fprintf(stderr, "\nCleaning up and removing shared memory segments.\n");

        
	/* deatach the shared memory */
        if(shmdt(shared_mem) == -1)
        {
            perror("shmdt\n");
            exit(EXIT_FAILURE);
        }

        /* Remove the deatached shared memory */
        if(shmctl(smem_id, IPC_RMID, 0) == -1)
        {
            perror("shmctl\n");
            exit(EXIT_FAILURE);
        }

	char command[50];

   	strcpy(command, "./removeMem.sh" );
   	system(command);

	return 0;
}
