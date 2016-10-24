/* Andrew Samuels - absamue
	Nicole Michaud - nmichau
	Parth Patel - pspatel
	Stuart Jackson - sjacks

	CPSC Project 3 - HTTP Server and clients
	Client 2 - ahsoka

	The second client is able to take N number of items as arguments, and
	will send each of them as an add request to the server. Once each item
	has been added, the client will send a view request and then go through
	the payload to ensure that the server processed the items correctly.

	***IMPORTANT***
	Due to the server only supporting append and view functionalies, this
	client will only work with a fresh instance of the server where the
	buffer is empty. If the servers buffer is not empty, the body will
	inlude data from previous requests which will then result in an invalid
	test. However running the same command multiple times in a row will
	verify if the data entries have not changed. 

	Before terminating, the program will display the number of sent
	requests, the time taken in seconds, the returned message, and finally
	the verification status.
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <time.h>
#include <stdbool.h>

//node for holding the a1-aN arguments
struct node{
	char *data;
	struct node *next;
};

//linked list that can be accessed by both main and insert
struct node *head;

//insert a new data node
void insert(char *new_data){
	//create new node
	struct node *new = (struct node*)malloc(sizeof(struct node));
	new->data = malloc(sizeof(new_data));
	strcpy(new->data, new_data);

	//check if head is empty
	if(head == NULL){
		new->next = NULL;
		head = new;
	}
	//add at end
	else{
		struct node *temp = head;
		while(temp->next != NULL){
			temp = temp->next;
		}

		temp->next = new;
	}

}

int main(int argc, char *argv[]){
	clock_t tic = clock(); //start timer

	int sock, n, portno;
	int att = 0;
	unsigned int length;
	struct sockaddr_in serv, from; //address holders
	struct hostent *hp; //host information

	char message[256]; //orignial message from flags
	char ret[256]; //returned message from server
	char *status; //HTTP response status
	char *header = NULL; //header if included
	char body[1000]; //body of HTTP response
	char *verification; //verification status

	//get a1-aN and insert into list
	int i;
	for(i = 0; i<argc; i++){
		if(strstr(argv[i], "-a") != NULL){
			insert(argv[i+1]);
		}
	}

	if(head == NULL){
		printf("At least one data argument must be included\n");
		exit(0);
	}

	//grab the command line flags
	int f;
	while((f = getopt(argc, argv, "p:s:h:a:")) != -1){
		switch(f){
			//port number
			case 'p': portno = atoi(optarg);
						 break;
			//server ip
			case 's': hp = gethostbyname(optarg);
						 if(hp == 0){
							 fprintf(stderr,"Error, host not found.\n");
							 exit(0);
						 }
						 break;
			//header
			case 'h': header = optarg;
						 break;
			//data
			//were just getting getopt to shut up here
			case 'a':
						 break;
			//error message
			default: opterr = 0;
						break;
		}
	}
	
	//get default header if not specified
	if(header == NULL)
		header = hp->h_name;

	//create a datagram socket
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if(sock < 0){
		printf("Error opening socket\n");
		exit(0);
	}

	//set up socket parameters
	serv.sin_family = AF_INET;
	bcopy((char *)hp->h_addr, (char *)&serv.sin_addr, hp->h_length);
	serv.sin_port = htons(portno);
	length = sizeof(struct sockaddr_in);

	//attempt to make the connection
	if(connect(sock, (struct sockaddr *) &serv, length) < 0){
		printf("error on connect\n");
		exit(0);
	}

	//send an add request for all data items
	struct node *temp = head;
	while(temp != NULL){		
		memset(message, 0, sizeof(message));
		sprintf(message, "GET /add?%s HTTP/1.1\nHost: %s\n", temp->data,
				header);

		//write our message to the socket
		n = write(sock, message, sizeof(message));
		if(n < 0){
			printf("error on write\n");
			exit(0);
		}
		//increment attempts
		else
			att++;
	
		//read returned message from the socket
		memset(ret, 0, sizeof(ret));
		n = read(sock, ret, sizeof(ret));
		if(n < 0){
			printf("error on read\n");
			exit(0);
		}

		//get next item
		temp = temp->next;
	
		//see if we should reopen socket
		if(temp !=NULL){
			//create a datagram socket
			sock = socket(AF_INET, SOCK_STREAM, 0);
			if(sock < 0){
				printf("Error opening socket\n");
				exit(0);
			}	
		
			//attempt to make the connection
			if(connect(sock, (struct sockaddr *) &serv, length) < 0){
				printf("error on connect\n");
				exit(0);
			}
		}

	}

	//send a view request to get the buffer
	//create a datagram socket
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if(sock < 0){
		printf("Error opening socket\n");
		exit(0);
	}	
		
	//attempt to make the connection
	if(connect(sock, (struct sockaddr *) &serv, length) < 0){
		printf("error on connect\n");	
		exit(0);
	}
	
	//set up the message
	memset(message, 0, sizeof(message));
	sprintf(message, "GET /view? HTTP/1.1\nHost: %s\n", header);

	//write our message to the socket
	n = write(sock, message, sizeof(message));
	if(n < 0){
		printf("error on write\n");
		exit(0);
	}
	//increment attempts
	else
		att++;

	//read returned message from the socket
	memset(ret, 0, sizeof(ret));
	n = read(sock, ret, sizeof(ret));
	if(n < 0){
		printf("error on read\n");
		exit(0);
	}
	//get info we need out of response
	else{
		//get http status
		char *token = strtok(ret, "\n");	
		status = token;
		if(strstr(status, "OK 200") != NULL){
			verification = "Verified";	
		}
		else{
			verification = status;
		}

		//luckily we know what the response will be, so skip through
		//unrelated info and get body
		memset(body, 0, sizeof(body));
		for(int i=0; i<7; i++){
			token = strtok(NULL, "\n");
		}

		//check to see if the returned info is currect
		temp = head;
		while(temp != NULL){
			//direct compared current item to line of response
			if(strstr(token, temp->data) == NULL)
				verification = "Invalid";
			//success, add to body for printing
			else{
				strcat(body, token);
				strcat(body, "\n");
			}

			//get next token and move down list
			token = strtok(NULL, "\n");
			temp = temp->next;

		}
	}
	//stop clock and print what we got
	clock_t toc = clock();
	double time = (double)(toc - tic) / CLOCKS_PER_SEC;
	printf("%d	%f	%s	%s\n", att, time, body, verification);

	return 0;
}
