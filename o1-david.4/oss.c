

/*
 * John David
 * CS 4760
 * Assignment 4
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
    int processes = 100;
	

	
    int file_flag;
    file_flag = 0;
    char *file_name;
    FILE *output_file;   
 
    extern char *optarg;
    extern int optind;


        
    /* Opening output file in write mode */
    if((output_file = fopen("oss.log", "w")) == NULL){
        /* if fopen fails */
        perror("\nOSS: fopen failure. sending kill signal");
        kill(getppid(), SIGTERM);
        exit(EXIT_FAILURE);
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

/*

    int c;
    for (c = 0; c < processes; c++) {

                pid = fork();

                if (pid == 0) {

                        sprintf(parameter1, "%d", c + 1);

                        char *args[] = {"./user", parameter1, parameter2, parameter3, parameter4, NULL}

                        fprintf(stderr, "About to start user # %d \n", c + 1);
                        execvp(args[0], args);
                    
                        perror("execvp failed\n");
                        exit(1);

                } else if (pid < 0) {
                        perror("fork failed\n");
                } else {
                        pids[c] = pid;
                        //sleep(1);
                }
        }


*/

    /* set run time for 2 seconds */

    run_time = 10;

    alarm(run_time);
    int counter = run_time;

    pid_t ppid;
    ppid = getpid();

    strcat(outStr, "OSS: started\n");

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

    int user_to_kill;

    char time1[5];
    char time2[5];

    int user_pid;

/* MAIN LOOP */
    int c = 0;
    int max_procs = 100;
    
    int rand_interval;

    int maxTimeBetweenNewProcsNS = 1;
    int maxTimeBetweenNewProcsSecs = 1;
    
    int set_proc_this_cycle = 0;

    /* test proc */
    int solo = 0;

    int quantum;



    while(max_procs > 0){

        rand_interval = random_num(5);
        //fprintf(stderr, "OSS: random interval: %d simulated seconds\n", rand_interval);

        shared_mem->nanoseconds += 5000;

        if(shared_mem->nanoseconds == 1000000000){
            fprintf(stderr, "here\n");
            shared_mem->seconds += 1;
        }

        //fprintf(stderr, "OSS: clocktime: %d seconds, %d nanoseconds\n", shared_mem->seconds, shared_mem->nanoseconds);
        
        if(solo < 1){

            quantum = random_num(100);

            shared_mem->process_control_block[1].quantum = quantum;

    
            solo = 1;
            set_proc_this_cycle = 1;
            
            fprintf(stderr, "OSS: forking a new user\n");

            pid = fork();

            if (pid == 0) {

                sprintf(parameter1, "%d", c + 1);

                char *args[] = {"./user", parameter1, parameter2, parameter3, parameter4, NULL};

                fprintf(stderr, "About to start user # %d \n", c + 1);
                execvp(args[0], args);
                    
                perror("execvp failed\n");
                exit(1);

            } else if (pid < 0) {
                perror("fork failed\n");
            } else {
                pids[c] = pid;
                num_users = c; 
            }


        }


        if(set_proc_this_cycle == 1){
            max_procs -= 1;
            set_proc_this_cycle = 0;
            fprintf(stderr, "OSS: process %d started\n", c + 1);
            c += 1; 
        }

        if(shared_mem->seconds == 2){
            fprintf(stderr, "OSS: 2 simulated seconds have passed. Exiting.\n");
            break;
        }

        if(max_procs == 0){
            fprintf(stderr, "OSS: 100 processes have been completed. Exiting.\n");
            break;
        }

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
