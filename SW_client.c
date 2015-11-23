#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>


#define BUF_SIZE  512

typedef struct Files{
	unsigned int byte;
	int s_number;
	int check;
	char name[BUF_SIZE];

}Files;

int main(int argc, char *argv[]) {
    int sock; //socket
    FILE *fp; 
    int len,count=0;
    int num_read=0;
    int sequence=1;
    struct sockaddr_in servaddr;
    int addrlen = sizeof(servaddr);
    Files files;
    int total_size =0; 

    files.s_number=0;
    struct timeval tv;
	
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

   tv.tv_sec = 0;
   tv.tv_usec = 400000;	
  
   setsockopt(sock, SOL_SOCKET,SO_RCVTIMEO,&tv,sizeof(tv));


        fputs("Input file_name: ", stdout);
	scanf("%s",files.name);

	fp = fopen(files.name,"rb");

        if(fp==NULL)
        {
		perror("file open is error!");
		exit(0);


	}

	fseek(fp,0L,SEEK_END);
	total_size  = ftell(fp);
	files.byte = total_size;
	fseek(fp,0L,SEEK_SET);
	
	files.check = 1; // file name

		
	do{

		len = sendto(sock,(char*)&files, sizeof(files),0,(struct sockaddr*)&servaddr,addrlen);
	
	if(len<0)
		continue;



	len = recvfrom(sock,(int *)&sequence,1,0,(struct sockaddr*)&servaddr, &addrlen);

	if(len<0){

		if(errno == EWOULDBLOCK)
			continue;

		else
		{
			printf("recvfrom error\n");
			exit(0);
		}
		
	}

	
	}while(files.s_number==sequence);
	
	files.s_number = sequence;	
	files.check = 0; //file contents
	
	while(1){


		num_read = fread(files.name,1,BUF_SIZE,fp);
		files.byte = num_read;
		count += files.byte;

		do{

			len = sendto(sock,(char*)&files, sizeof(files),0,(struct sockaddr *)&servaddr,addrlen);
		
			if(len<0)
				continue;

			
			len = recvfrom(sock,(int *)&sequence,1,0,(struct sockaddr*)&servaddr, &addrlen);

		if(len<0){

		if(errno == EWOULDBLOCK)
			continue;
	

		else{
			printf("recvfrom error\n");
			exit(0);
		}
	
		}


		}while(files.s_number==sequence);

		
		if(files.s_number==1)
			files.s_number=0;
		else
			files.s_number=1;

			
		if(files.byte ==0 && count==total_size)
			break;
		
		
	}

	
	printf("file upload is done, total_size : %d\n",total_size);

	fclose(fp);
	close(sock);

	return 0;
}
