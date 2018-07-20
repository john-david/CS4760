

/*
 * John David
 * CS 4760
 * Assignment 3
 *
*/

#include <string.h>
#include "utility.h"

static int total_time;
static int num_users;
static int message_queue_id;


volatile pid_t *pids;
volatile pid_t producer_pid;

static void INThandler(int);

static void ALARMhandler(int);

struct mbuf {
    
    long mtype;     /* message type */
    char mtext[MAX_MTEXT];  /* message body */
    int muser;

};


int main(int argc, char *argv[]){

	signal(SIGALRM, ALARMhandler);
	signal(SIGINT, INThandler);

        int opt;
        int argcount;

	int time_flag;
	int run_time;	

	int process_flag;
        int processes;
	
	
        int file_flag;
        file_flag = 0;
        char *file_name;
        FILE *output_file;   
 
        extern char *optarg;
        extern int optind;


        processes = 0;
        time_flag = 0;
	process_flag = 0;

        printf("Hello, we have %d arguments\n", argc);

/*
	int error;
   	int i;
   	int id, n;
   	sem_t *semlock;
*/

        if(argc > 7)
                perror("proc_fan: Error: Improper usage, more than 7 arguments");



        while ((opt = getopt(argc, argv, "hs:l:t:")) != -1) {

                switch (opt) {

			case 'h':
			fprintf(stderr, "OSS: Help Section \n", argv[0]);
			fprintf(stderr, "OSS: Usage: %s [optional: -s processes -l output_file-name -t run_time]\n",argv[0]);
			exit(0);	
	
                        case 's':
                        processes = atoi(optarg);
                        process_flag = 1;
                        break;

                        case 'l':
                        file_flag = 1;
                        file_name = optarg;
                        fprintf(stderr, "file included: %s \n", file_name);
                        break;

			case 't':
			time_flag = 1;
			run_time = atoi(optarg);
			break;	
		
                        default: /* '?' */

                        fprintf(stderr, "OSS: Error: Usage: %s [optional: -s processes -l output_file-name -t run_time]\n",argv[0]);
			fprintf(stderr, "OSS: For help: %s [-h]\n", argv[0]);
                        exit(EXIT_FAILURE);
               }
           }

        printf("paramters entered: process_flag=%d; processes=%d; time_flag=%d; run_time=%d; optind=%d\n", 
		process_flag,  processes, time_flag, run_time, optind);


	if(process_flag == 0)
	{
		processes = 5;
		fprintf(stderr, "OSS: no user argument for users\n");
		fprintf(stderr, "OSS: default to  %d users \n", processes);
	}
	else
		fprintf(stderr, "OSS: user entered  %d users \n", processes);


	num_users = processes;

    if(file_flag == 1){
            /* file set up */

        char outfile_name[256];
        strcat(outfile_name, file_name);

        /* Opening output file in write mode */
        if((output_file = fopen(outfile_name, "w")) == NULL){
            perror("\nOSS: fopen failure. sending kill signal");
                kill(getppid(), SIGTERM);
                exit(EXIT_FAILURE);
        }


     }
    else{

        /* Opening output file in write mode */
        if((output_file = fopen("oss.log", "w")) == NULL){
            /* if fopen fails */
                perror("\nOSS: fopen failure. sending kill signal");
                kill(getppid(), SIGTERM);
        exit(EXIT_FAILURE);
        }
    }

    fprintf(stderr, "\nOSS: log file has been created\n");




	/* shared memory */

        pid_t pid;

        pids = mmap(0, processes*sizeof(pid_t), PROT_READ|PROT_WRITE,
              MAP_SHARED | MAP_ANONYMOUS, -1, 0);

        if (!pids) {
                perror("mmap failed");
                exit(1);
        }
        memset((void *)pids, 0, processes*sizeof(pid_t));

	int smem_id;
        key_t smem_key = ftok("README", 1);
        if ( 0 > smem_key )
	{
                perror("ftok"); /*Displays the error message*/
                /*Error handling. Return with some error code*/
        }
        else
        /* This is not needed, just for success message*/
        {
                printf("ftok success\n");
        }

        void *temp_ptr = (void *)0;
        struct shared_mem_struct *shared_mem;

        if((smem_id = shmget(smem_key, sizeof(struct shared_mem_struct), IPC_CREAT | 0777))<0){
                perror("shmget");
                exit(EXIT_FAILURE);
        }

        if((temp_ptr = shmat(smem_id, (void *)0, 0)) == (void *)-1)
	{

                perror("shmat");
                exit(EXIT_FAILURE);
        }
        else if (shmctl(smem_id , IPC_RMID , 0) == -1)
        {
                perror("smem_id");
        }

        shared_mem = (struct shared_mem_struct *) temp_ptr;

	/* */

    shared_mem->seconds = 0;
    shared_mem->nanoseconds = 0;

    char outStr[200];
    char append[20];

    strcat(outStr, "Clock initialized: ");
    sprintf(append, "%ld", shared_mem->seconds);
    strcat(outStr, append);
    strcat(outStr, " seconds ");
    sprintf(append, "%ld", shared_mem->nanoseconds);
    strcat(outStr, append);
    strcat(outStr, " nanoseconds \n");

    fprintf(stderr, "%s", outStr);

    fputs(outStr, output_file);

	char parameter1[32];
        char parameter2[32];
        char parameter3[32];
        char parameter4[32];


        //sprintf(parameter1," %d", processes);
        sprintf(parameter2," %d", smem_key);
        sprintf(parameter3," %d", smem_id);
        

    /* SET MESSAGE QUEUE  */

    /* queue for oss to recieve */

    int msqid;
    long lkey;
    key_t key;

    key = ftok("README", 1);

    if ((msqid = msgget(key, IPC_CREAT | 0600)) < 0) {
        perror("OSS: Error: mssget");
        exit(errno);
    }

    fprintf(stderr, "\nMessage Queue id#: %d\n", msqid);
    
    message_queue_id = msqid;

    /* add msqid to parameters list to send to children */
    sprintf(parameter4," %d", msqid);

    /* queue for users to send */



	/* start users */
        int c;
        for (c = 0; c < processes; c++) {

                pid = fork();

                if (pid == 0) {

                        sprintf(parameter1, "%d", c + 1);

                        char *args[] = {"./user", parameter1, parameter2, parameter3, parameter4, NULL};    /* each element represents a command line argument */

                        fprintf(stderr, "About to start user # %d \n", c + 1);
                        execvp(args[0], args);
                        /* if we reach this point, exec has failed */
                        perror("execvp failed\n");
                        exit(1);

                } else if (pid < 0) {
                        perror("fork failed\n");
                } else {
                        pids[c] = pid;
                        //sleep(1);
                }
        }


	fprintf(stderr, "\nfinished starting users\n");

        /* setting up processing timer */

	if(time_flag == 0)
	{        	
		fprintf(stderr, "OSS: no user argument for timer \n");
		fprintf(stderr, "OSS: default to 20 seconds \n");
		run_time = 20;
		total_time = 20;
	}
	else
	{
		total_time = run_time;
		fprintf(stderr, "OSS: user entered %d seconds \n", run_time);
	}

        fprintf(stderr, "\nOSS: starting %d second countdown clock\n", run_time);


        alarm(run_time);
        int counter = run_time;

        pid_t ppid;
        ppid = getpid();



    strcat(outStr, "Users started\n");

    fputs(outStr, output_file);


        /* place holder for while loop with sleep(1) counting down */

    size_t maxBytes;
    struct mbuf msg;
    ssize_t msgLen;
    int type;

    /* set for breaking apart user message. */

    char buf[MAX_MTEXT];
    char token[3];
    const char delim[2] = "/";

        int i = 0;

    int total_users = processes;
    int user_to_kill;

    char time1[5];
    char time2[5];

    int user_pid;

/* MAIN LOOP */

        while (1) {
		                
		    //fprintf(stderr, "OSS: %d seconds left \n", counter--);
		    //sleep(1);
        
        /* Now we wait for signals */

            fprintf(stderr, "OSS: Waiting for a message\n ");
            msgrcv(msqid, &msg, MAX_MTEXT, 0, 0); 
            
        /* msg received if we reach this point */
        
        /* msg format: <User #> <time rolled> <time expired> */

            fprintf(stderr, "\nOSS: Recieved: %s \n", msg.mtext);    
        
        msgLen = strlen(msg.mtext);

        memcpy(buf, msg.mtext, msgLen);
 
        user_to_kill = msg.muser;

        fprintf(stderr, "rec'd %d \n", user_to_kill);

        fprintf(stderr, "OSS: Killing User # %d \n", user_to_kill);        

        // user_pid = pids[user_to_kill - 1];
        
        //kill(pids[user_to_kill - 1], SIGINT);

        sprintf(parameter1, "%d", user_to_kill - 1);

/*
        pid = fork();

        if (pid == 0) {

                  sprintf(parameter1, "%d", c + 1);

                  char *args[] = {"./user", parameter1, parameter2, parameter3, parameter4, NULL};

                  fprintf(stderr, "OSS: About to RESTART user # %d \n", c + 1);
                  execvp(args[0], args);
                
                  perror("execvp failed\n");
                  exit(1);

         } else if (pid < 0) {
                  perror("fork failed\n");
         } else {
                  pids[c] = pid;
         }
         
         total_users++;   
         fprintf(stderr, "OSS: %d users have been started.\n", total_users);

         if ( total_users == 100 ){
            fprintf(stderr, "\nOSS: 100 processes have been run. Ending simulation.\n"); 
            break; 
         }

*/

        }



    memset(outStr,0,strlen(outStr));

    strcat(outStr, "OSS process ending\n");
    fputs(outStr, output_file);

    fclose(output_file);

	fprintf(stderr, "cleaning up shared memory\n");

	shmdt(shared_mem);
        shmctl(smem_id, IPC_RMID, NULL);

    fprintf(stderr, "cleaning up message queue\n");
    
    if(msgctl(msqid, IPC_RMID, NULL) == -1){
        perror("execvp failed\n");
        exit(1);
    }

	fprintf(stderr, "OSS: exiting\n");	

return 0;
}


static void  INThandler(int sig)
{
        int signal_check;
        fprintf(stderr, "\nctrl^c signal received. exiting program. \n");

        int c;
        /* killing the children */
        for (c = 0; c < num_users; c++) {
                fprintf(stderr, "killing child number %d PID: %d \n", c + 1, pids[c]);
                signal_check = kill(pids[c], SIGINT);

                while(signal_check != 0){
                        fprintf(stderr, "waiting on child number %d PID: %d \n", c + 1, pids[c]);
                        waitpid(-1, NULL, WNOHANG);
                }

                if(signal_check == 0)
                        fprintf(stderr, "finished killing child number %d PID: %d \n", c + 1, pids[c]);
        }

        fprintf(stderr, "OSS: cleaning up shared memory\n");

	/* Remove the deatached shared memory and message queue */
        shmdt(shared_mem);
        shmctl(smem_id, IPC_RMID, NULL);


        fprintf(stderr, "cleaning up message queue\n");

        if(msgctl(message_queue_id, IPC_RMID, NULL) == -1){
            perror("execvp failed\n");
            exit(1);
        }


        fprintf(stderr, "OSS: exiting");
        exit(EXIT_SUCCESS);
}


static void ALARMhandler(int sig)
{
        fprintf(stderr, "\n%d Seconds has expired. \n", total_time);
        
        int signal_check;

        int c;
        /* killing the children */
        for (c = 0; c < num_users; c++) {
                fprintf(stderr, "killing child number %d PID: %d \n", c + 1, pids[c]);
                signal_check = kill(pids[c], SIGTERM);

                while(signal_check != 0){
                        fprintf(stderr, "waiting on child number %d PID: %d \n", c + 1, pids[c]);
                        waitpid(-1, NULL, WNOHANG);
                }


                if(signal_check == 0)
                        fprintf(stderr, "finished killing child number %d PID: %d \n", c + 1, pids[c]);
        }

        fprintf(stderr, "OSS: cleaning up shared memory\n");

	/* Remove the deatached shared memory and mesage queue */
        shmdt(shared_mem);
        shmctl(smem_id, IPC_RMID, NULL);

        fprintf(stderr, "cleaning up message queue\n");

        if(msgctl(message_queue_id, IPC_RMID, NULL) == -1){
            perror("execvp failed\n");
            exit(1);
        }


        fprintf(stderr, "OSS: exiting\n");
        exit(EXIT_SUCCESS);
}
