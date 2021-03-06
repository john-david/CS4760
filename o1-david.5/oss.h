/*
 * John David
 * CS 4760
 * Assignment 5
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

#define max_resources 20

#define max_instances 10

#define max_procs 18

#define sh_mem_alloc sizeof(sh_mem)



typedef struct{

    int shared_indicator;

    int total_instances;
    int instances_available;
    
    int claims[max_procs];
    int requests[max_procs];
    int allocations[max_procs];
    int released[max_procs];

}resource;


typedef struct{

    unsigned long int nanoseconds;
    unsigned long int milliseconds;
    unsigned long int seconds;


    int max_resource_const;
    int procs_used[max_procs];
    int procs_counter[max_procs];
    int wait_time[max_procs];
    unsigned long cpu_time[max_procs];

    resource resources[max_procs];    

}sh_mem;
