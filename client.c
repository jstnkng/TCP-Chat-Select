#include<sys/socket.h>
#include<netinet/in.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<sys/select.h>

//Justin King
//Client.c
//CIS 457
int main(int argc, char** argv)
{
	printf("Enter server IP Address: ");
	char serverIP[20];
	fgets(serverIP, 20, stdin);
	printf("Enter port number: ");
	char  port[5];
	fgets(port, 5, stdin);
	int portNumber = atoi(port);

	//File descriptor
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
	{
		printf("Error creating socket\n");
		return 1;
	}
	fd_set sockets;
	FD_ZERO(&sockets);
	
	struct sockaddr_in serveraddr;
	serveraddr.sin_family= AF_INET;
	//htons determines byte order (Host to Network Short)
	serveraddr.sin_port = htons(portNumber);
	serveraddr.sin_addr.s_addr = inet_addr(serverIP);

	int e = connect(sockfd,(struct sockaddr*) &serveraddr, sizeof(serveraddr));
	if (e<0)
	{
		printf("Error connecting...\n");
		return 2;
	}
	printf("Connected to server!\n");
	//Add stdin and sockfd to the list of sockets
	FD_SET(0, &sockets);
	FD_SET(sockfd, &sockets);
	printf("Enter a message: (or Quit)\n");
	while (1)
	{
		fd_set tmp_set = sockets;
		select(FD_SETSIZE, &tmp_set, NULL, NULL, NULL);
		int i;
		for(i=0;i<FD_SETSIZE;i++)
		{
			if (FD_ISSET(i, &tmp_set))
			{
				if (i == 0)
				{
					//If stdin, send a message to the server
					char message[999];
					fgets(message, 999, stdin);
					send(sockfd,message,strlen(message)+1,0);
					if (strcmp(message, "Quit\n")==0)
					{
						FD_CLR(i, &sockets);
						return 0;
					}
				} 
				else{
					//Receive incoming messages from the server
					char received[999];
					recv(i, received,999, 0);
					if (strcmp(received, "\n") != 0){
						printf("Server: %s\n", received);
					}
					if (strcmp(received, "Quit\n")==0){
						close(i);
						FD_CLR(i, &sockets);
						return 0;
					}
				}
			}
		}
	}

	return 0;
}
