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


void son() {
	int queueid;
	key_t key = 12345;
	struct msgbuf sbuf;
    size_t buflen;


    // Socket Initialization
    struct sockaddr_in endereco_servidor;
    struct sockaddr_in endereco_cliente;
    int sck, client_socket, *new_sock;

    memset((char *)&endereco_servidor,0,sizeof(endereco_servidor));
    endereco_servidor.sin_family = AF_INET;             
    endereco_servidor.sin_addr.s_addr = inet_addr("127.0.0.1");;
    endereco_servidor.sin_port = htons(7891);

    sck = socket(AF_INET, SOCK_STREAM, 0);
    if (sck < 0) 
    {
        fprintf(stderr, "Falha ao criar socket!\n");
        exit(1); 
    }

    if (bind(sck, (struct sockaddr *)&endereco_servidor,sizeof(endereco_servidor)) < 0)
    {
        fprintf(stderr,"Ligacao Falhou!\n");
        exit(1);
    }

    if (listen(sck, 5) < 0) 
    {
        fprintf(stderr,"Falhou ouvindo porta!\n");
        exit(1); 
    }

    int alen = sizeof(struct sockaddr_in);
    
    client_socket=accept(sck, (struct sockaddr *)&endereco_cliente, (socklen_t*)&alen);

    if (client_socket < 0)
    {
        perror("accept failed");
        exit(1);
    }

    printf("ENTROU SOCKET\n");



	if ((queueid = msgget(key, 0666)) < 0)
	{
		printf ("ERRO AO CONECTAR COM FILA");
	    exit(1);
    }
    
	while(1) {
        sleep(1);

        sbuf.mtype = 1;
        int read_size;
        char bufin[MAXSIZE];
        memset(&bufin, 0x0, sizeof(bufin));      
        read_size = recv(client_socket, &bufin, sizeof(bufin), 0);
        strcpy(sbuf.mtext, bufin);
        buflen = strlen(sbuf.mtext) + 1;

        //printf("%s\n", sbuf.mtext);
        if (msgsnd(queueid, &sbuf, buflen, 0) < 0)
        {
            printf ("ERRO AO ENVIAR MENSAGEM");
            exit(1);
        }
	}
}
void father() {
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