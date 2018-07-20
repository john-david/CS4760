/*
 * John David
 * CS 4760
 * Assignment 6
*/

#include "oss.h"


int user_num;
int shm_id; 
int sema_id;
                                 
int sig;                                     
char msgerr[80] = "";                         
shm_mem *shm_mem_ptr;                                    
                                                              
int mypid; 

void write_log(char *);
void sigHandler(int handle);
int signal_receieved();
void update_clock(int);
void sem_wait(int);
void sem_signal(int);


int main(int argc, char *argv[]){

    srand(time(NULL)); 

    mypid = getpid();                            

    int opt;
    int term_chance;

    while ((opt = getopt(argc, argv, "u:t:")) != -1) {

        switch (opt) {

            case 'u':
                user_num = atoi(optarg);
                fprintf(stderr, "Process #%d started.\n", user_num);
                break;
            
            case 't':
                term_chance = atoi(optarg);
                fprintf(stderr, "Process #%d termination probability set to 0.%d percent\n", user_num, term_chance); 
                break;

            default:
                fprintf(stderr, "Proc %d: Error: insufficient arguments passed. Terminating %d\n", user_num, mypid);
                exit(EXIT_FAILURE);
        }
    }


    //fprintf(stderr, "Proc #%d with %d percent termination chance running\n", user_num, term_chance);

    sprintf(msgerr, "proc #%d forked | PID: %d\n", user_num, mypid);
    write_log(msgerr);


    // connect to shared memory

    int Shm_key  = ftok(".", 91);
    int Sema_key = ftok(".", 92);

    shm_id = shmget(Shm_key, shm_mem_size, 0600 | IPC_CREAT);
    if (shm_id == -1) {
        perror("shmget failed. exiting\n");
        exit(EXIT_FAILURE);
    }

    if ( !(shm_mem_ptr = (shm_mem *)(shmat(shm_id, 0, 0)))) {
        perror("shmat failed. exiting\n");
        exit(EXIT_FAILURE);
    }

    fprintf(stderr, "shared memory allocated.\n");

    if ((sema_id = semget(Sema_key, 1, 0600 | IPC_CREAT)) == -1) {
        perror("semget failed. exiting\n");
        exit(EXIT_FAILURE);
    }





    while(shm_mem_ptr->seconds < 2){

        // check for signals
        if(signal_received()){
            fprintf(stderr, "Received signal %d - exiting...\n", sig);
            break;
        }

        // check for random segfault.
        if((rand() % 1000 + 1) <= term_chance){
            fprintf(stderr, "proc #%d segfaulted and is unexpectedly terminating.\n"); 
            exit(EXIT_FAILURE);
            
        }

            
        //update clock

        sem_wait(sema_id);
        update_clock(1000);
        sem_signal(sema_id);

    }      


    // ending sequence

    sprintf(msgerr, "proc #%d ending | PID: %d", user_num, mypid);
    write_log(msgerr);

    fprintf(stderr, "proc %d ending.", user_num); 

    return 0;
}

void write_log(char *msg) {

    FILE *fp;

    if(!(fp = fopen("oss.log", "a"))){
        perror("failed to open oss.log");
        exit(EXIT_FAILURE);
    }


    fprintf(fp, "proc %d:\t%s\n",user_num,  msg);
    fclose(fp);

}

void sigHandler(int handle) {
    sig = handle;
}

int signal_received() {

    signal(SIGINT, sigHandler);
    signal(SIGTERM, sigHandler);

    if (sig == 2) 
        fprintf(stderr, "proc %d: (SIGINT) received. Exiting\n", user_num);

    return 0;
}

void sem_wait(int sem_id){

    struct sembuf sbuf;
    sbuf.sem_num = 0;
    sbuf.sem_op = -1;
    sbuf.sem_flg = 0;

    if (semop(sem_id, &sbuf, 1) == -1){
            exit(EXIT_FAILURE);
    }

}

void sem_signal(int sem_id){

    struct sembuf sbuf;
    sbuf.sem_num = 0;
    sbuf.sem_op = 1;
    sbuf.sem_flg = 0;

    if (semop(sem_id, &sbuf, 1) == -1){
            exit(EXIT_FAILURE);
    }

}



void update_clock(int extra_time) {

        // add time from extras during loop
        shm_mem_ptr->nanoseconds += extra_time;

        // add cycle time
        shm_mem_ptr->nanoseconds +=  100;

        // update clock
        if ( shm_mem_ptr->nanoseconds >= 1000000000 ) {
            shm_mem_ptr->seconds++;
            shm_mem_ptr->nanoseconds -= 1000000000;
        }

}

