/*
 * John David
 * CS 4760
 * Assignment 5
*/

#include "oss.h"


struct sembuf sbuf;

int user_num;

void interrupt_signal_alert(int);

void terminate_signal_alert(int);

int signal_handler();

void sem_wait(int);

void sem_signal(int);

int main(int argc, char *argv[]){

    user_num = atoi(argv[1]);

    int sh_mem_ID;

    sh_mem *sh_mem_ptr;

    int sem_clock;

    int sem_resources;

    char msg_log[1000] = "";

    int shared_mem_key  = ftok("README", 'a');
    int sema_key1 = ftok("README", 'b');
    int sema_key2  = ftok("README", 'c');

    /* shared memory allocation */
    sh_mem_ID = shmget(shared_mem_key, sh_mem_alloc, 0600 | IPC_CREAT);
    if (sh_mem_ID == -1) {
        perror("user: shmget Err");
        exit(1);
    }

    if ( ! (sh_mem_ptr = (sh_mem *)(shmat(sh_mem_ID, 0, 0)))) {
        perror("user: shmat Err");
        exit(1);
    }

    fprintf(stderr, "user %d: shared memory has been allocated\n", user_num);

    /* set up and initialize semaphores */

    /* semophore to control access to clock */
    if ((sem_clock = semget(sema_key1, 1, 0600 | IPC_CREAT)) == -1) {
        perror("user: clock semget error\n");
        exit(1);
    }

    /* semaphore to control access to resources */
    if ((sem_resources = semget(sema_key2, 1, 0600 | IPC_CREAT)) == -1) {
        perror("user: resource semget error\n");
        exit(1);
    }

    fprintf(stderr, "user %d: attached to clock and resources semaphores\n", user_num);

    /* preset tracking values */

    sh_mem_ptr->cpu_time[user_num] = 0;

    sh_mem_ptr->wait_time[user_num] = 0;

    sh_mem_ptr->procs_counter[user_num] = 0;


    /* determine user resource needs */

    srand(time(NULL));  
    
    int i;
    int max_resource_const = sh_mem_ptr->max_resource_const;
    int resources_needed;                          
       
    for(i = 0; i < max_resources; i++) {
        resources_needed = rand() % max_resource_const;  

        if(resources_needed == max_resource_const ) {
            
            sh_mem_ptr->resources[i].claims[user_num] = 1 + (rand() % sh_mem_ptr->resources[i].total_instances);
            
            fprintf(stderr, "user %d claimed max resources %d\n", user_num,  sh_mem_ptr->resources[i].claims[user_num]);
        
        }
        else{

            sh_mem_ptr->resources[i].claims[user_num] = 1 + (rand() % resources_needed);

            //sh_mem_ptr->resources[i].claims[user_num] = 1 + (rand() % sh_mem_ptr->resources[i].total_instances);
            
            fprintf(stderr, "user %d claimed resources %d\n", user_num,  sh_mem_ptr->resources[i].claims[user_num]);
        }
    }
    
    

    /* main user loop */    

    int term_const = 10; //setting a 1% termination constant 
    int wait_time; 
    int release; 
    int request;

    int release_const;
    int request_const;

    int starting_time = sh_mem_ptr->seconds;    

    while(1){

        /* check for chance of termination against termination constant */
        if ( (rand() % 1000) <= term_const && sh_mem_ptr->seconds - starting_time > 0 ) {
            /* close out resource requests and release them */
            for (i = 0; i < max_resources; i++) {

                sem_wait(sem_resources); 

                request = 0;

                sh_mem_ptr->resources[i].requests[user_num] = request;

                release = sh_mem_ptr->resources[i].allocations[user_num];

                sh_mem_ptr->resources[i].released[user_num] = release;

                sem_signal(sem_resources);

            }

            sh_mem_ptr->procs_used[user_num] = 0;

            exit(0);

        }


        /* request and release resources */
        
        /* every loop will have a different request and release ratio */
        request_const = rand() % 3 + 1;
        release_const = rand() % 3 + 1;

        for(i = 0; i < max_resources; i++){
            
            if ( sh_mem_ptr->resources[i].allocations[user_num] > 0){ 

                    //if ( release_const == 1 ) {

                if ( rand() % release_const == 1 ) {
                        
                        release = rand() % sh_mem_ptr->resources[i].allocations[user_num];

                        sem_wait(sem_resources);        
                        
                        sh_mem_ptr->resources[i].released[user_num] += release;
                     
                        fprintf(stderr, "user %d: has released %d instances of resource %d\n", user_num, release, i);

                        sh_mem_ptr->cpu_time[user_num] += 50;

                        sem_signal(sem_resources);
 
                    
                }

            } else if ( sh_mem_ptr->resources[i].requests[user_num] == 0 ) {
                
                    if( rand() % request_const == 1 ) {
                        
                        request = rand() % ( sh_mem_ptr->resources[i].claims[user_num] - sh_mem_ptr->resources[i].allocations[user_num] );

                        if ( request > 0 ) {      

                            sem_wait(sem_resources);
     
                            sh_mem_ptr->resources[i].requests[user_num] = request;

                            sem_signal(sem_resources);  

                            fprintf(stderr, "user %d: has requested %d instances of resource %d \n", user_num, request, i);

                            sh_mem_ptr->cpu_time[user_num] += 10000; 
                        }
                
                    }
            }else{

                fprintf(stderr, "user %d: has not requested any instances of resources this cycle.\n", user_num);
            }
        }


        /* increment the clock */

        wait_time = 1 + ( rand() % 100 );       

        sem_wait(sem_clock);       
        
        sh_mem_ptr->milliseconds += wait_time;

        if(sh_mem_ptr->milliseconds >= 1000){
            
            sh_mem_ptr->seconds++;
            
            sh_mem_ptr->milliseconds -= 1000;
        }

        sh_mem_ptr->wait_time[user_num] += wait_time;

        sem_signal(sem_clock);              

        
        /* check for signals before looping again */

        if(signal_handler()){
            fprintf(stderr, "user %d: signal receieved. exiting.\n", user_num);
            break;
        }

    }

    return 0;
}


void interrupt_signal_alert(int sig) {
    fprintf(stderr, "user %d: interrupt signal (SIGINT) received, exiting process\n", user_num);
}

void terminate_signal_alert(int sig) {
    //fprintf(stderr, "oss: 2 real life seconds have elapsed.\n");
    fprintf(stderr, "user %d: termination signal (SIGTERM) received, exiting process\n", user_num);
}

int signal_handler() {

    signal(SIGINT, interrupt_signal_alert);
    signal(SIGTERM, terminate_signal_alert);
    //signal(SIGALRM, alarm_signal_alert);

    return 0;
}

void sem_wait(int sem_id) {
    /* struct sembuf sbuf; */
         
    sbuf.sem_num = 0;                           
    sbuf.sem_op = -1;                           
    sbuf.sem_flg = 0;                           

    if (semop(sem_id, &sbuf, 1) == -1)  {
            exit(0);
    }

    /* return; */
}

void sem_signal(int sem_id) {
    /* struct sembuf sbuf; */
                     
    sbuf.sem_num = 0;                           
    sbuf.sem_op = 1;                        
    sbuf.sem_flg = 0;                       

    if (semop(sem_id, &sbuf, 1) == -1)  {
            exit(1);
    }

    /* return; */
}




