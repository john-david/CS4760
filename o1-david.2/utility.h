

/* 
 * John David
 * CS 4760
 * Assignment 2
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
#include <sys/types.h>
#include <sys/mman.h>


#define BUFFERSIZE 5
#define PROD_WAIT 5
#define CONS_WAIT 5
#define RUN_TIME 100
#define CONSUMER_ID 20
#define KEY_CODE 1234


extern int turn;

extern int flag[CONSUMER_ID];

enum state {IDLE, WANT_IN, IN_CS};


struct shared_mem_struct{

	int buffer_size;
        int flag[21];
        int turn;
        char bounded_buffer[5][200];

};

int smem_id;
//key_t mem_key = KEY_CODE;
void *temp_ptr = (void *)0;
struct shared_mem_struct *shared_mem;

int random_num(int n){}



#endif




