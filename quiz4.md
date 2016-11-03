# Presenation 10 - Repeaters, Bridges, and switches

## Lan limitation
* Only designed to send data so far
* can be fixed with a repeater
* can use hardware that is designed to relay further
* promiscuous
	* bridge connects two LANs and transfers packets between them
	* bridges receive all packets sent on the segment
	* will only forward frames that are valid
	* uses MAC addresses to perform filtering

## Adaptive and learning bridges
* can learn the location of computers on the LAN automatically
* adds computers to a table when a frame passes through a bridge

## Applications of bridges
* connect buildings on a campus
* DSL and cable modems act like bridges

## Distributed spanning tree
* prevents endless bridge loops
* views bridges as nodes in a graph and imposes a tree on the graph
* IEEE 802.1d
* IEEE 802.1w - Rapid spanning tree protocol. 

## Switching
* an ethernet switch is called a layer two switch
* **switch**: digital device that forwards data packets
* permits multiple simultaneous transmissions
* V ports connected to N computers = N/2 transfers at once.

## Virtual switching
* have a single switch emulate multiple switches
* a packet broadcasted will only reach computers connected to the VLAN

## IEEE 802.1Q
* each vlan given unique number: **VLAN tag**
* switch can add tag for internal use only
* this standard only compatible with switches.
* tag is inserted in the middle of a frame header

## Vlan tag process
1. Computer broadcasts a frame
2. switch delivers a copt to each local port
3. switch adds the vlan tag
4. switch sends to connected VLAN switch with tag included
5. Switch removes tag when sending frame to a computer

## Miscellaneous bridge info
* bridges typcially not sold stand-alone
* typically implemented into an ethernet switch
