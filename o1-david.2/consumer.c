

/*
 *
 * John David
 * CS 4760
 * Assignment 2
 *
*/

#include "utility.h"

/*

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

*/

#define SIZE 256
//#define true 0;
//#define false 1;

volatile sig_atomic_t done = 0;
int interrupted;


void term(int signum){

        //fprintf(stderr, "\nconsumer: attempting to exit gracefully....\n");
	interrupted = 0;
        done = 1;

}

void normal(int signum){

	//fprintf(stderr, "\nconsumer: ending process normally. \n");
	interrupted = 1;
	done = 1;

}

int main(int argc, char *argv[]){

	char outStr[512];

	/* setup shared memory access */

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

	int consumer_number;

	if (argc != 5){

                fprintf(stderr, "Usage: %s consumer_number\n", argv[0]);
                perror("Error: improper usage >> exiting program >>");
                exit(EXIT_SUCCESS);
        }

	consumer_number = atoi(argv[1]);

	int total_processes = atoi(argv[4]);

	fprintf(stderr, "\nHello, I'm consumer #%d and was set up w/ %d total_processes", consumer_number, total_processes);

	/* file set up */

        FILE *output_file;

	char outfile_name[256];
	strcpy(outfile_name, "cons");
	strcat(outfile_name, argv[1]);
	strcat(outfile_name, ".log");	

        /* Opening output file in write mode */
        if((output_file = fopen(outfile_name, "w")) == NULL){
		fprintf(stderr, "consumer %d :", consumer_number);
	        perror("\nfopen failure. sending kill signal to master.");
                kill(getppid(), SIGTERM);
                exit(EXIT_FAILURE);
        }

        /* Get the current time. */
        curtime = time (NULL);

        /* Convert it to local time representation. */
        loctime = localtime (&curtime);
        strftime (time_string, SIZE, "%I:%M:%S", loctime);

        strcpy(outStr, time_string);
        strcat(outStr, " ");
        strcat(outStr, "Start\n");

        fputs(outStr, output_file);

        fprintf(stderr, "consumer %d: start time %s \n", consumer_number, time_string);	



	int i = consumer_number; // consumer is process # ..
        int loop_sleep = 0;
        char str[10];


        /* local to each process */
        int j;


while (!done){

        while (1 && !done) {
                
		loop_sleep = rand() % 5 + 1;
                fprintf(stderr, "\nConsumer %d: Sleeping %d seconds.", consumer_number, loop_sleep);

                fprintf(stderr, "\ninside consumer  loop");

                while (1) {
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

        fprintf(stderr, "\nInside Consumer Critical Section");

        j = (shared_mem->turn + 1) % total_processes;

        while (shared_mem->flag[j] == IDLE)
                j = (j + 1) % total_processes;

        shared_mem->turn = j;
        shared_mem->flag[i] = IDLE;


        j = (shared_mem->turn + 1) % total_processes;

        while (shared_mem->flag[j] == IDLE)
                j = (j + 1) % total_processes;

        shared_mem->turn = j;
        shared_mem->flag[i] = IDLE;

        fprintf(stderr, "\nInside Consumer Remainder Section");

        }






}

        printf("done.\n");	

        loop_sleep = rand() % 5 + 1;
        fprintf(stderr, "\nConsumer: sleeping %d seconds", loop_sleep);

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

        fprintf(stderr, "consumer %d: termination time %s \n", consumer_number,  time_string);



	fclose(output_file);


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
