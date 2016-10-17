/* Andrew Samuels
 * CPSC 3600
 * Project 1 - Case inversion server
 * This file contains the implementation for running the server. The server
 * can be run by compiling and then running the executable with the -p
 * <portno> flag, where portno is the port number that the server will
 * accept connections from. It is recommened to use a portno of 5K-10K.
 *
 * Upon execution the server will wait for client connections from the
 * yeller or checker client. Once a connection is received, the server will
 * output the message and then invert the case of the letters in the
 * message. After the inversion, the message will be written back to the
 * socket and the server will continue to wait for a new connection.
 *
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

	int sock, length, n, portno;
	socklen_t fromlen;
	char buffer[256]; //holder for recieved messages
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
	sock = socket(AF_INET, SOCK_DGRAM, 0);
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

	//listen for connections until CTRL+C
	while(1){
		//get data from the socket
		n = recvfrom(sock,buffer,256,0,(struct sockaddr*)&from,&fromlen);
		if(n < 0){
			printf("Error on recvfrom\n");
			exit(0);
		}
		else{
			//revieced successful, add to connection count
			count++;

			//get the string of the connected IP and save it in the list
			inet_ntop(AF_INET, &(from.sin_addr), IP, fromlen);
			insert(IP);

		}
		//print what we got
		write(1,"Recieved a datagram: ",21);
		write(1,buffer,n);
		printf("\n");
		
		//invert case
		char ch;
		int i=0;
		for(i; i<strlen(buffer); i++){
			ch = buffer[i];
			if(ch >= 'A' && ch <= 'Z')
				buffer[i] = buffer[i] + 32;
			else if(ch >= 'a' && ch <= 'z')
				buffer[i] = buffer[i] - 32;
		}

		//send back altered message
		n = sendto(sock, buffer, strlen(buffer), 0, (struct sockaddr*)&from, fromlen);
		if(n < 0){
			printf("Error on sendto\n");
			exit(0);
		}
	}

	return 0;
}
