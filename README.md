# Distributed-Hash-Table-Pastry-Algorithm
In P2P systems, networks organise themselves into overlay networks and
relay/store data for each other. An overlay network is a network that is
built on top of another network. Nodes in an overlay network can be
thought of as being connected to each other by virtual or logical links,
each of which is corresponding to a physical path in the underlying
network. In distributed systems such as P2P networks and client-server
applications, the nodes run on top of Internet.
Pastry is a structured type of an overlay network, structured in the
sense that it imposes particular structures on the overlay networks. The
structuring is necessary to achieve efficient resource search in such
large scale distributed storage networks. The structure imposed by Pastry
is that of Distributed Hash Tables. Like any other structured P2P
overlay, Pastry maintains a structure to accommodate the participating
nodes and their corresponding routing data, aside to the routing
algorithm to be followed by the nodes to enable communication. Also
maintained is a structured format of join/leave mechanism that ensures
fault tolerance and enables self-organisation.
Distributed Hash Tables apply a hashing function (like the Secure Hash
Algorithm version 1, SHA-1), to form the node IDs. IDs for the data items
are created by applying the same hash function. The node IDs and data IDs
fall into the same address space. These data times are stored on the
closest node with the node ID greater than or equal to the data iD. If
the node with the closest node ID does not store the data item, then the
node does not exist in the system. Following the respective data storage
tables, any node in the system can be found in the overlay.
P2P systems are highly dynamic, with respect to the nodes that can join
or leave at anytime during the functioning of the overlay. For a node to
join, it needs to acquire a unique node ID and then position itself into
the overlay structure based on the node ID and the geometry of the
system. Subsequently, all the routing tables of the nodes affected by the
join of the new node and the routing table of the joining node itself are
updated. A new node willing to join the system first contacts a
bootstrapping server and gets a partial list of existing nodes. This lets
the joining nodes know in advance an entry point into the network.
Something similar happens when a node leaves the system. When a node
leaves the system or becomes unreachable, nodes that point to that node
are affected. The routing table entries are to be updated as a result. In
the case when he node does not leave the network abruptly, it notifies
its neighbours about its departure, and then these nodes forward the
information about the departure of this node to the other nodes in the
networks. But under circumstances of unexpected leave by a node, the node
leaving the network will not be able to notify to the neighbouring nodes.
In such a scenario, an efficient fault tolerance and detection mechanism,
such as keep-alive messages or periodic checking, is required.
Pastry was proposed in 2001 by Antony Rowstron and Peter Druschel and was
developed at Microsoft Research Ltd, Rice University, Purdue University
and University of Washington.
In Pastry, data items and nodes have a unique 128-bit IDs ranging from 0
to 2128 - 1. For routing, these IDs are treated of as sequences of digits
in base 2b. Typically, b = 4, so these digits are hexadecimal. These IDs
are arranged as a circle modulo 2128. These node IDs are generated
randomly and distributed in the ID space. These nodes are arranged as a
ring, in the ascending order of the node IDs. So, the basic idea of
routing involves finding the 128 bit hash for a given key, finding the
node closest to the hash value and subsequently, sending the request to
that particular node.
Each node in Pastry maintains 3 tables, Routing table, Leaf set and
Neighbourhood set.
Routing table contains floor(log 2b N) rows with 2b number of columns,
where N is the number of pastry nodes.
The entries at row i point to nodes that share the first i digits of the
prefix with the key. Each cell refers to a base 2b digit. If the digit
associated with a cell matches the (i+1)th digit of the key, then we have
a node that matches the key with a longer prefix. The request is then
routed to that node. Thus, Pastry can route to the numerically closest
node to a given key in less than floor(log 2b N) steps under normal
operation.
Leaf set is a set of L nodes that contains L/2 nodes with the numerically
closest larger node IDs and L/2 nodes with numerically smaller node IDs.
Neighbourhood set M contains the node IDs and the IP addresses of the |M|
nodes that are closest to the local node. This set is normally not used
in routing messages, but it is useful in maintaining locality properties.
Routing Algorithm:
1. Send the join request to the neighbours.
2. Maximal prefix matching of the node ID with its neighbouring node ID
in the leaf set.
1. If found in leaf set
1. Send the join request to the neighbours
2. Send the routing table to the neighbours
2. Else
1. Search in routing table based on prefix
2. Send the routing table and join request to all the corresponding
nodes in the matched prefix row.
3. Else if not found anywhere
1. Create an entry in neighbourhood set
2. Send the join request to every node present in neighbourhood
set.
Our Implementation of Pastry:
Functions-
1. Port <X>
creating a node which has its listening port as X.
2. Join <Node ID> <IP> <Port>
Joining a new node to the existing network that has Node ID, IP and
a listening Port.
3. Put <key> <value>
Store the key value pair with matched prefix Node ID
4. Get <key>
Retrieve the key value pair with matched prefix Node ID
5. lset
Prints the leaf set of a pastry node.
6. nset
Prints the neighbourhood set of a pastry node.
7. routetable
Prints the routing table of a pastry node.
8. quit
Deletes a pastry node and moves its data to one of its
neighbourhood nodes.
9. shutdown
Shuts down the entire pastry network.
