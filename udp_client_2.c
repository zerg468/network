#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define BUF_SIZE  128

typedef struct Files{
	char name[255];
	unsigned int byte;
}Files;

int main(int argc, char *argv[]) {
    int sock; //socket
    char message[BUF_SIZE];
    char file_name[255];
    char *end_file = "endoffile";
    FILE *fp; 
    int len,per,count=0;
    int num_read=0;
    struct sockaddr_in servaddr;
    int addrlen = sizeof(servaddr);
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
	//scanf("%s",file_name);
	scanf("%s",files.name);

	//fp = fopen(file_name,"rb");
	fp = fopen(files.name,"rb");

        if(fp==NULL)
        {
		perror("file open is error!");
		exit(0);


	}

	fseek(fp,0L,SEEK_END);
	files.byte = ftell(fp);
	fseek(fp,0L,SEEK_SET);

	//len = sendto(sock,file_name,sizeof(file_name),0,(struct sockaddr *)&servaddr, addrlen);
	len = sendto(sock,(char*)&files, sizeof(files),0,(struct sockaddr*)&servaddr,addrlen);

	
	if(len<0)
	{
		perror("file sending is error");
		exit(0);

	}

	
	while(1){


		num_read = fread(message,1,BUF_SIZE,fp);
		
		//len = sendto(sock,message,BUF_SIZE,0,(struct sockaddr *)&servaddr,addrlen);
		if(num_read>0){
			len = sendto(sock,message,num_read,0,(struct sockaddr *)&servaddr,addrlen);
		
		if(len==-1)
		{
			count += num_read;
			perror("file sending is error");
			exit(0);
		}

			count+=num_read;

		}

		else if(num_read ==0 && count == files.byte)
			break;

			

	}
		

	printf("file upload is done, %d bytes %d bytes \n",files.byte,count);

	fclose(fp);
	close(sock);

	return 0;
}
