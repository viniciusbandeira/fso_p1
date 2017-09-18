#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/shm.h>

#define MAXSIZE 144

struct msgbuf
{
    long    mtype;
    char    mtext[MAXSIZE];
};


void sendmsg() {
	int queueid;
	key_t key = 12345;
	struct msgbuf sbuf;
    size_t buflen;

    key_t key_shm = 9876;
    int shmid;
    char *shm;
    char *s;
    
    shmid = shmget(key_shm, MAXSIZE, 0666);
    
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

	if ((queueid = msgget(key, 0666)) < 0)
	{
		printf ("ERRO AO CONECTAR COM FILA");
	    exit(1);
    }
    
	while(1) {
        sleep(1);
        if(*shm != '*')
        {
            sbuf.mtype = 1;
            strcpy(sbuf.mtext, shm);
            buflen = strlen(sbuf.mtext) + 1;
    
            if (msgsnd(queueid, &sbuf, buflen, 0) < 0)
            {
                printf ("ERRO AO ENVIAR MENSAGEM");
                exit(1);
            }
            *shm = '*';
        }
	}
}
void receivemsg() {
	int queueid;
    key_t key = 12345;
    struct msgbuf rbuf;
    struct msqid_ds buf;
    
    if((queueid = msgget(key, (IPC_CREAT | 0666))) < 0) {
    	printf("ERRO AO CRIAR FILA\n");
    	exit(1);
    }
    
  	while(1) 
  	{
        sleep(0);
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
            printf("%s\n", rbuf.mtext);
        }
    }
}
int main() {
	
    pid_t pid = fork();
    if (pid > 0)
    {
        receivemsg();   
    }
    else if (pid == 0)
    {
        sendmsg();
    }
    else
    {
        printf("FALHA AO CRIAR FILHO\n");
        exit(1);
    }
   
	return 0;
}