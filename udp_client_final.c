#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define BUF_SIZE  256

typedef struct Files{
	unsigned int byte;
	int check;
	char name[BUF_SIZE];

}Files;

int main(int argc, char *argv[]) {
    int sock; //socket
    FILE *fp; 
    int len,count=0;
    int num_read=0;
    struct sockaddr_in servaddr;
    int addrlen = sizeof(servaddr);
    Files files;
    int total_size=0;
	
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
	total_size = ftell(fp);
	fseek(fp,0L,SEEK_SET);
	files.byte = total_size;
	files.check = 1;

	len = sendto(sock,(char*)&files, sizeof(files),0,(struct sockaddr*)&servaddr,addrlen);

	
	if(len<0)
	{
		perror("file sending is error");
		exit(0);

	}

	
	while(1){

		files.check = 0;
	
		num_read = fread(files.name,1,BUF_SIZE,fp);
		files.byte = num_read;
		count += files.byte;

		if(num_read<0)
		{
			perror("file read is error");
			exit(0);
		}
	len = sendto(sock,(char*)&files,sizeof(files),0,(struct sockaddr *)&servaddr,addrlen);
		
		if(len<0)
		{
			
			perror("file sending is error");
			exit(0);
		}

			
		else if(files.byte ==0 && count == total_size)
			break;

	}
	
	
	if(len<0)
		perror("file sending is error");
	
	printf("file upload is done, %d bytes\n",total_size);

	fclose(fp);
	close(sock);

	return 0;
}
