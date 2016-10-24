/* Andrew Samuels - absamue
	Nicole Michaud - nmichau
	Parth Patel - pspatel
	Stuart Jackson - sjacks

	CPSC Project 3 - HTTP Server and clients
	Server - starLord

	The server is capable of receiving HTTP/1.1 messages. After receiving a
	message, the server will process the message to determine if it was a
	valid request. The server contains a local buffer that is used to hold
	messages sent from the requests. Along with this, the server supports
	add and view functionality, where add will append the related data to
	the buffer and view will send the entire contents of the buffer as part
	of the HTTP response.

	Once the server has successfully processed the request, it will build an
	appropriate response to send back to the socket depending on the add or
	view command received.

	The request and response are displayed on standard out.

	Upon CTRL+C termination, the server will display the number of received
	connections and a list of associated IP's.
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
#include <stdbool.h>
#include <time.h>

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
	char buffer[1000]; //holder for recieved messages
	char process[256]; //holder for read message
	char *response = NULL; //HTTP response
	char data[80]; //message to add to buffer
	struct sockaddr_in server, from; //address holders
	char IP[INET_ADDRSTRLEN];
	bool add = false; //switch between add and view requests
	bool view = false;
	char *date;
	char *last_modified;
	char *content = "Content-Type: text/plain\n";
	char *Server = "Server: Group5/1.1\n\n";
	char *connection = "Connection: close\n";
	char content_length[140];
	char body[1000];
	char ret [1256]; //message to send

	//set up correct size of time items
	time_t t = time(&t);
	struct tm tm = *localtime(&t);

	date = malloc(sizeof("Date:  \n-::") +
		sizeof(tm.tm_mon+1) + sizeof(tm.tm_mday) + sizeof(tm.tm_hour) 
		+ sizeof(tm.tm_min) + sizeof(tm.tm_sec));
	
	last_modified = malloc(sizeof("Last-Modified:  \n-::") +
		sizeof(tm.tm_mon+1) + sizeof(tm.tm_mday) + sizeof(tm.tm_hour) 
		+ sizeof(tm.tm_min) + sizeof(tm.tm_sec));

	sprintf(last_modified, "Last-Modified: %d-%d %d:%d:%d\n",
		tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);

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
		if(newsock == -1){
			printf("Error on accept\n");
			exit(0);
		}	
		else{
			//revieced successful, add to connection count
			count++;

			//get the string of the connected IP and save it in the list
			inet_ntop(AF_INET, &(from.sin_addr), IP, fromlen);
			insert(IP);
		}


		//get message
		bzero(process, 255);
		int n = read(newsock, process, 255);
		printf("--------------------------------\n");
		printf("%s\n", process);

		//break message into tokens
		char *token = strtok(process, " ");
		
		//check if GET
		if(strcmp(token, "GET") != 0){
			printf("Only GET method allowed\n");
			response = "HTTP/1.1 405 Method Not Allowed\nAllow: GET\n";
			printf("%s\n", token);
		}

		
		//next token, add or view
		token = strtok(NULL, " ");

		//check if add
		if(strstr(token, "/add?") != NULL){
			add = true;
			//grab the substring of the data parameter
			char *sub = strstr(token, "/add?");
			bzero(data, 79);
			strcpy(data, sub + 5);

		}
		//not add, check if view
		else if(strstr(token, "/view?") != NULL){
			view = true;

			//copy the buffer into body
			memset(body, '\0', sizeof(body));
			strcpy(body, buffer);
			strcat(body, "\n");

		}
		//bad action, give 404
		else{
			response = "HTTP/1.1 404 Not Found\n";
		}

		//get next token, HTTP protocol
		token = strtok(NULL, " \n");

		//make sure we get 1.1
		if(strcmp(token, "HTTP/1.1") != 0){
			response = "HTTP/1.1 400 Bad Requesti\n";
		}


		//get next token, host
		token = strtok(NULL, " ");

		//check if host header is included
		if(strcmp(token, "Host:") != 0){
			printf("Request must include host header\n");
		}
		//host included, get the hostname
		else{
			//see if we are supposed to add to the buffer
			if(add == true){
				//get the hostname
				token = strtok(NULL, " ");
				char *hostname = malloc(sizeof(token));
				hostname = token;

				//build the data entry, data from action followed by host
				char *add_data = malloc(sizeof(data) + sizeof(hostname) +
						sizeof(" \n"));
				strcpy(add_data, data);
				strcat(add_data, " ");
				strcat(add_data, hostname);

				//add to the buffer
				if((sizeof(buffer) - strlen(buffer)) > strlen(add_data))
					strcat(buffer, add_data);
				else
					printf("Not enough space in buffer\n");

				//record modified time
				time(&t);
				tm = *localtime(&t);
				memset(last_modified, 0, sizeof(last_modified));
				sprintf(last_modified, "Last-Modified: %d-%d %d:%d:%d\n",
						tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min,
						tm.tm_sec);

			}

		}

		//PROCESSING COMPLETE, build response
		if(response == NULL)
			response = "HTTP/1.1 OK 200\n";
	

		//get date
		time(&t);
		tm = *localtime(&t);
		memset(data, 0, sizeof(date));
		sprintf(date, "Date: %d-%d %d:%d:%d\n",
				tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min,
				tm.tm_sec);

		//not a view request, make sure we dont send meaningless data
		if(view == false)
			memset(body, '\0', sizeof(body));

		//write the size of the message
		memset(content_length, 0, sizeof(content_length));
		sprintf(content_length, "Content-Length: %d\n", (strlen(body) / 8));

		//build the response
		strcpy(ret, response);
		strcat(ret, connection);
		strcat(ret, date);
		strcat(ret, last_modified);
		strcat(ret, content);
		strcat(ret, content_length);
		strcat(ret, Server);
		strcat(ret, body);

		printf("%s", ret);
		printf("--------------------------------\n");
		
		n = write(newsock, ret, sizeof(ret));
		//close the client socket
		close(newsock);
		
		//reset some of the things we used
		response = NULL;
		add = false;
		view = false;
		memset(ret, 0, sizeof(ret));
		
	}

	//never reached :(
	//OS will take care of this on ctrl-c
	close(sock);

	return 0;
}
