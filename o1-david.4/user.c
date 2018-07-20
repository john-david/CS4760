

/*
 * John David
 * CS 4760
 * Assignment 4
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

    /* user type  */
    
    int get_user_type = 0;

    int quantum;

    quantum = shared_mem->process_control_block[user_num].quantum;

    get_user_type = random_num(100);
   
    int user_type = 0; 

    if(get_user_type > 50)
        user_type = 1;
    else
        user_type = 2;

    /* setting this user to type 1 (real-time) for testing */
    char user_class[10];
    user_type = 1;    

    switch(user_type){

        case 1: 
            strcat(user_class, "real_time");
            fprintf(stderr, "User %d: user_class: %s\n", user_num, user_class);
            break;

        case 2:
            strcat(user_class, "norm_proc");  
            fprintf(stderr, "User %d: user_class: %s\n", user_num, user_class);  
            break;

    }

	fprintf(stderr, "User %d started. class: %s PID: %d \n", user_num, user_class, pid);

    char message[500];
    int result;

	while(1)
	{
		//fprintf(stderr, "User %d class: %s PID: %d sending message back to OSS. \n", user_num, user_class, pid);

        sprintf(message, "");


        /* send message to OSS */
        sprintf(message, "%d XX:XX/XX:XX", user_num);
        
        msgLen = strlen(message);

        msg.mtype = user_type;

        msg.muser = user_num;

        memcpy(msg.mtext, message, msgLen);
        
	    result = msgsnd(msqid, &msg, msgLen, 0);
        if (result == -1)
            {
                perror("msgsnd");
                exit(EXIT_FAILURE);
            }

        

        //fprintf(stderr, "User %d: message sent \n", user_num);

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
