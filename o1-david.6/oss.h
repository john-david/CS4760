/*
 * John David
 * CS 4760
 * Assignment 6
*/


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
#include <sys/sem.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <limits.h>
#include <fcntl.h>


#define max_resources 20

#define max_procs 18

#define max_frame 1024

#define shm_mem_size sizeof(shm_mem) 

typedef struct{

    long mType;
    char mText[80];
     
}Message;

typedef struct{

    int page_id;
    int dirty_bit;
    int second_chance;
    int allocated[max_frame];

}Page;

typedef struct{

    Page page;
    int status;
    int ref_bit;

}Frame;

typedef struct{

    Frame frames[256];
    int frame_id;
    int status;    
    int delimiter;

}PageTable;


typedef struct{

    unsigned long int nanoseconds;
    unsigned long int seconds;
    
    Message message;

    int total_mem;

    PageTable table;

    int procs_used[max_procs]; 

}shm_mem;



