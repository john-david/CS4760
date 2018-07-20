/*
 * John David
 * CS 4760
 * Assignment 5
*/

#include "oss.h"

int proc_pid[max_procs] = { 0 };

int sh_mem_ID;
                                
sh_mem *sh_mem_ptr;
                              
int sem_clock;
                              
int sem_resources;

struct sembuf sbuf;                       
                                 
char msg_log[1000] = "";
                           
void sem_wait(int);

void sem_signal(int); 

void write_log(char*);

int kill_proc(int, int);

void interrupt_signal_alert(int);

void terminate_signal_alert(int);

void alarm_signal_alert(int);

int sigcheck();



int main(int argc, char *argv[]){

    int verbose = 0;
    int line_count = 0;

    int opt;

    while ((opt = getopt(argc, argv, "v")) != -1) {
               switch (opt) {
               case 'v':
                   verbose = 1;
                   break;
               default: /* '?' */
                   fprintf(stderr, "Usage: %s -v (if you want verbose) \n",
                           argv[0]);
                   exit(EXIT_FAILURE);
               }
    }

    if(verbose == 1)
        fprintf(stderr, "oss: verbose option set\n");


    int shared_mem_key  = ftok("README", 'a');     
    int sema_key1 = ftok("README", 'b');        
    int sema_key2  = ftok("README", 'c'); 


    /* shared memory allocation */
    sh_mem_ID = shmget(shared_mem_key, sh_mem_alloc, 0600 | IPC_CREAT);
    if (sh_mem_ID == -1) {
        perror("shmget Err");
        exit(1);
    }

    if ( ! (sh_mem_ptr = (sh_mem *)(shmat(sh_mem_ID, 0, 0)))) {
        perror("shmat Err");
        exit(1);
    }

    fprintf(stderr, "oss: shared memory has been allocated\n");

    /* set up and initialize semaphores */

    union semun { 
        int val; 
        struct semid_ds *buf; 
        unsigned short * array; 
    }argument;

    argument.val = 1;

    /* semophore to control access to clock */
    if ((sem_clock = semget(sema_key1, 1, 0600 | IPC_CREAT)) == -1) {
    }

    if (semctl(sem_clock, 0, SETVAL, argument) == -1) {
        perror("clock semctl error\n");
        exit(1);
    }

    /* semaphore to control access to resources */
    if ((sem_resources = semget(sema_key2, 1, 0600 | IPC_CREAT)) == -1) {
        perror("resource semget error\n");
        exit(1);
    }

    if (semctl(sem_resources, 0, SETVAL, argument) == -1) {
        perror("resource semctl error\n");
        exit(1);
    }

    fprintf(stderr, "oss: clock and resources semaphores have been set\n");

    /* checking  critical section entry with semaphores and set clock to 0:00 */
    sem_wait(sem_clock);  
                     
    sh_mem_ptr->seconds = 0;
    sh_mem_ptr->nanoseconds = 0;

    sem_signal(sem_clock);  


    /* initialize resources with random values and set intial values to 0 */

    int i, j, x, y, z;

    for (i = 0; i < max_resources; i++) {
        
        y = (rand() % max_instances) + 1;

        sh_mem_ptr->resources[i].total_instances = y;

        sh_mem_ptr->resources[i].instances_available = y;

        sprintf(msg_log, "oss: resource %d will have %d instances\n", i, sh_mem_ptr->resources[i].total_instances);
        line_count++;        

        fprintf(stderr, "%s", msg_log);

        write_log(msg_log);

        for (j = 0; j < max_procs; j++) {

            sh_mem_ptr->resources[i].claims[j] = 0;
            sh_mem_ptr->resources[i].requests[j] = 0;
            sh_mem_ptr->resources[i].allocations[j] = 0;
            sh_mem_ptr->resources[i].released[j] = 0;

        }

    }

    for (i = 0; i < max_procs; i++){ 
        sh_mem_ptr->procs_used[i] = 0;
    }

    sprintf(msg_log, "oss: resources have been allocated\n");

    fprintf(stderr, "%s", msg_log);

    write_log(msg_log);
    line_count++;


    /* set max resource constant for users */

    y = (rand() % max_instances) + 1;

    sh_mem_ptr->max_resource_const = y;



    /* prepare 2 second alarm */
    //struct sigaction action;
    //action.sa_handler = alarm_signal_alert;
    //action.sa_flags = SA_RESTART; //<-- restart 

    //sigaction(SIGALRM, &action, NULL);

    alarm(2);

    /* main loop */

    int count;
    int new_proc;
    char proc_arg[25] = ""; 

    int claim;
    int requests_granted = 0;

    while(1){

        /* check if any resources have been released */

        for (i = 0; i < max_resources; i++) {
            for (j = 0; j < max_procs; j++) {
                if ( sh_mem_ptr->resources[i].released[j] != 0 ) {
                    /* log resource release  */
                    sprintf(msg_log, "oss: acknowledging proc %02d released R%d\n", j, i);
                    
                    sprintf(msg_log, "%s at time: %ld : %ld : %ld \n", msg_log, sh_mem_ptr->seconds, sh_mem_ptr->milliseconds, sh_mem_ptr->nanoseconds);
                    
                    write_log(msg_log);
                    line_count++;
                    
                    /* log release in shared mem */
                    sem_wait(sem_resources);

                    sh_mem_ptr->resources[i].instances_available += sh_mem_ptr->resources[i].released[j];
                    sh_mem_ptr->resources[i].released[j] = 0;
                
                    sem_signal(sem_resources);
                }
            }
        }

        /* allocate resources  */

        for (i = 0; i < max_resources; i++) {
            for (j = 0; j < max_procs; j++) {
                if (sh_mem_ptr->resources[i].requests[j] > 0) {        
                    claim = sh_mem_ptr->resources[i].claims[j] - sh_mem_ptr->resources[i].allocations[j];
                    
                    x = sh_mem_ptr->resources[i].requests[j];
                    y = sh_mem_ptr->resources[i].allocations[j]; 
                    y = y + sh_mem_ptr->resources[i].requests[j];
                    z = sh_mem_ptr->resources[i].total_instances;
                    count = sh_mem_ptr->resources[i].instances_available;        
            
                    if ( x > claim || y > z) {

                        sprintf(msg_log, "oss: blocking P%02d for requesting R%d \n ", j, i);

                        sprintf(msg_log, "%s at time: %ld : %ld : %ld \n ", msg_log, sh_mem_ptr->seconds, sh_mem_ptr->milliseconds, sh_mem_ptr->nanoseconds);

                        write_log(msg_log);
                        line_count++;

                        sem_wait(sem_resources);
                        sh_mem_ptr->resources[i].requests[j] = -1;
                        sem_signal(sem_resources);

                    } else if (x <= count){

                        sem_wait(sem_resources);
                        sh_mem_ptr->procs_counter[j] += sh_mem_ptr->resources[i].requests[j];
                        
                        sprintf(msg_log, "oss: unblocking P%02d and granting it R%d \n", j, i);
                        sprintf(msg_log, "%s at time: %ld : %ld : %ld \n ", msg_log, sh_mem_ptr->seconds, sh_mem_ptr->milliseconds, sh_mem_ptr->nanoseconds);

                        requests_granted++;

                        write_log(msg_log);
                        line_count++;                        

                        sh_mem_ptr->resources[i].instances_available -= sh_mem_ptr->resources[i].requests[j];
                        sh_mem_ptr->resources[i].allocations[j] += sh_mem_ptr->resources[i].requests[j];                        
                        
                        sh_mem_ptr->resources[i].requests[j] = 0;

                        sem_signal(sem_resources);

                    }   
                }
            }
        }

        /* fork users if below max_procs */              
        count = 0;

        for (i = 0; i < max_procs; i++) {

            
            if ( sh_mem_ptr->procs_used[i] == 0 && proc_pid[i] > 0 ) {
                sprintf(msg_log, "oss: proc %02d has ended. \n", i);
                write_log(msg_log);
                line_count++;

                if ( kill_proc(i, SIGTERM) != 0 ) {
                    sprintf(msg_log, "oss: error killing proc %02d \n", i);
                    write_log(msg_log);
                    line_count++;
                }
            proc_pid[i] = 0;
            }

            if ( sh_mem_ptr->procs_used[i] ) 
                count++;
        }
        

        /*
        if(verbose == 0 && line_count <= 100000 ){
            sprintf(msg_log, "oss: current proc count is %d \n", count);
            write_log(msg_log);
            line_count++;
        }
        */

        /* find a slot for the new process in shared memory */
        if ( count < max_procs ) {
            for (i = 0; i <= max_procs; i++) {
                if ( sh_mem_ptr->procs_used[i] == 0 ) {
                    new_proc = i;
                    break;
                }
            }

            if(verbose == 1 && line_count <= 100000){
                sprintf(msg_log, "oss: forking %d \n", new_proc);
                write_log(msg_log);
                line_count++;
            }
                
            /* fork a new process */

            if ((proc_pid[new_proc] = fork()) < 0) {
                perror("oss: error forking new proc \n");
    
            } else {

                if (proc_pid[new_proc] == 0) {
                    sprintf(proc_arg, "%02d", new_proc);
                    execl("./user", "user", proc_arg, (char *)NULL);

                    if(verbose == 1 && line_count <= 100000){

                        sprintf(msg_log, "oss: exec proc %02d after fork \n", new_proc);
                        write_log(msg_log);
                    }

                }else {

                    sh_mem_ptr->procs_used[new_proc] = 1;
                    if(verbose == 1 && line_count <= 100000)
                        sprintf(msg_log, "oss: forked pid: %d for proc: %02d \n", proc_pid[new_proc], new_proc);
                        write_log(msg_log);
                }
            }

        }

        /* update clock 1 to 500 millseconds */
       
        sem_wait(sem_clock);   
          
        sh_mem_ptr->milliseconds += rand() % 500 + 1;
        
        if(sh_mem_ptr->milliseconds >= 1000){
            sh_mem_ptr->seconds += 1;
            sh_mem_ptr->milliseconds -= 1000;
        }
        
        sh_mem_ptr->nanoseconds += rand() % 1000 + 1;

        if(sh_mem_ptr->nanoseconds >= 1000) {
            sh_mem_ptr->milliseconds += 1;
            sh_mem_ptr->nanoseconds -= 1000;
        }

        sem_signal(sem_clock); 

        sprintf(msg_log, "oss: current time:  ");
        sprintf(msg_log, "%s  %ld : %ld : %ld \n ", msg_log, sh_mem_ptr->seconds, sh_mem_ptr->milliseconds, sh_mem_ptr->nanoseconds);

        write_log(msg_log);
        line_count++;

 
        /* check if 60 simulated seconds have passed */

        if(sh_mem_ptr->seconds >= 250) {
            
            sprintf(msg_log, "oss: 60 seconds have elapsed. Exiting\n");

            write_log(msg_log);
            line_count++;

            break;
        }

        /* if line_count % 20 == 0, print resource table */

    

        /* check for signals before looping again */
        
        if(signal_handler())
            break;

    }/* end while loop */

 
    /* clean up shared memory and semaphore allocation */

    for (i = 0; i < max_procs; i++) {
        if ( sh_mem_ptr->procs_used[i] > 0 ) {
            if ( kill_proc(i, SIGTERM) != 0 ) {
                fprintf(stderr, "oss: error terminating user proc: %02d \n", i);
            }
        }
    }

    int sh_mem_ptr_clean = shmctl(sh_mem_ID, IPC_RMID, (struct shmid_ds *)NULL);

    if ((semctl(sem_clock, 0, IPC_RMID, 1) == -1) && (errno != EINTR)) {
        fprintf(stderr, "oss: clock removed\n");
    }

    if ((semctl(sem_resources, 0, IPC_RMID, 1) == -1) && (errno != EINTR)) {
        fprintf(stderr, "oss: resources removed\n");
    }



    return 0;
}

void interrupt_signal_alert(int sig) {
    fprintf(stderr, "oss: interrupt signal (SIGINT) received, exiting process\n");
}

void terminate_signal_alert(int sig) {
    //fprintf(stderr, "oss: 2 real life seconds have elapsed.\n");
    fprintf(stderr, "oss: termination signal (SIGTERM) received, exiting process\n");
}

void alarm_signal_alert(int sig){
    fprintf(stderr, "oss: 2 seconds gave elapsed.\n");
    fprintf(stderr, "oss: alarm signal (SIGALRM) received, exiting process.\n");
}


int signal_handler(){

    signal(SIGINT, interrupt_signal_alert);
    signal(SIGTERM, terminate_signal_alert);
    signal(SIGALRM, alarm_signal_alert);

    return 0;
}

void sem_wait(int sem_id) {
    
    sbuf.sem_num = 0;                           
    sbuf.sem_op = -1;                           
    sbuf.sem_flg = 0;                           

    if (semop(sem_id, &sbuf, 1) == -1)  {
            exit(0);
    }
}

void sem_signal(int sem_id) {
                     
    sbuf.sem_num = 0;                           
    sbuf.sem_op = 1;                        
    sbuf.sem_flg = 0;                       

    if (semop(sem_id, &sbuf, 1) == -1)  {
            exit(1);
    }
}

void write_log(char *msg) {

    FILE *fp;
    if (!(fp = fopen("oss.log", "a"))) {
        perror("oss error: can't open oss.log");
        exit(1);
    }
    
    fprintf(fp, "oss:\t%s\n", msg);
    fclose(fp);
}

int kill_proc(int proc, int sig) {

    int status;         
    
    kill(proc_pid[proc], sig);
    wait(&status);

    return WEXITSTATUS(status);
}

