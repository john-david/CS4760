


/*
 * john David
 * CS 4760
 * Assignment 3
 *  
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


struct shared_mem_struct{

        long int seconds;
	long int nanoseconds;

};

int smem_id;
//key_t mem_key = KEY_CODE;
void *temp_ptr = (void *)0;
struct shared_mem_struct *shared_mem;

//key_t mq_key;

#define MAX_MTEXT 500

#endif




