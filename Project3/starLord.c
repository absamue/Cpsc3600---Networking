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
	char buffer[1024]; //holder for recieved messages
	char process[256];
	char *response = NULL;
	char *data = NULL;
	struct sockaddr_in server, from; //address holders
	char IP[INET_ADDRSTRLEN];
	bool add = false;
	bool view = false;
	char *date;
	char *last_modified;
	char *content;
	char *Server;
	char *connection;
	char *body;
	char *ret;

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

		//get first line
		int n = read(newsock, process, 255);

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
			if(data != NULL)
				free(data);
			data = malloc(sizeof(token) - sizeof("/add?"));
			char *sub = strstr(token, "/add?");
			data = sub + 5;

		}
		//not add, check if view
		else if(strstr(token, "/view?") != NULL){
			view = true;
			//grab the substring of the data parameter
			if(data != NULL)
				free(data);
			data = malloc(sizeof(token) - sizeof("/view?"));
			char *sub = strstr(token, "/view?");
			data = sub + 6;
		}
		//bad action, give 404
		else{
			response = "HTTP/1.1 404 Not Found";
		}


		//get next token, HTTP protocol
		token = strtok(NULL, " \n");

		//make sure we get 1.1
		if(strcmp(token, "HTTP/1.1") != 0){
			response = "HTTP/1.1 400 Bad Request";
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
				strcat(add_data, "\n");

				//add to the buffer
				strcat(buffer, add_data);

				//record modified time
				time_t t = time(NULL);
				struct tm tm = *localtime(&t);
				last_modified = malloc(sizeof("Last-Modified:  \n-::") +
						sizeof(tm.tm_mon) + sizeof(tm.tm_mday) +
						sizeof(tm.tm_hour) + sizeof(tm.tm_min) +
						sizeof(tm.tm_sec));
				sprintf(last_modified, "Last-Modified: %d-%d %d:%d:%d\n",
						tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min,
						tm.tm_sec);
			}

		}
		//PROCESSING COMPLETE, build response
		if(response == NULL)
			response = "HTTP/1.1 OK\n";
	
		connection = "Connection: close\n";

		//get date
		time_t t = time(NULL);
		struct tm tm = *localtime(&t);
		date = malloc(sizeof("Date:  \n-::") +
						sizeof(tm.tm_mon) + sizeof(tm.tm_mday) +
						sizeof(tm.tm_hour) + sizeof(tm.tm_min) +
						sizeof(tm.tm_sec));
		sprintf(date, "Date: %d-%d %d:%d:%d\n",
				tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min,
				tm.tm_sec);

		content = "Content-Type: text/plain\n";
		Server = "Server: Group5/1.0\n";

		if(view == false)
			body = NULL;
		else{
			body = malloc(sizeof(buffer) + sizeof("\n"));
			strcpy(body, "\n");
			strcat(body, buffer);
		}
//		ret = malloc(sizeof(response) + sizeof(date) + sizeof(last_modified) +
//				sizeof(content) + sizeof(Server) + sizeof(connection) +
//				sizeof(body));
//		strcpy(ret, response);
//		strcat(ret, date);
//		strcat(ret, connection);
//		strcat(ret, last_modified);
//		strcat(ret, content);
//		strcat(ret, Server);
//		strcat(ret, body);


	printf("%s%s%s%s%s%s%s", response, date, connection, last_modified,
			content, Server, body);

		n = write(newsock, ret, sizeof(ret));
		//close the client socket
		close(newsock);
		response = NULL;
	}

	//never reached :(
	//OS will take care of this on ctrl-c
	close(sock);

	return 0;
}
