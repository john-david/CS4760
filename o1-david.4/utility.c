

/* John David
 * CS 4760 
 * Assignment 4
*/


#include "utility.h"

int random_num(int n){

    time_t t;
    srand((unsigned)time(&t));
    return (rand() % n + 1);

}


static Process_Block** queue;

static Process_Block* process_block_controller;

static Process_Block openblock = {-1, -1, -1, -1, -1};

int init_priority_queue()
{
    queue = (Process_Block**)malloc(100*sizeof(Process_Block*));
    if (queue == NULL)
        return -1;
    int i, j;
    for (i = 0; i < 100; i++) {
        queue[i] = (Process_BLock*)malloc(100*sizeof(Process_BLock));
        if (queue[i] == NULL)
            return -1;
        for (j = 0; j < 100; j++) {
            queue[i][j] = openblock;
        }
    }
    return 0;
}


int init_process_control_block()
{
    initpriorityqueue();
    process_block_controller = (Process_Block*)malloc(sizeof(Process_BLock));
    if (process_block_controller == NULL)
        return -1;
    int i;
    for (i = 0; i < 100; i++) {
        process_block_controller[i] = openblock;
    }
    return 0;
}

process_block_controller** get_priority_queue()
{
    if (queue != NULL)
        return queue;
    else
        return NULL;
}

int get_queue_count()
{
    int i = 0;
    int  e = 0; 
    int count = 0;
    
    process_block_controller** queue = get_priority_queue();
    for (i = 0; i < 5; i++) {
        for (e = 0; e < 18; e++) {
            if (queue[i][e].proc_id != -1) {
                count++;
            }
        }
    }
    return count;
}

int decide_queue_schedule(process_block_controller* process)
{
    int p;

    p = (int) rand() % 5 + 1;

    int q = 10000000;

    if (p == 1)
        q = q / 2;
    else
        fprintf(stderr, "error");

    process->priority = p;
    process->quantum = q;

    return p;
}



