#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>

#define BUF_SIZE 2048

void error_handling(char *message);

typedef struct Files{
	unsigned int byte;
	int check;
	char name[BUF_SIZE];

}Files;

int main(int argc, char *argv[])
{
	int serv_sock;
	Files files;
	int nbyte;
	int clnt_adr_sz;
	struct sockaddr_in serv_adr, clnt_adr;
	FILE *fp;	
	int count=0;
	int error_check=0;
	int total_size=0;
	struct timeval tv;

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

	clnt_adr_sz = sizeof(clnt_adr);
	
	tv.tv_sec = 5;
	tv.tv_usec = 0;
	
	setsockopt(serv_sock, SOL_SOCKET,SO_RCVTIMEO,&tv,sizeof(tv));

	while(1)
	{
	 	
 
	  nbyte = recvfrom(serv_sock,(char*)&files,sizeof(files),0,(struct sockaddr*)&clnt_adr, &clnt_adr_sz);
		
		if(nbyte<0){

			if(errno == EWOULDBLOCK)
			{
				printf("server didn't receive file name\n");
				continue;
			}
			
			else
				error_handling("error");
		
		}

		else{

		if(files.check==0)
			continue;
		
		fp = fopen(files.name,"wb");
		total_size = files.byte;

	
		}


		while(1)
		{

			nbyte = recvfrom(serv_sock,(char*)&files,sizeof(files),0,(struct sockaddr*)&clnt_adr, &clnt_adr_sz);

					
			if(nbyte<0){

			if(errno == EWOULDBLOCK)
			{
				error_check = 1;
				break;
			}
				else
				error_handling("error");
		

			}
			

			else{
			
			count += files.byte;
									
			if(files.byte==0 && count == total_size)
				break;		

			fwrite(files.name,1,files.byte,fp);		
				
			}
			
			
	
		}
	
		
	if(error_check==0)
		printf("file download is done\n");

	else
		printf("file download is error\n");
	
	fclose(fp);
	
	memset(&files,0,sizeof(files));
	count = 0;
	total_size = 0;
	error_check=0;

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
