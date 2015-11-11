#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define BUF_SIZE  256

struct sockaddr_in servaddr;
int addrlen = sizeof(servaddr);

typedef struct Files{

	char name[255];
	unsigned int byte;

}Files;

void sendMessage(int s, char* buf) {
    if((sendto(s, buf, strlen(buf), 0, (struct sockaddr *)&servaddr, addrlen)) < 0) {
        perror("sendto fail");
        exit(0);
    }
}

int main(int argc, char *argv[]) {
    int sock; //socket
    char message[BUF_SIZE];
    char file_name[255];
    FILE *fp; 
    int len,per,count;
    Files files;

    if(argc!=3){
	printf("Usage : %s <IP> <PORT> \n",argv[0]);
	exit(1);
	}

	sock = socket(PF_INET,SOCK_DGRAM,0);
	if(sock==-1)
	{
		perror("socket fail");
		exit(0);
	}

    memset(&servaddr, 0, addrlen); 
    servaddr.sin_family = AF_INET; 
    servaddr.sin_addr.s_addr = inet_addr(argv[1]); 
    servaddr.sin_port = htons(atoi(argv[2])); 

        fputs("Input file_name: ", stdout);
	scanf("%s",files.name);

        fp = fopen(files.name,"rb");

        if(fp==NULL)
        {
		perror("file open is error!");
		exit(0);


	}

	fseek(fp,0L,SEEK_END);
	files.byte = ftell(fp);
	fseek(fp,0L,SEEK_SET);
	

	len = sendto(sock,(char*)&files,sizeof(files),0,(struct sockaddr *)&servaddr, addrlen);
	
	if(len<0)
	{
		perror("error");
		exit(0);

		}

	//sendMessage(sock,file_name);
	
	per = files.byte / BUF_SIZE;
	count = per;
	
	while(count){

		fread(message,1,BUF_SIZE,fp);
		//sendMessage(sock,message);
		
		len = sendto(sock,message,BUF_SIZE,0,(struct sockaddr *)&servaddr,addrlen);
		if(len<0)
		{
			perror("error");
			exit(0);
			}
		
		count--;
	}
		
	count = files.byte - (per*BUF_SIZE);
	fread(message,1, count,fp);

	len = sendto(sock,message,BUF_SIZE,0,(struct sockaddr*)&servaddr,addrlen);

	if(len==-1)
	{
		perror("error");
		exit(0);
	}

	printf("file upload is done");

	fclose(fp);

	//message[0] = '\0';
	//sendMessage(sock,"end of file");
	close(sock);

	return 0;
}
