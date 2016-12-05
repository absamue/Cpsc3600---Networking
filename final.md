# Quiz 3

1. Repeater is a digital device that amplifies and relays information.
	* False
2. In the Ad hoc On-demand Distance Vector routing algorithm, when the
ROUTE REQUEST packet reaches the destination node, the node creates a ROUTE
REPLY packet and broadcasts it back to the sender.
	* False
3. Each IP address is divied into a prefix and suffix, where prefix
identifies physical network to which a host is attached and suffix
identifies a specific host within that network.
	* True
4. IPv6 addresses, which are represented with a colon hexidecimal notation,
	have a 3 level heirarchy. **k**  bit global prefix, **64-k** bit subnet,
	and **64** bit interface.
	* True
5. The sending host in IPv4 is the one that must perform fragmentation by
choosing a datagram size that is smaller than the Maximum Transmission Unit
on the network.
	* False
6. Which statement is **not true** regarding a brige and its application?
	* A bridge uses IP addresses to perform filtering
7. Ethernet bridges communicate among themselves using a multicast address
that is reserved for spanning tree. Which of the following is the multicast
address?
	* 01:80:C2:00:00:00
8. Which statement is **not true** regarding the Distance Vector Routing
algorithm?
	* Uses Dijkstra's shortest path algorithm to compute a forwarding table.
9. Which of teh following example is a special "limited broadcast" IPv4
address that is used by a host to send a direct broadcast when the network
ID is not know? This address can only be used as the destination address.
	* 255.255.255.255
10. Using the Link-State Routing algorithm, what is the next-hop node in
node a's forwatding table if it wants to send packets to node e?
	* c
11. How many bytes does IPv4 and Ipv6 use in their address representation?
	* IPv4: 4 bytes, 16 bits
	* IPv6: 16 bytes, 64 bits
12. In the classful shceme of IPv4 addresses, which class (A/B/B) can
support the most hosts on one physical network?
	* A
13. Suppose Clemson University needs 50 Ip addresses to accommodate
internet connection to its schools and departments. What is the maximum
number of bits in the IPv4 prefix that can be assigned by an ISP to
minimize the number of unused suffixes?
	* 26
14. If a datagram packet arrives at router R1, destined to 128.100.50.15
and there are two entries in R1's forwarding table, where will the packet
be sent to?
	* Destination: 128.100.50.0. 
	* Mask: 255.255.255.0
	* Next hop: 192.5.0.7
15. How can a datagram packet be sent across a physical network that
doesn't understand the datagram format?
	* Make the packet the payload of a frame that the network understands.

# Quiz 4

1. Frames transmitted acress a physical network must contain the MAC
address of the next hop. So, a next-hop IP address must be resolved to an
equivalent MAC address.
	* True
2. Both Adress Resolution Protocol and Internet Communication Message
Protocol messages are encapsulated directly as frame payload for
transmission.
	* False
3. NAPT allows for arbitrary number of application on arbitrary hosts to
communicate simultaneously with arbitrary destinations. FOr this purpose,
NAPT only needs to know IPaddresses, which are stored in its' translation
table.
	* False
4. TCP proves a reliable, connection-oriented, full-duplex stream transport
service that allows two application programs to form a connection, send
data in either direction, and then terminate the connection. Each TCP
connection is started reliably and terminated gracefully.
	* True
5. To handle packet loss, TCP uses positive acknowledgement with
retransmission, It means, whenever a frame arrives intact, the receiving
protocol software sends a small acknowledgement message that reports
successful reception.
	* True
6. Which statement is **not true** regarding the Dynamic Host Configuration
Protocol lease?
	* When a lease expires, a host cannot re-negotiate a longer lease.
7. Which of the following is the non-routable class C block of private IPv4
addresses used by Network Address Translation?
	* 192.169.0.0/16
8. Which of the following is **not true** regarding User Datagram Protocol?
	* If a packet is lost, UDP will retransmit it.
9. What is the term for when a packet from a previous interrupted
transmission is used incorrectly in a subsequent transmission?
	* Replay
10. Border Gateway Protocol
	* Exterior Gatewat protocol
11. Routing Information Protocol
	* Interior Gateway protocol
12. Its forwarding table does not change throughout the entire life of the
network.
	* Static routing
13. It is an IP companion protocol used to report errors back to the
original source.
	* ICMP
14. It uses flow control technique known as sliding window.
	* TCP
17. In which layer are the following protocols implemented?
	* IP = 3
	* TCP = 4
	* UDP = 4
20. What does TCP slow start mean?

TCP will begin by sending a small amount of data. If an ACK is recieved 
for all data, the amount sent is doubled. This exponential increase continues
until half of the sending/receiving capacity is reached. At this point it
will switch to a linear increase to ensure congestion is not caused. 
