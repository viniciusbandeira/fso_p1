#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/shm.h>
#include <math.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

#define MAXSIZE 144

struct msgbuf
{
    long    mtype;
    char    mtext[MAXSIZE];
};


void father() {
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
void son() {
	int queueid;
    key_t key = 1234;
    struct msgbuf rbuf;
    struct msqid_ds buf;


    // Socket Initialization
    int clientSocket;
    char buffer[MAXSIZE];
    struct sockaddr_in serverAddr;
    socklen_t addr_size;

    /*---- Create the socket. The three arguments are: ----*/
    /* 1) Internet domain 2) Stream socket 3) Default protocol (TCP in this case) */
    clientSocket = socket(PF_INET, SOCK_STREAM, 0);

    /*---- Configure settings of the server address struct ----*/
    /* Address family = Internet */
    serverAddr.sin_family = AF_INET;
     // Set port number, using htons function to use proper byte order 
    serverAddr.sin_port = htons(7891);
    /* Set IP address to localhost */
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    /* Set all bits of the padding field to 0 */
    memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);  

    /*---- Connect the socket to the server using the address struct ----*/
    addr_size = sizeof serverAddr;
    connect(clientSocket, (struct sockaddr *) &serverAddr, addr_size);



	if ((queueid = msgget(key, 0666)) < 0)
	{
		printf ("ERRO AO CONECTAR COM FILA");
	    exit(1);
	}

  	while(1) 
  	{
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
            if (send(clientSocket, rbuf.mtext, strlen(rbuf.mtext), 0) == -1)
                printf("send");
        }
    }
}
int main() {
	
    pid_t pid = fork();
    if (pid > 0)
    {
    	father();
    }
    else if (pid == 0)
    {
	 	son();   
    }
    else
    {
        printf("FALHA AO CRIAR FILHO\n");
        exit(1);
    }
   
	return 0;
}

