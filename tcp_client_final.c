#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

#define BUFSIZE 1024
#define NAMESIZE 256

void error_handling(char* err_msg);

int main(int argc, char **argv)
{
	int sockfd;
	char message[BUFSIZE + 1];
	char filename[NAMESIZE];
	int str_len, str_total = 0, total_bytes = 0;
	struct sockaddr_in server_addr;
	FILE *fp = NULL;

	//ARGUMENT CHECK
	if (argc < 3)
	{
		fprintf(stderr, "Usage: %s [IP] [port]\n", argv[0]);
		exit(1);
	}

	//SOCKET
	sockfd = socket(PF_INET, SOCK_STREAM, 0);			
	if (sockfd < 0)
		error_handling("Socket error");

	memset(&server_addr, 0, sizeof(server_addr));		//initialize
	server_addr.sin_family = AF_INET;					//IPv4
	server_addr.sin_addr.s_addr = inet_addr(argv[1]);	//Ip address
	server_addr.sin_port = htons(atoi(argv[2]));		//port; port# -> sockaddr 할당

	//CONNECT
	if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0)   
		error_handling("Connect error");
	else puts("Connected server");

	//FILE OPEN
	fputs("Input file_name: ", stdout);
	scanf("%s", filename);

	fp = fopen(filename, "rb");
	if (fp == NULL)
		error_handling("File Input Error");

	//SEND FILE NAME
	if (send(sockfd, filename, NAMESIZE, 0) < 0)				//Sending fixed sized file name
		error_handling("File Name Send Error");

	//SEND FILE SIZE
	fseek(fp, 0, SEEK_END);								//Setting file pointer to the end of file
	total_bytes = ftell(fp);							//Saving file size
	rewind(fp);											//Setting file pointer to the start

	if (send(sockfd, (char *)&total_bytes, sizeof(total_bytes), 0) < 0)
		error_handling("File Size Send Error");

	//FILE DATA TRANSFER
	while (1){
		str_len = fread(message, 1, BUFSIZE, fp);
		//FILE DATA TO SEND
		if (str_len > 0){											//File data to send
			if (send(sockfd, message, str_len, 0) < 0)				
				error_handling("Send error");

			str_total += str_len;									//Saving amount of data sent
		}
		else if (str_len == 0 && str_total == total_bytes)			//When data sent == file size
			break;													//File sending finish
		else 
			error_handling("File error");							//File sending error
	}

	printf("Message Sent: %d Bytes\n", str_total);					//File sending success

	//CLOSE
	fclose(fp);
	close(sockfd);

	return 0;
}

void error_handling(char* err_msg)
{
	perror(err_msg);
	exit(1);
}
