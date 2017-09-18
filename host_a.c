#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#define MAXSIZE 144

struct msgbuf
{
    long    mtype;
    char    mtext[MAXSIZE];
};
void create_shared_memory(){
    int shmid;
    key_t key;
    char *shm;
    char *s;
    
    key = 1234;

    shmid = shmget(key, SH_SIZE, IPC_CREAT | 0666);
    
    if(shmid < 0)
    {
        perror("shmget");
        exit(1);
    }

    shm = shmat(shmid, NULL, 0);

    if(shm == (char *) -1)
    {
        perror("shmat");
        exit(1);
    }
}

void sendmsg() {
	int queueid;
	key_t key = 1234;
	struct msgbuf sbuf;
    size_t buflen;

    if((queueid = msgget(key, (IPC_CREAT | 0666))) < 0) {
    	printf("ERRO AO CRIAR FILA\n");
    	exit(1);
	}
	while(1) {
		sbuf.mtype = 1;
	    scanf("%s", sbuf.mtext);
	    buflen = strlen(sbuf.mtext) + 1;

	    if (msgsnd(queueid, &sbuf, buflen, 0) < 0)
	    {
	        printf ("ERRO AO ENVIAR MENSAGEM");
	        exit(1);
	    }	
	}
}
void receivemsg() {
	int queueid;
	key_t key = 1234;
    struct msgbuf rbuf;
    struct msqid_ds buf;

	if ((queueid = msgget(key, 0666)) < 0)
	{
		printf ("ERRO AO CONECTAR COM FILA");
	    exit(1);
	}

  	while(1) 
  	{
        sleep(3);
        if(msgctl(queueid, IPC_STAT, &buf) != 0)
        {
            perror("msgctl");
            exit(1);
        }
        while((int *)buf.msg_qnum > 0)
        {
            if (msgrcv(queueid, &rbuf, MAXSIZE, 1, 0) < 0) 
            {
                printf ("ERRO AO RECEBER MENSAGEM");
                exit(1);
            }
            if(msgctl(queueid, IPC_STAT, &buf) != 0)
            {
                perror("msgctl");
                exit(1);
            }


        }
    }
}
int main() {
	
    pid_t pid = fork();
    if (pid > 0)
    {
    	sendmsg();
    }
    else if (pid == 0)
    {
	 	receivemsg();   
    }
    else
    {
        printf("FALHA AO CRIAR FILHO\n");
        exit(1);
    }
   
	return 0;
}

