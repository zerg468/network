#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

#define BUFSIZE 2048
#define NAMESIZE 256

int recvn(int socket, char *buf, int len, int flags);
void error_handling(char* err_msg);

int main(int argc, char **argv)
{
	int server_sockfd, client_sockfd;
	char message[BUFSIZE + 1];
	char filename[NAMESIZE];
	int str_len, str, total_bytes, rcv_total = 0;
	FILE *fp = NULL;

	struct sockaddr_in server_addr;
	struct sockaddr_in client_addr;
	int client_addr_size;

	//ARGUMENT CHECK
	if (argc < 2)
	{
		fprintf(stderr, "Usage: %s [port]\n", argv[0]);
		exit(1);
	}


	//SOCKET
	server_sockfd = socket(PF_INET, SOCK_STREAM, 0);
	if (server_sockfd < 0)
	{
		perror("Socket error");
		exit(1);
	}

	//BIND
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(atoi(argv[1]));

	if (bind(server_sockfd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0)
		error_handling("bind error");

	//LISTEN
	if (listen(server_sockfd, 5) < 0) 
		error_handling("listen error");

	client_addr_size = sizeof(client_addr);

	while (1){
		//ACCEPT
		client_sockfd = accept(server_sockfd, (struct sockaddr*)&client_addr, &client_addr_size); 
		if (client_sockfd < 0)
			error_handling("accept error");
		else printf("Connectd client\n");

		//FILE NAME RECEIVE
		str_len = recvn(client_sockfd, filename, NAMESIZE, 0);							//Receiving fixed-sized file name
		if (str_len < 0)
			error_handling("Receive filename error");

		//FILE SIZE RECEIVE
		str_len = recvn(client_sockfd, (char *)&total_bytes, sizeof(total_bytes), 0);		//Receiving File total size
		if (str_len < 0)
			error_handling("Receive file size error");
		
		//OPEN FILE
		fp = fopen(filename, "wb");
		if (fp == NULL)
			error_handling("File error");

		//FILE DATA RECEIVE
		while (1)
		{
			str_len = recvn(client_sockfd, message, BUFSIZE, 0);
			if (str_len < 0)																//Receiving error
				error_handling("Receive file data error");

			else if (str_len == 0) break;													//All data received
			else																			//Data read -> write to a file
			{
				fwrite(message, 1, str_len, fp);
				if (ferror(fp))																//File writing error
					error_handling("Receive file data writing error");

				rcv_total += str_len;														//Saving total bytes received
			}
		}
								
		printf("File Receiving Success\n");												

		//CLOSE
		fclose(fp);
		close(client_sockfd);
	}

	close(server_sockfd);
	return 0;
}

//Receiving fuction to get full data the client sent
int recvn(int socket, char *buf, int length, int flags)
{
	int received;
	char *ptr = buf;	//buffer full
	int left = length;		//left buffer size

	while (left > 0)
	{
		received = recv(socket, ptr, left, flags);
		if (received < 0)
			error_handling("Socket Error");
		
		else if (received == 0) break;

		left -= received;
		ptr += received;
	}
	return (length - left);
}

void error_handling(char* err_msg)
{
	perror(err_msg);
	exit(1);
}
