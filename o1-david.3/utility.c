

/* John David
 * CS 4760 
 * Assignment 3
 *
*/


#include "utility.h"

int random_num(int n){

    time_t t;
    srand((unsigned)time(&t));
    return (rand() % n + 1);

}



