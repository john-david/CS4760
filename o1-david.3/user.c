

/*
 * John David
 * CS 4760
 * Assignment 3
*/

#include "utility.h"

static void ALARMhandler(int);

int this_user;

struct mbuf {

    long mtype;
    char mtext[MAX_MTEXT];
    int muser;
};

int main(int argc, char *argv[])
{

	signal(SIGALRM, ALARMhandler);
	int user_num = atoi(argv[1]);

	this_user = user_num;

	/* shared memory */

	int smem_id = atoi(argv[2]);
	key_t smem_key = atoi(argv[3]);
    


        void *temp_ptr = (void *)0;
        struct shared_mem_struct *shared_mem;

        if((smem_id = shmget(smem_key, sizeof(struct shared_mem_struct), IPC_CREAT | 0777))<0){
                perror("shmget");
                exit(EXIT_FAILURE);
        }

        if((temp_ptr = shmat(smem_id, (void *)0, 0)) == (void *)-1)
        {

                perror("shmat");
                exit(EXIT_FAILURE);
        }
        else if (shmctl(smem_id , IPC_RMID , 0) == -1)
        {
                perror("smem_id");
        }

        shared_mem = (struct shared_mem_struct *) temp_ptr;

        /* Set up Message Quueu to send messages to OSS  */

        int msqid, msgLen;
        struct mbuf msg;
        msqid = atoi(argv[4]);

        fprintf(stderr, "User %d rec'd msqid # %d\n", user_num, msqid);

        /* Set up message queue to send messages to other users */

	int pid = getpid();

	fprintf(stderr, "User started. PID: %d \n", pid);

    char message[500];
    int result;

	while(1)
	{
		fprintf(stderr, "User %d PID: %d sleeping 1 second \n", user_num, pid);
		sleep(1);

        sprintf(message, "");


        /* send message to OSS */
        sprintf(message, "%d XX:XX/XX:XX", user_num);
        
        msgLen = strlen(message);

        msg.mtype = 1;

        msg.muser = user_num;

        memcpy(msg.mtext, message, msgLen);
        
	    result = msgsnd(msqid, &msg, msgLen, 0);
        if (result == -1)
            {
                perror("msgsnd");
                exit(EXIT_FAILURE);
            }


        fprintf(stderr, "User %d: message sent \n", user_num);

	}


	fprintf(stderr, "User %d: cleaning up shared memory\n", this_user);

        /* Remove the deatached shared memory */
        shmdt(shared_mem);
        shmctl(smem_id, IPC_RMID, NULL);

	fprintf(stderr, "User ended.\n");	
	
return 0;
}

static void  INThandler(int sig)
{

        fprintf(stderr, "User %d: cleaning up shared memory\n", this_user);

        /* Remove the deatached shared memory */
        shmdt(shared_mem);
        shmctl(smem_id, IPC_RMID, NULL);

        fprintf(stderr, "User %d: exiting", this_user);
        exit(EXIT_SUCCESS);
}



static void ALARMhandler(int sig)
{

        fprintf(stderr, "USER %d: cleaning up shared memory\n", this_user);

        /* Remove the deatached shared memory */
        shmdt(shared_mem);
        shmctl(smem_id, IPC_RMID, NULL);

        fprintf(stderr, "User %d: exiting", this_user);
        exit(EXIT_SUCCESS);
}
