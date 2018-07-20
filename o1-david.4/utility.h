


/*
 * john David
 * CS 4760
 * Assignment 4 
 *   
*/

#ifndef _utility_h
#define _utility_h

/* Including Standard libraries */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <time.h>
#include <sys/mman.h>
#include <errno.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <sys/stat.h>

#include <sys/msg.h>
#include <sys/ipc.h>


int random_num(int n){};

typedef struct{

        unsigned int proc_id;
        unsigned int used_cpu_time;
        unsigned int system_total_time;
        unsigned int wait_time;
        unsigned int priority;
        unsigned int quantum;


}Process_Block;

struct shared_mem_struct{
    
    /* clock */
    long seconds;
	long nanoseconds;

    /* process control block */
    
    Process_Block process_control_block[100];

};


/* shared memory */
int smem_id;
//key_t mem_key = KEY_CODE;
void *temp_ptr = (void *)0;
struct shared_mem_struct *shared_mem;

//key_t mq_key;

#define MAX_MTEXT 500

/* process control block matrix */

int init_process_control_block(){};
int init_priority_queue(){};



#endif




