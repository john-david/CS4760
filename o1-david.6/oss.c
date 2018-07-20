/*
 * John David
 * CS 4760
 * Assignment 6
*/

#include "oss.h"

int shm_id;
int sema_id;
shm_mem *shm_mem_ptr;
char msgerr[80] = "";  

int oss_mq_id;
int proc_mq_id; 

int user;

int proc_pids[max_procs] = { 0 }; 


void write_log(char *);
void sem_wait(int);
void sem_signal(int);
int kill_proc(int, int);
void cleanup(int);
void update_clock(int);

int new_proc(int, int);


int main(int argc, char *argv[]){

    fprintf(stderr, "Welcome to OSS - memory management.\n");

    int i;
    int opt;
    int process_flag = 0;
    int processes;
    int term_flag = 0;
    int term_chance = 0;

    while ((opt = getopt(argc, argv, "hu:t:")) != -1) {

        switch (opt) {

            case 'h':
                fprintf(stderr, "OSS: Help Section \n", argv[0]);
                fprintf(stderr, "OSS: Usage: %s [optional: -u max_processes]\n",argv[0]);
                exit(0);

            case 'u':
                processes = atoi(optarg);
                
                if(processes > 18){
                    fprintf(stderr, "OSS: Error: No more than 18 processes can be requested. \n");
                    exit(EXIT_FAILURE);            
                }

                process_flag = 1;
                break;

            case 't':
                term_chance = atoi(optarg);

                if(term_chance > 100 | term_chance < 1){
                    fprintf(stderr, "OSS: Error: segfault chance must be between 1-100\n");
                    exit(EXIT_FAILURE);
                }
    
                term_flag = 1;
                break;

            default: /* '?' */
                fprintf(stderr, "OSS: Error: Usage: %s [optional: -u processes -t segfault % (1-100)]\n", argv[0]);
                fprintf(stderr, "OSS: For help: %s [-h]\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    if(process_flag == 1){
        fprintf(stderr, "arguments set: %d max_processes\n", processes);
    }
    else{
        processes = 18;
        fprintf(stderr, "no max on processes set, default value [%d] being used.\n", processes);
    }

    if(term_flag == 1){
        fprintf(stderr, "termination chance set: process segfault probability: [0.%d%]\n", term_chance);
    }
    else{
        term_chance = ( rand() % 4 + 1 );
        fprintf(stderr, "no segfault probability set, randomly generated value [0.%d%] being used.\n", term_chance);
    }

    srand(time(NULL));

    int Shm_key  = ftok(".", 91);        
    int Sema_key = ftok(".", 92);          
    key_t oss_key = 2112;    
    key_t proc_key = 2113;

    if((oss_mq_id = msgget(oss_key, IPC_CREAT | 0777)) == -1) {
        perror("oss msgget for oss queue");
        exit(-1);
    }

    if((proc_mq_id = msgget(proc_key, IPC_CREAT | 0777)) == -1) {
        perror("proc msgget for proc queue");
        exit(-1);
    }

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

    // init semaphores

    union semun { 
        int val; 
        struct semid_ds *buf; 
        unsigned short * array; 
    } argument;

    argument.val = 1;

    if (semctl(sema_id, 0, SETVAL, argument) == -1) {
        sprintf(msgerr, "oss: semctl (shared mem)");
        perror(msgerr);
        exit(EXIT_FAILURE);
    }

    fprintf(stderr, "semaphore set up.\n");

    sem_wait(sema_id);                       
    shm_mem_ptr->seconds = 0;
    shm_mem_ptr->nanoseconds = 0;
    sem_signal(sema_id);

    // initialize proc_used in shared mem

    for (i = 0; i < max_procs; i++) 
        shm_mem_ptr->procs_used[i] = 0;



    int count = 0;
    
    int next_proc_secs = 0;
    long int next_proc_nano = 0;
    int current_time_secs = 0;
    long int current_time_nano = 0;

//
// MAIN LOOP
//

    while(shm_mem_ptr->seconds < 2){

        sem_wait(sema_id);
        current_time_secs = shm_mem_ptr->seconds;
        current_time_nano = shm_mem_ptr->nanoseconds;
        sem_signal(sema_id);


        // check if new proc needs to be spawned based on random roll
        if( current_time_nano >= next_proc_nano ) {

            // get current proc count


            // if proc count < max, spawn new proc
            if(count < processes){
                count += 1;    
                fprintf(stderr, "OSS: new proc spawning at %d : %ld .\n", current_time_secs, current_time_nano);            
               
                 // select proc number fork new proc
                for (i = 0; i <= processes; i++) {
                    if ( shm_mem_ptr->procs_used[i] == 0 ) {
                        user = i;
                        break;
                    }
                }
            
                sprintf(msgerr, "Selected user_num %d to fork", user);
                write_log(msgerr);
           
                new_proc(user, term_chance);    
            
            }
        
        } 

        //next_proc_secs = current_time_secs;
        next_proc_nano = ( ( rand() % 50000 + 100 ) + current_time_nano );


        
/*
        // simulate proc termination
        if( ( rand() % 100 + 1) <= term_chance ){
           fprintf(stderr, "proc %d terminating\n", count); 
           count -= 1;       
        }

*/

        sem_wait(sema_id);
        update_clock(1000);
        sem_signal(sema_id);

        //fprintf(stderr, "proc count: %d\n", count);

    }
    
    // clean up 
    
    cleanup(SIGTERM);    

    fprintf(stderr, "Exiting. Bye.\n");

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

int kill_proc(int proc, int sig) {

    int status;
                                
    kill(proc_pids[proc], sig);

    wait(&status);

    return WEXITSTATUS(status);
}

void cleanup(int sig){

    int i;
    for(i = 0; i < max_procs; i++){

        if(shm_mem_ptr->procs_used[i] > 0){

            if(kill_proc(i, sig) != 0){
                fprintf(stderr, "OSS: Problem killing proc #%2d\n", i);

                sprintf(msgerr, "Problem killing proc %02d", i);
                write_log(msgerr);
            }
        }
    }

    int shm_ptr_ret = shmctl(shm_id, IPC_RMID, (struct shmid_ds *)NULL);
    
    if((semctl(sema_id, 0, IPC_RMID, 1) == -1) && (errno != EINTR)){
        fprintf(stderr, "OSS: shared memory and semaphores removed\n");

        sprintf(msgerr, "shared memory and semaphores removed");
        perror(msgerr);
    }
    
    return;
}

void write_log(char *msg) {

    FILE *fp;

    if(!(fp = fopen("oss.log", "a"))){
        perror("oss: failed to open oss.log");
        exit(EXIT_FAILURE);
    }

    
    fprintf(fp, "oss:\t%s\n", msg);
    fclose(fp);

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

int new_proc(int user_num, int term_chance){

    char parameter1[32];
    char parameter2[32];
    char parameter3[32];
    char parameter4[32];

    sprintf(parameter1,"-u %d", user_num);
    sprintf(parameter2,"-t %d", term_chance);


    if ((proc_pids[user_num] = fork()) < 0) {

        sprintf(msgerr, "OSS: fork FAILED for proc #%d", user_num);
        perror(msgerr);

        write_log("UNRECOVERABLE ERROR forking child. Exiting.");
        cleanup(SIGTERM);

        exit(1);

    } else {

        if (proc_pids[user_num] == 0) {
            

            char *args[] = {"./user", parameter1, parameter2, NULL};
            //char *args[] = {"./user", parameter1, parameter2, parameter3, parameter4, NULL};

            //fprintf(stderr, "OSS: about to start proc # %d \n", user_num);
            execvp(args[0], args);
            
            // print if error
            sprintf(msgerr, "oss: exec proc %d ERROR", user_num);
            perror(msgerr);
       
         } else {
            sprintf(msgerr, "Forked process ID %d for proc %d", proc_pids[user_num], user_num);
            write_log(msgerr);
            
            shm_mem_ptr->procs_used[user_num] = 1;
        }
    }

    
}





