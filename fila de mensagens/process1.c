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

	if ((queueid = msgget(key, 0666)) < 0)
	{
		printf ("ERRO AO CONECTAR COM FILA");
	    exit(1);
	}

  	while(1) 
  	{
  		if (msgrcv(queueid, &rbuf, MAXSIZE, 1, 0) < 0) 
  		{
     		printf ("ERRO AO RECEBER MENSAGEM");
	        exit(1);
  		}

    	printf("%s\n", rbuf.mtext);
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

