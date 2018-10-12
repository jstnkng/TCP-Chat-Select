#include<sys/socket.h>
#include<netinet/in.h>
#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<stdlib.h>
#include<sys/select.h>

//Justin King
//Server.c
//CIS 457

int main(int argc, char** argv)
{
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	fd_set sockets;
	FD_ZERO(&sockets);
	printf("Enter port number: ");
	char port[5];
	fgets(port, 5, stdin);
	int portNumber = atoi(port);
	struct sockaddr_in serveraddr,clientaddr;

	bzero(&serveraddr, sizeof(serveraddr));

	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(portNumber);
	serveraddr.sin_addr.s_addr = INADDR_ANY;

	int b = bind(sockfd, (struct sockaddr*) &serveraddr, sizeof(serveraddr));
	if (b < 0)
	{
		printf("Error binding address...\n");
		return 1;
	}
	else printf("Server started...\n");
	listen(sockfd, 10);
	FD_SET(0, &sockets);
	FD_SET(sockfd, &sockets);
	int clientsocket = 0;
	while (1)
	{
		fd_set tmp_set = sockets;
		select(FD_SETSIZE, &tmp_set, NULL, NULL, NULL);
		int i;
		for(i=0;i<FD_SETSIZE;i++)
		{
			if (FD_ISSET(i, &tmp_set))
			{
				if (i == sockfd)
				{
					//Handle new client connections
					printf("Connected to client.\n");
					int len = sizeof(clientaddr);
					clientsocket = accept(sockfd, (struct sockaddr*) &clientaddr, &len);
					FD_SET(clientsocket, &sockets);
					printf("Enter a message: (or Quit)\n");
				}
				else if (i == 0)
				{
					//If the socket is stdin, send a message to the client
					char message[999];
					fgets(message,999,stdin);
					if (clientsocket != 0){
						send(clientsocket,message,strlen(message)+1,0);
					}
					if (strcmp(message, "Quit\n")==0)
					{
						close(i);
						FD_CLR(i, &sockets);
						return 0;
					}
				}
				else if (i == clientsocket){
					//Receive messages from the client
					char received[999];
					recv(i, received,999, 0);
					if(strcmp(received,"\n") != 0){
						printf("Client: %s\n", received);
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
