/* Andrew Samuels
 * CPSC 3600
 * Project 3 */
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
#include <regex.h>

int main(int argc, char *argv[]){
	clock_t tic = clock(); //start timer

	int sock, n, portno;
	int att = 0;
	unsigned int length;
	struct sockaddr_in serv, from; //address holders
	struct hostent *hp; //host information

	char message[256]; //orignial message from flags
	char ret[256]; //returned message from server
	char *action = NULL;
	char *data;
	char *status;
	char body[1000];
	bool view = false;
	int i;
	for(i = 0; i<argc; i++){
		if(strstr(argv[i], "-a") != NULL)
			printf("%s\n", argv[i+1]);
	}

	//grab the command line flags
	int f;
	while((f = getopt(argc, argv, "p:s:a:h:v")) != -1){
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
			//message
			case 'a': 
						 break;
			//header
			case 'h':
						 break;
			//view request
			case 'v': view = true;
						 break;
			//error message
			default: 
						break;
		}
	}
	


	if(view == true){
		if(action != NULL)
			free(action);
		action = malloc(sizeof("/view?"));
		sprintf(action, "/view?");
	}

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

	sprintf(message, "GET %s HTTP/1.1\nHost: %s\n", action, hp->h_name);

	printf("%s", message);

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

		//luckily we know what the response will be, so skip through
		//unrelated info and get body
		memset(body, 0, sizeof(body));
		for(int i=0; i<7; i++){
			token = strtok(NULL, "\n");
		}
		while(token != NULL){
			strcat(body, token);
			strcat(body, "\n");
			token = strtok(NULL, "\n");
		}
	}
	//stop clock and print what we got
	clock_t toc = clock();
	double time = (double)(toc - tic) / CLOCKS_PER_SEC;
	printf("%d	%f	%s	%s	%s\n", att, time, data, status, body);

	return 0;
}
