


#include "utility.h"

/*

#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

*/

#define SIZE 256
//#define false 1
//#define true 1
int interrupted;

volatile sig_atomic_t done = 0;
 
void term(int signum){

	//fprintf(stderr, "\nproducer: attempting to exit gracefully....\n");
	interrupted = 0;
    	done = 1;

}

void normal(int signum){

	//fprintf(stderr, "\nproducer: ending process normally...\n");
	interrupted = 1;
	done = 1;
}

int main(int argc, char *argv[]){

	char outStr[512];
	char *emptyString = "";	
	
	/* shared memory setup */

	key_t smem_key = atoi(argv[2]);
	int smem_id = atoi(argv[3]);


	/* Creating shared memory segment */


        if((smem_id = shmget(smem_key, sizeof(struct shared_mem_struct), IPC_CREAT | 0666))<0)
        {   // if shmget fails
            perror("shmget");
            exit(EXIT_FAILURE);
        }

 
      
        if((temp_ptr = shmat(smem_id, (void *)0, 0)) == (void *)-1)
        {   // if shmat fails
            perror("shmat");
            exit(EXIT_FAILURE);
        }
  

      
        /* Typecast the shared memory block to our structure */
        shared_mem = (struct shared_mem_struct *) temp_ptr;
	

	/* Attaching the shared memory to data space */

	int tester = 5;

        //shared_mem->proc_id = 0;
        //shared_mem->flag[tester] = WANT_IN;

	//fprintf(stderr, "\ntesting");

	//fprintf(stderr, "\nProducer: flag Test # 1:  %d", shared_mem->flag[tester] );

	//shared_mem->flag[tester] = WANT_IN;

        //fprintf(stderr, "\nProducer: flag Test # 2:  %d", shared_mem->flag[tester] );
        //strcpy(shared_mem->bounded_buffer[2], "\nproducer: hello, I'm a string");
        //fprintf(stderr, "\nBuffer: %s ", shared_mem->bounded_buffer[2]);



	int total_processes = atoi(argv[1]);
	
	fprintf(stderr, "\nProducer: received %d total_processes\n", total_processes);
	
	/* clock/time set up */

	char time_string[SIZE];
  	time_t curtime;
  	struct tm *loctime;

	/* signal handling for graceful exit */

	interrupted = 1;

	struct sigaction action1;
    	memset(&action1, 0, sizeof(struct sigaction));
    	action1.sa_handler = term;
    	sigaction(SIGINT, &action1, NULL);
	//sigaction(SIGTERM, &action, NULL);

	struct sigaction action2;
        memset(&action2, 0, sizeof(struct sigaction));
        action2.sa_handler = normal;
        sigaction(SIGTERM, &action2, NULL);

	fprintf(stderr, "\nHello, I'm the producer. \n");

	/* file set up */

	FILE *input_file, *output_file;

	/* opening the input file in read mode */

	if((input_file = fopen("data.input", "r")) == NULL)
		{   // if fopen fails
		    perror("\nproducer fopen failure on data.input file. sending kill signal to master.");
		    exit(EXIT_FAILURE);
		}

	/* Opening output file in write mode */
        if((output_file = fopen("prod.log","w")) == NULL){   
	    	/* if fopen fails */
            	perror("\nproducer fopen failure. sending kill signal to master.");
            	kill(getppid(), SIGTERM);
		exit(EXIT_FAILURE);
        }

	fprintf(stderr, "\nproducer log file has been created");

	
	/* Get the current time. */
  	curtime = time (NULL);

  	/* Convert it to local time representation. */
  	loctime = localtime (&curtime);
	strftime (time_string, SIZE, "%I:%M:%S", loctime);

	strcpy(outStr, time_string);
	strcat(outStr, " ");
	strcat(outStr, "Start\n");

	fputs(outStr, output_file);

	fprintf(stderr, "\nproducer: start time %s \n", time_string);
	
	int i = 0; // producer is process #0
	int loop_sleep = 0;
	int n = 21;
	int z;	
	char str[10];	

	char my_string[200];
	char file_buffer[200];
	int index = 0;
	shared_mem->buffer_size = 0;

	/* local to each process */
	int j;

while(!done){

	while (1 && !done) {

		fprintf(stderr, "\ninside producer loop");

  		while (1 && !done) {
    		shared_mem->flag[i] = WANT_IN;
    		j = shared_mem->turn;
    	
		while (j != i) {
      			if (shared_mem->flag[j] != IDLE)
        			j = shared_mem->turn;
      			else
        			j = (j + 1) % total_processes;
    		}
    		shared_mem->flag[i] = IN_CS;
    		j = 0;
    	
		while ((j < total_processes) && ( j == i || shared_mem->flag[j] != IN_CS))
      			j++;
    		if ((j >= total_processes) && (shared_mem->turn == i || shared_mem->flag[shared_mem->turn] == IDLE))
      			break;
  		}
  
	shared_mem->turn = i;

/*
 *	CRITICAL SECTION
 *
*/

	fprintf(stderr, "\nProducer: Inside Critical Section");	

	while(index <= BUFFERSIZE && shared_mem->buffer_size <= BUFFERSIZE){

		if(shared_mem->bounded_buffer[index] == ""){

			fprintf(stderr, "\nfile loop test");

			while (fgets(file_buffer, sizeof(file_buffer), input_file) != NULL && index <= BUFFERSIZE){			

			fprintf(stderr, "\ntesting");
			fprintf(stderr, "\nProducer: Reading %s \n", file_buffer);
			sprintf(my_string, file_buffer);
			memcpy(shared_mem->bounded_buffer[index], my_string, strlen(my_string));
			}

		}
		index++;
		shared_mem->buffer_size = index;
	}	
	
	if (index == BUFFERSIZE && shared_mem->buffer_size == BUFFERSIZE)
               	fprintf(stderr, "Producer: Buffer is full");
            
	shared_mem->buffer_size = 0;
	index = 0;
        while (index < BUFFERSIZE){
               	fprintf(stderr, "buffer slot #%d: %s ", index,  shared_mem->bounded_buffer[index]);
          	fprintf(stderr, "\n");
		index++;
	}

		

	j = (shared_mem->turn + 1) % total_processes;

	while (shared_mem->flag[j] == IDLE)
     		j = (j + 1) % total_processes;

	shared_mem->turn = j;
	shared_mem->flag[i] = IDLE;

/*
 *	REMAINDER SECTION
 *
*/

	fprintf(stderr, "\nProducer: Inside Remainder Section");

	loop_sleep = rand() % 5 + 1;
        fprintf(stderr, "\nproducer: sleeping %d seconds", loop_sleep);

        sleep(loop_sleep);
	
	}

}
	fprintf(stderr, "\nProducer: Outside Remainder Section");

	loop_sleep = rand() % 5 + 1;

	fprintf(stderr, "\nproducer: resting %d seconds for clean up.", loop_sleep);

	sleep(loop_sleep);


	
	/* closing file routine */	

	/* Get the current time. */
        curtime = time (NULL);

        /* Convert it to local time representation. */
        loctime = localtime (&curtime);
        strftime (time_string, SIZE, "%I:%M:%S", loctime);

	char reason[SIZE];
	
	if(interrupted == 0){

		strcpy(reason, "killed");

	}else{

		strcpy(reason, "normal");
	
	}


        strcpy(outStr, time_string);
        strcat(outStr, " ");
        strcat(outStr, "Terminated");
	strcat(outStr, " ");
	strcat(outStr, reason);
	strcat(outStr, "\n");

        fputs(outStr, output_file);

        fprintf(stderr, "\nproducer: termination time %s \n", time_string);
	


	fclose(output_file);
	fclose(input_file);


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




	return 0;

}
