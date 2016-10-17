/* Andrew Samuels
 * CPSC 3600
 * Project 1 - Case inversion server
 *
 * This file contains the implemetation for running the bellower client. The
 * client can be run by compiling and then running the executable as
 * follows:
 * ./bellower -p <portno> -s <server IP/name> -m <message>
 *
 * <portno> is the port that the client will attempt to connect to at the
 * server. The portno must be the same as the one that was used with the -p
 * flag when running caseConverter.
 *
 * <server IP/name> is the IP address or name of the server. This should be
 * the IP address of the physical machine that is running the server. The
 * name of the server machine can be used if the client machine is networked to
 * the server machine.
 *
 * <message> is the message that will be sent to the server to be inverted.
 *
 * Upon execution, the client will make the connection to the server, send
 * the message, recieve the response, and finally display the following
 * information:
 *
 * Connection attempts, time elapsed, original message, returned message.
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

int main(int argc, char *argv[]){
	clock_t tic = clock(); //start timer

	int sock, n, portno;
	int att = 0;
	unsigned int length;
	struct sockaddr_in serv, from; //address holders
	struct hostent *hp; //host information

	char orig[256]; //orignial message from flags
	char ret[256]; //returned message from server


	//grab the command line flags
	int f;
	while((f = getopt(argc, argv, "p:s:m:")) != -1){
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
			case 'm': strcpy(orig, optarg);
						 break;
			//error message
			default: fprintf(stderr, "Usage: bellower -s <serverIP> -p <serverPort> -m <message>\n");
						break;
		}
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

	//write our message to the socket
	n = write(sock, orig, 255);
	if(n < 0){
		printf("error on write\n");
		exit(0);
	}
	//increment attempts
	else
		att++;

	//read returned message from the socket
	n = read(sock, ret, 255);
	if(n < 0){
		printf("error on read\n");
		exit(0);
	}

	//stop clock and print what we got
	clock_t toc = clock();
	double time = (double)(toc - tic) / CLOCKS_PER_SEC;
	printf("%d	%f	%s	%s\n", att, time, orig, ret);

	return 0;
}
