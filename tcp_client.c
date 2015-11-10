#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 128
void error_handling(char *message);

typedef struct Files{

	char name[255];
	unsigned int byte;
}Files;

int main(int argc, char *argv[])
{
	int sock;
	char message[BUF_SIZE];
	char file_name[BUF_SIZE];
	int retval,per,count;
	FILE *fp;
	struct sockaddr_in serv_adr;
	Files files;

	if(argc!=3) {
		printf("Usage : %s <IP> <port>\n", argv[0]);
		exit(1);
	}
	
	sock=socket(PF_INET, SOCK_STREAM, 0);   
	if(sock==-1)
		error_handling("socket() error");
	
	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family=AF_INET;
	serv_adr.sin_addr.s_addr=inet_addr(argv[1]);
	serv_adr.sin_port=htons(atoi(argv[2]));
	
	if(connect(sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr))==-1)
		error_handling("connect() error!");

	else
		puts("Connected server");
	
	fputs("Input file_name: ", stdout);
	scanf("%s",files.name);

        fp = fopen(files.name,"rb");

        if(fp==NULL)
                error_handling("file open is error!");

        fseek(fp,0L,SEEK_END);
        files.byte = ftell(fp);
        fseek(fp,0L,SEEK_SET);

	retval =  send(sock,(char*)&files,sizeof(files),0);
	if(retval==-1)
		 error_handling("file name send error!");

	per = files.byte / BUF_SIZE;
	count = per;
		

	while(count) 
	{

		fread(message,1,BUF_SIZE,fp);
		retval = send(sock,message,BUF_SIZE,0);
		if(retval==-1)
			error_handling("file uploading error");
		

		count--;
	
	}
	
	count = files.byte - (per*BUF_SIZE);
	fread(message,1,count,fp);
	
	retval = send(sock,message,BUF_SIZE,0);
                if(retval==-1)
                        error_handling("file uploading error");

	printf("file upload is done\n");
	fclose(fp);
	close(sock);
	return 0;
}

void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}

