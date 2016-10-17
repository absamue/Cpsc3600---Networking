/* Andrew Samuels
 * CPSC 3600
 * Project 3 
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include <arpa/inet.h>

//node to hold IP's that connect
struct node{
	char IP[INET_ADDRSTRLEN];
	struct node *next;
};

//count connections and IP's globally so they can be used after CTRL+C
int count = 0;
struct node *head;

//catch CTRL+C, print out necessary info and exit
void intHandler(int handler){
	//print number of connections
	printf("\n%d	", count);

	//print all of the unique IP's by going through list
	struct node *temp = head;
	while(temp != NULL){
		printf("%s", temp->IP);
		if(temp->next != NULL)
			printf(", ");
		temp = temp->next;
	}
	printf("\n");
	exit(0);
}

//insert connected IPs, ignoring duplicates
void insert(char *newIP){

	//first node
	if(head == NULL){
		struct node *new = (struct node*)malloc(sizeof(struct node));
		strcpy(new->IP, newIP);
		new->next = NULL;

		head = new;
	}
	//check if new IP
	else{
		struct node *temp = head;
		while(temp != NULL){
			//exit if we already have seen it
			if((strcmp(newIP, temp->IP)) == 0){
				return;
			}
			temp = temp->next;
		}

		//it's new, add to list
		struct node *new = (struct node*)malloc(sizeof(struct node));
		strcpy(new->IP, newIP);
		new->next = head;
		head = new;
	}
}

int main(int argc, char *argv[]){
	signal(SIGINT, intHandler);

	int sock, newsock, length, n, portno;
	socklen_t fromlen;
	char buffer[1024]; //holder for recieved messages
	struct sockaddr_in server, from; //address holders
	char IP[INET_ADDRSTRLEN];
	//get command line flags
	int f;
	while((f = getopt(argc, argv, "p:")) != -1){
		switch(f){
			//port
			case 'p': portno = atoi(optarg);
						 break;
			default:
						 printf("Please use the -p flag to select a port number in the range 5K-10k\n");
						 return 0;
		}
	}

	//create a datagram socket
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if(sock < 0){
		printf("error opening socket\n");
		exit(0);
	}
	//set up socket parameters
	length = sizeof(server);
	bzero(&server, length);
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(portno);

	//bind the socket to the host ip address
	if(bind(sock, (struct sockaddr *) &server, length) < 0){
		printf("Error on bind\n");
		exit(0);
	}

	fromlen = sizeof(struct sockaddr_in);

	//start listening to the socket, maximum 5 connections
	listen(sock, 5);

	//accept connections until ctrl+c
	while(1){
		//get client file descriptor once they connect
		newsock = accept(sock, (struct sockaddr *) &from, &fromlen);

		//read the sent message into the buffer
		bzero(buffer, 256);
		n = read(newsock, buffer, 255);
		if(n < 0){
			printf("Error on read\n");
			exit(0);
		}
		else{
			//revieced successful, add to connection count
			count++;

			//get the string of the connected IP and save it in the list
			inet_ntop(AF_INET, &(from.sin_addr), IP, fromlen);
			insert(IP);
		}

		

		//close the client socket
		close(newsock);
	}

	//never reached :(
	//OS will take care of this on ctrl-c
	close(sock);

	return 0;
}
