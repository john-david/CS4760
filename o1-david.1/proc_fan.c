

/*
 *
 * John David
 * CS 4760 
 * Assignment 1
 *
*/

/* proc_fan.c */



#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>
#include <errno.h>

int main(int argc, char *argv[]){
        int flags, opt;
        int processes;
        int argcount;

        extern char *optarg;
        extern int optind;

        int fflag;
        fflag = 0;
        char *file_name;

        processes = 0;
        flags = 0;

        printf("Hello, we have %d arguments\n", argc);

        if(argc > 5)
                perror("proc_fan: Error: Improper usage, more than 4 arguments");


        while ((opt = getopt(argc, argv, "n:f:")) != -1) {

                switch (opt) {

                        case 'n':
                        processes = atoi(optarg);
                        flags = 1;
                        break;

                        case 'f':
                        fflag = 1;
                        file_name = optarg;
                        fprintf(stderr, "file included: %s \n", file_name);
                        break;

                        default: /* '?' */

                        fprintf(stderr, "proc_fan: Error: Usage: %s [-n processes] [optional: -f file-name]\n",argv[0]);
                        exit(EXIT_FAILURE);
               }
           }

        printf("flags=%d; fflag=%d; processes=%d; optind=%d\n", flags, fflag, processes, optind);

        int i = processes;
        char parameter[32];
        sprintf(parameter, "%d", i);


        /* if argc = 3: ./proc_fan -n (int processes) */
        /* in this case, we utilize the file "testing.data" automatically */

        if(flags == 1 && fflag == 0){

                fprintf(stderr, "executing runsim with %d processes using the default testing.data file", processes);

                char command[150];

                strcpy(command, "./runsim ");
                strcat(command, parameter);
                strcat(command, " < ");
                strcat(command, "testing.data");

                system(command);


        }

        /* if argc = 4: ./proc_fan -n (int processes) (file_name) */
        /* in this case, we utilize the user provided file */
        
        if(flags == 1 && fflag == 1){


                fprintf(stderr, "executing runsim with %d processes using \'%s\' file\n", processes, file_name);

                char command[150];

                strcpy(command, "./runsim ");
                strcat(command, parameter);
                strcat(command, " < ");
                strcat(command, file_name);

                system(command);

        }

        exit(EXIT_SUCCESS);
}
