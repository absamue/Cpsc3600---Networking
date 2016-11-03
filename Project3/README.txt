CPSC 3600 
Group 5: Nicole Michaud, Parth Patel, Andrew Samuels, Staurt Jackson
Project 3: HTTP client/server
10/24/2016

Overview: This project contains an HTTP server(starLord) and 2 clients: chopper and ahsoka. Chopper has 2 possible modes of operation both of which send a GET request to the server and display the payload received. The second client, Ahsoka, verifies the request was done correctly. The server receives data parses it to decide if it is valid, and then performs an add or view. StarLord then sends the data as the payload in an HTTP response. 

Chopper: Through parsing the arguments, we determined which mode to operate chopper in: -v for view or -a for simply inputting a message. If in view mode, the client creates a view request and the message is not sent as an add request. If in input mode, the message is sent as an add request. The header is added to the message before it is sent to the client. Then the info returned by the server is parsed and output.

Ahsoka: Since any amount of messages can be sent to this client, we looped
through the comamnd line arguments and checked if we found a substring
matching "-a", and then added the next argument to a linked. Each of the
items in the linked list is then send as an add request. Finally a view
request is sent to check if the items were added correctly.  If the message returned by the client is what we expect, then the message is verified. 

StarLord: The server keeps track of a linked list of IPs that have connected to it as well as when the message was last modified. This information is output when the server is closed. Once connected to a client, it waits for a message and performs a GET request on the message. If the client is in input mode, the message is added. If it is in view mode, it is not. HTTP protocol is checked and information is added to the header and then sent back to the client. 

How To Run: Simply type make to compile and build the program. Use the appropriate flags to run the client and server. 
  Chopper: ./chopper -s <serverIP> -p <serverPort> -h <hostHeader> [-a <message>][-v]
  Ahsoka: ./ahsoka -s <serverIP> -p <serverPort> -h <hostHeader> -a1 <message>...-aN <messageN>
  StarLord: ./starLord -p <serverPort>
