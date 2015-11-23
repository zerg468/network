#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#define BUF_SIZE   2048

void error_handling(char *message);

typedef struct Files{
	unsigned int byte;
	int s_number;
	int check;
	char name[BUF_SIZE];
	
}Files;

int main(int argc, char *argv[])
{
	int serv_sock;
	int nbyte;
	int clnt_adr_sz;
	struct sockaddr_in serv_adr, clnt_adr;
	FILE *fp;	
	Files files;
	int count=0;
	int ACK=0;
	int one=1,zero=0;
	int total_size=0;
	int send_size = 0;

	
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

	
	while(1)
	{
			
	  nbyte = recvfrom(serv_sock,(char*)&files,sizeof(files),0,(struct sockaddr*)&clnt_adr, &clnt_adr_sz);

	if(nbyte>0)
	{
		  if(files.s_number!=ACK)
		{
			nbyte = sendto(serv_sock,(int *)&zero,1,0,(struct sockaddr*)&clnt_adr, clnt_adr_sz);
			continue;
		  }

		   else{

			nbyte = sendto(serv_sock,(int *)&one,1,0,(struct sockaddr*)&clnt_adr, clnt_adr_sz);

		if(nbyte<0)
			continue;

		   }

	}
	

	else{

		printf("recvfrom error\n");
		continue;
	}


		

		if(files.check==1)
		{ //receive file name

		total_size = files.byte;
		ACK = one;
	
		fp = fopen(files.name,"wb");
		
		}

		else
			continue;

		
		while(1)
		{

			nbyte = recvfrom(serv_sock,(char*)&files,sizeof(files),0,(struct sockaddr*)&clnt_adr, &clnt_adr_sz);
	
			if(nbyte<0){
				printf("recvfrom error\n");
				continue;
			}

		 if(ACK==one)
		{
			if(files.s_number!=ACK)
			{
			nbyte = sendto(serv_sock,(int *)&one,1,0,(struct sockaddr*)&clnt_adr, clnt_adr_sz);

					continue;
			}

		  	 else{

			nbyte = sendto(serv_sock,(int *)&zero,1,0,(struct sockaddr*)&clnt_adr, clnt_adr_sz);
			

			if(nbyte<0){
			printf("nbyte<0 \n");	
			
			continue;
			}
			
			}

		}
		
			
		else{

		 if(files.s_number!=ACK)
		{
			nbyte = sendto(serv_sock,(int *)&zero,1,0,(struct sockaddr*)&clnt_adr, clnt_adr_sz);

			continue;
		}
		   else{

			nbyte = sendto(serv_sock,(int *)&one,1,0,(struct sockaddr*)&clnt_adr, clnt_adr_sz);

		if(nbyte<0){
			printf("nbyte<0 \n");				
			continue;}

	   }

	}
		if(ACK==1)
			ACK=0;
		
		else
			ACK=1;	
		
	
		count+=files.byte;

		if(files.byte==0 && count == total_size)
			break;		
	
		
		fwrite(files.name,1,files.byte,fp);

	}
	

	printf("file download is done, total_size : %d \n",total_size);
	fclose(fp);

	memset(&files,0,sizeof(files));
	ACK=0;
	total_size=0;
	count = 0;
	send_size = 0;
	
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
