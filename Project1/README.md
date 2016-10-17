#UDP Echo Server and Clients

This project contains a UDP echo server, **caseInverter**, and two clients
**yeller** and **checker**. The project can be run by using the makefile to
generate the executables, and then follow the instructions below to run
each individual program.

##caseInverter Server

caseInverter.c contains the implementation for running the server. The server
can be run by compiling and then running the executable as follows:
`./caseInverter -p <portno>`

`<portno>` is the port that the client will attempt to connect to at the
server. The portno must be the same as the one that was used with the -p
flag when running caseInverter.

Upon execution the server will wait for client connections from the
yeller or checker client. Once a connection is received, the server will
output the message and then invert the case of the letters in the
message. After the inversion, the message will be written back to the
socket and the server will continue to wait for a new connection.

##Yeller Client

yeller.c contains the implemetation for running the yeller client. The
client can be run by compiling and then running the executable as
follows:
`./yeller -p <portno> -s <server IP/name> -m <message>`

`<portno>` is the port that the client will attempt to connect to at the
server. The portno must be the same as the one that was used with the -p
flag when running caseInverter.

`<server IP/name>` is the IP address or name of the server. This should be
the IP address of the physical machine that is running the server. The
name of the server machine can be used if the client machine is networked to
the server machine.

`<message>` is the message that will be sent to the server to be inverted.

Upon execution, the client will make the connection to the server, send
the message, recieve the response, and finally display the following
information:

1. Connection attempts
2. Time elapsed
3. Original message 
4. returned message.

##Checker Client

chcker.c contains the implemetation for running the checker client. The
client can be run by compiling and then running the executable as
follows:
`./checker -p <portno> -s <server IP/name> -m <message>`

`<portno>` is the port that the client will attempt to connect to at the
server. The portno must be the same as the one that was used with the -p
flag when running caseInverter.

`<server IP/name>` is the IP address or name of the server. This should be
the IP address of the physical machine that is running the server. The
name of the server machine can be used if the client machine is networked to
the server machine.

`<message>` is the message that will be sent to the server to be inverted.

Upon execution, the client will make the connection to the server, send
the message, and then send the recieved message once more to confirm
that the server is correct. If the original message is the same as the
second returned message then we have confirmed that the server is
operating correctly. 
 
The program will then finally display the following information:

1. Connection attempts
2. Time elapsed 
3. Original message
4. Returned message
5. Verifvication status

##makefile

The makefile will automatically compile the files to produce the
appropirate executable name as in the above examples.
