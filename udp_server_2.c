#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define BUF_SIZE    512

void error_handling(char *message);

typedef struct Files{
	char name[255];
	unsigned int byte;
}Files;

int main(int argc, char *argv[])
{
	int serv_sock;
	char message[BUF_SIZE];
	char file_name[255];
	int nbyte;
	int clnt_adr_sz;
	struct sockaddr_in serv_adr, clnt_adr;
	FILE *fp;	
	Files files;
	int count=0;

	
	if(argc!=2)
	{
		printf("Usage : %s <port> \n",argv[0]);
		exit(1);

	}
	
	serv_sock = socket(PF_INET,SOCK_DGRAM,0);
	if(serv_sock == -1)
		error_handling("UDP socket creation error");

	memset(&serv_adr,0,sizeof(serv_adr));
	serv_adr.sin_family = AF_INET;
	serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_adr.sin_port = htons(atoi(argv[1]));
	
	if(bind(serv_sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr))==-1)
		error_handling("bind() error");

	while(1)
	{
	  clnt_adr_sz = sizeof(clnt_adr);
	  
	  nbyte = recvfrom(serv_sock,(char*)&files,sizeof(files),0,(struct sockaddr*)&clnt_adr, &clnt_adr_sz);

		if(nbyte==-1)
			error_handling("error");

		fp = fopen(files.name,"wb");
		count = files.byte;

		while(count>0)
		{
			nbyte = recvfrom(serv_sock,message,BUF_SIZE,0,(struct sockaddr*)&clnt_adr, &clnt_adr_sz);

			if(nbyte<0)
				error_handling("error");
			else if(nbyte==0)
				break;
	
			//message[nbyte] = 0;

			else{
				fwrite(message,1,nbyte,fp);		

				count -= nbyte;

			}
			
	
		}
		

	printf("file download is done %d bytes\n",files.byte);
	fclose(fp);
	
	
	}
	
	close(serv_sock);

	return 0;

}

void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n',stderr);
	exit(1);
}
