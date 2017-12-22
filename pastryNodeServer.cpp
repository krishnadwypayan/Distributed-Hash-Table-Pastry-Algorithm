#include "pastry.h"

string nodeID;
distributedHashTable dht;
routingTable node;
string message;
string remaining;
connectionInfo connection;
connectionInfo recvDetails;
string sendIP;
int sendPort;
int value_found=0;
string delim1 = "$#$";
int shutup = 0;

vector<string> split(string s){
    vector<string> list;
    size_t pos;
    string token;
    while ((pos = s.find(delim1)) != string::npos) {
        token = s.substr(0, pos);
        list.push_back(token);
        s.erase(0, pos + delim1.length());
    }

    if(s.find('\0') != string::npos){
        token = s.substr(0, s.find('\0'));
        list.push_back(token);
    }
    else if(s.find('\0') == string::npos){
        list.push_back(s);
    }
    return list;
}

// Function to insert default IP and port
void checkAndInsert(string hashedDetails, string receivedIP, string receivedPort) {
	int where;
	map<string,vector<pair<string,int> > >::iterator it;
    if(hashedDetails.substr(0, hashedDetails.length() - 1) == nodeID.substr(0,nodeID.length() - 1)) {
    	where = LEAF_SET;
        node.leaf_set.push_back(make_pair(receivedIP, stoi(receivedPort)));        
    }
    else if(where != LEAF_SET){
        // searching for first 2 chars
        if(hashedDetails.substr(0, hashedDetails.length() - 2) == nodeID.substr(0,nodeID.length() - 2)) {
        	where = ROUTING_TABLE;
        	it = node.routing_set.find(hashedDetails.substr(0, 2));
        	if(it == node.routing_set.end()) {	        		
        		node.routing_set[hashedDetails.substr(0, hashedDetails.length() - 2)].push_back(make_pair(receivedIP, stoi(receivedPort)));
        	}
        	else {
          		(it->second).push_back(make_pair(receivedIP, stoi(receivedPort)));
          	}
        }
        else if(hashedDetails.substr(0, hashedDetails.length() - 3) == nodeID.substr(0,nodeID.length() - 3)) {
        	where = ROUTING_TABLE;
        	it = node.routing_set.find(hashedDetails.substr(0, 1));
        	if(it == node.routing_set.end()) {	        		
        		node.routing_set[hashedDetails.substr(0, hashedDetails.length() - 3)].push_back(make_pair(receivedIP, stoi(receivedPort)));
        	}
        	else {
          		(it->second).push_back(make_pair(receivedIP, stoi(receivedPort)));
          	}
        }
        else {
        	where = NEIGHBOUR_SET;
        	node.neighbour_set.push_back(make_pair(receivedIP, stoi(receivedPort)));        	
        }
    }
}

// Thread handler function for join
void *handleJoinThread(void *recvDetails) {

    connectionInfo ci = *(struct connectionInfo *)recvDetails;

	string join_nodeID, join_IP, join_port;

    int pos = remaining.find_first_of('$');
	join_nodeID = remaining.substr(0, pos);
	remaining = remaining.substr(pos+delim1.length());
	pos = remaining.find_first_of('$');
	join_IP = remaining.substr(0, pos);
	remaining = remaining.substr(pos+delim1.length());
	pos = remaining.find_first_of('$');
	join_port = remaining.substr(0, pos);
	
    message = join_nodeID + delim1 + join_IP + delim1 + join_port + delim1;

    int where = NOTHING;
	int me=0;
    map<string,vector<pair<string,int> > >::iterator it;
    if(join_nodeID == nodeID) {
 		me=1;
    }
    else if(join_nodeID.substr(0, join_nodeID.length() - 1) == nodeID.substr(0,nodeID.length() - 1)) {
        //  connect and send
        where = LEAF_SET;
        node.leaf_set.push_back(make_pair(join_IP, stoi(join_port)));
    }
    else if(where != LEAF_SET){
        // searching for first 2 chars
        if(join_nodeID.substr(0, join_nodeID.length() - 2) == nodeID.substr(0,nodeID.length() - 2)) {
        	where = ROUTING_TABLE;
        	it = node.routing_set.find(join_nodeID.substr(0, 2));
        	if(it == node.routing_set.end()) {	        		
        		node.routing_set[join_nodeID.substr(0, join_nodeID.length() - 2)].push_back(make_pair(join_IP, stoi(join_port)));
        	}
        	else {
          		(it->second).push_back(make_pair(join_IP, stoi(join_port)));
          	}
        }
        else if(join_nodeID.substr(0, join_nodeID.length() - 3) == nodeID.substr(0,nodeID.length() - 3)) {
        	where = ROUTING_TABLE;
        	it = node.routing_set.find(join_nodeID.substr(0, 1));
        	if(it == node.routing_set.end()) {	        		
        		node.routing_set[join_nodeID.substr(0, join_nodeID.length() - 3)].push_back(make_pair(join_IP, stoi(join_port)));
        	}
        	else {
          		(it->second).push_back(make_pair(join_IP, stoi(join_port)));
          	}
        }
        else {
        	where = NEIGHBOUR_SET;
            uniqueNSet.clear();
            uniqueNSet.insert(make_pair(join_IP, stoi(join_port)));
            for(auto it = node.neighbour_set.begin(); it != node.neighbour_set.end(); it++) {
                uniqueNSet.insert({(*it).first, (*it).second});
            }

            node.neighbour_set.clear();

            for(auto it = uniqueNSet.begin(); it != uniqueNSet.end(); it++) {
                node.neighbour_set.push_back({(*it).first, (*it).second});
            }

            uniqueNSet.clear();

        }
    }

    if(!me) {
        if(where == LEAF_SET) {
        	for(auto i = node.leaf_set.begin(); i != node.leaf_set.end(); i++) {
                if(ci.IP != (*i).first && ci.port != (*i).second)
        		    connectToNetwork((*i).first, (*i).second, LEAF_SET);
        	} 
		}
		else if(where == ROUTING_TABLE) {
        	for(auto i = (it->second).begin(); i != (it->second).end(); i++) {
                if(ci.IP != (*i).first && ci.port != (*i).second)
        		    connectToNetwork((*i).first, (*i).second, ROUTING_TABLE);
        	}
		}
		else if(where == NEIGHBOUR_SET) {
            for(auto i = node.neighbour_set.begin(); i != node.neighbour_set.end(); i++) {
                if(ci.IP != (*i).first && ci.port != (*i).second)
        		    connectToNetwork((*i).first, (*i).second, NEIGHBOUR_SET);
        	}
		}
		connectToNetwork(join_IP, stoi(join_port), NOTHING);
	}		
	pthread_exit(0);
}

// Thread handler function for put
void *handlePutThread(void *) {
	int where = SELF;
	
	message = remaining;
	int pos = remaining.find_first_of("$");
	string key = remaining.substr(0, pos);
	string value = remaining.substr(pos + delim1.length());
	value = value.substr(0, value.length() - delim1.length());
	string key_Id = key.substr(0, 4);
	
	map<string,vector<pair<string,int> > >::iterator it;
	if(key_Id == nodeID) {
		where = SELF;
		dht.DHT.insert(make_pair(key, value));
	}
    else if(key_Id.substr(0, key_Id.length() - 1) == nodeID.substr(0,nodeID.length() - 1)) {
        where = LSP;      
    }
    else if(where != LSP) {
        if(key_Id.substr(0, key_Id.length() - 2) == nodeID.substr(0,nodeID.length() - 2)) {
        	it = node.routing_set.find(key_Id.substr(0, 2));
        	if(it != node.routing_set.end()) {
          		where = RTP;
          	}
        }
        else if(key_Id.substr(0, key_Id.length() - 3) == nodeID.substr(0,nodeID.length() - 3)) {
        	it = node.routing_set.find(key_Id.substr(0, 1));
        	if(it != node.routing_set.end()) {
          		where = RTP;
          	}
        }
        else {
        	where = NSP;        	
        }
    }
    if(where == LSP) {
    	for(auto i = node.leaf_set.begin(); i != node.leaf_set.end(); i++) {
            if((*i).first != recvDetails.IP && (*i).second != recvDetails.port)
    		    connectToNetwork((*i).first, (*i).second, LSP);
    	}
	}
	else if(where == RTP) {
    	for(auto i = (it->second).begin(); i != (it->second).end(); i++) {
            if((*i).first != recvDetails.IP && (*i).second != recvDetails.port)
    		    connectToNetwork((*i).first, (*i).second, RTP);
    	}
	}
	else if(where == NSP) {
		for(auto i = node.neighbour_set.begin(); i != node.neighbour_set.end(); i++) {
            if((*i).first != recvDetails.IP && (*i).second != recvDetails.port)
    		    connectToNetwork((*i).first, (*i).second, NSP);
    	}
	}
	pthread_exit(0); 
}

// Thread handler function for get
void *handleGetThread(void *) {

	int where = SELFG;
	
	message = remaining;
	int pos = remaining.find_first_of("$");
	string key = remaining.substr(0, pos);
	string value = remaining.substr(pos + delim1.length());
	value = value.substr(0, value.length() - delim1.length());
	string key_Id = key.substr(0, 4);

    remaining = remaining.substr(key.length() + delim1.length());
    int dollarPos = remaining.find_first_of('$');
    sendIP = remaining.substr(0, dollarPos);
    remaining = remaining.substr(dollarPos + delim1.length());
    dollarPos = remaining.find_first_of('$');
    sendPort = stoi(remaining.substr(0, dollarPos));

    map<string,vector<pair<string,int> > >::iterator it;
	if(key_Id == nodeID) {
		where = SELFG;
		message = "send" + delim1 + key + delim1 + dht.DHT[key];
		connectToNetwork(sendIP, sendPort, SEND);
	}
    else if(key_Id.substr(0, key_Id.length() - 1) == nodeID.substr(0,nodeID.length() - 1)) {
        where =  LSG;      
    }
    else if(where != LSG) {
        if(key_Id.substr(0, key_Id.length() - 2) == nodeID.substr(0,nodeID.length() - 2)) {
        	it = node.routing_set.find(key_Id.substr(0, 2));
        	if(it != node.routing_set.end()) {	        		
        		where = RTG;
          	}
        }
        else if(key_Id.substr(0, key_Id.length() - 3) == nodeID.substr(0,nodeID.length() - 3)) {
        	it = node.routing_set.find(key_Id.substr(0, 1));
        	if(it != node.routing_set.end()) {	        		
        		where = RTG;
          	}
        }
        else {
        	where = NSG;        	
        }
    }
    if(where == LSG) {
    	for(auto i = node.leaf_set.begin(); i != node.leaf_set.end(); i++) {
    		if((*i).first != recvDetails.IP && (*i).second != recvDetails.port)
                connectToNetwork((*i).first, (*i).second, LSG);
    	}
	}
	else if(where == RTG) {
    	for(auto i = (it->second).begin(); i != (it->second).end(); i++) {
    		if((*i).first != recvDetails.IP && (*i).second != recvDetails.port)
                connectToNetwork((*i).first, (*i).second, RTG);
    	}
	}
	else if(where == NSG) {
		for(auto i = node.neighbour_set.begin(); i != node.neighbour_set.end(); i++) {
    		if((*i).first != recvDetails.IP && (*i).second != recvDetails.port && !value_found)
                connectToNetwork((*i).first, (*i).second, NSG);
    	}
        value_found = 0;
	} 
	pthread_exit(0);
}

// Thread handler function
void *startServer(void *connection) {

	// cout << "\nServer started" << endl;
	connectionInfo con = *(struct connectionInfo *)connection;

	// Identifier of the socket getting created.
	int server_socket;

    struct sockaddr_in server_addr;
    socklen_t size;

    // Creating the socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);

    // Error checking for creation of socket
    if (server_socket < 0) {
        perror("\nError establishing socket...\n");
        exit(1);
    }

    // cout << "Server Socket creation Successful" << endl;

    // Initializing the server address data structure.
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(con.IP.c_str());
    server_addr.sin_port = htons(con.port);
    size = sizeof(server_addr);

    // Binding the port to socket
    if ((bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr))) < 0) {
        perror("Error Binding connection in Server\n");
        exit(1);
    }

    // Listening for nodes
    cout << "Listening for Nodes on port " << con.port << endl;
    listen(server_socket, 5);

	int returnVal;

	while(1) {    	
    	
    	// Accept the connection from other nodes 
	    returnVal = accept(server_socket,(struct sockaddr *)&server_addr, &size);

	    // Error checking for accept() call
	    if (returnVal < 0) 
	        perror("Error on accepting the connection");

	    int bufSize = 1024;
	    char buffer[bufSize];
	    string buf;

    	bzero(buffer, bufSize);	   
    	recv(returnVal, buffer, bufSize, 0);
    	
        buf = buffer;
    	string token;

    	int pos = buf.find_first_of(delim1);
    	token = buf.substr(0, pos);
    	remaining = buf.substr(pos+delim1.length());

        // Receive a Join Message
    	if(token == "join") {
            string conn = "connection";
            int posCon = remaining.find_first_of('c');
            string details = remaining.substr(0, posCon);

            remaining = remaining.substr(posCon + conn.length() + delim1.length());
            
            int pos1 = details.find_first_of('$');
            string receivedIP = details.substr(0, pos1);
            details = details.substr(pos1+delim1.length());
            pos1 = details.find_first_of('$');
            int receivedPort = stoi(details.substr(0, pos1));

            connectionInfo recvDetails;
            recvDetails.IP = receivedIP;
            recvDetails.port = receivedPort;

    		// get the remaining string from the buffer and pass it to handleJoin.
    		pthread_t newJoinThread;
    		int threadJoin;

    		threadJoin = pthread_create(&newJoinThread, NULL, handleJoinThread, (void *)&recvDetails);
    		if(threadJoin != 0) {
				perror("Thread Join Creation failed\n");
				exit(EXIT_FAILURE);
			}

			void* thread_result;
			threadJoin = pthread_join(newJoinThread, &thread_result);
			if (threadJoin != 0) {
				perror("Thread join failed");
				exit(EXIT_FAILURE);
			}
			cout << "Added to Pastry Network" << endl;
            close(returnVal);
    	}
    	else if(token == "put") {
            string conn = "connection";
            int posCon = remaining.find_first_of('c');
            string details = remaining.substr(0, posCon);

            remaining = remaining.substr(posCon + conn.length() + delim1.length());
            
            int pos1 = details.find_first_of('$');
            string receivedIP = details.substr(0, pos1);
            details = details.substr(pos1+delim1.length());
            pos1 = details.find_first_of('$');
            int receivedPort = stoi(details.substr(0, pos1));

            recvDetails.IP = receivedIP;
            recvDetails.port = receivedPort;


    		pthread_t newPutThread;
    		int threadPut;

    		threadPut = pthread_create(&newPutThread, NULL, handlePutThread, static_cast<void*>(&remaining));
    		if(threadPut != 0) {
				perror("Thread Put Creation failed\n");
				exit(EXIT_FAILURE);
			}

			void* thread_result_put;
			threadPut = pthread_join(newPutThread, &thread_result_put);
			if (threadPut != 0) {
				perror("Thread Put failed");
				exit(EXIT_FAILURE);
			}

			cout << "Finished handling Put" << endl;
            close(returnVal);
    	}
    	else if(token == "get") {
            string conn = "connection";
            int posCon = remaining.find_first_of('c');
            string details = remaining.substr(0, posCon);

            remaining = remaining.substr(posCon + conn.length() + delim1.length());
            
            int pos1 = details.find_first_of('$');
            string receivedIP = details.substr(0, pos1);
            details = details.substr(pos1+delim1.length());
            pos1 = details.find_first_of('$');
            int receivedPort = stoi(details.substr(0, pos1));

            recvDetails.IP = receivedIP;
            recvDetails.port = receivedPort;

    		pthread_t newGetThread;
    		int threadGet;

    		threadGet = pthread_create(&newGetThread, NULL, handleGetThread, static_cast<void*>(&remaining));
    		if(threadGet != 0) {
				perror("Thread Get Creation failed\n");
				exit(EXIT_FAILURE);
			}

			void* thread_result_get;
			threadGet = pthread_join(newGetThread, &thread_result_get);
			if (threadGet != 0) {
				perror("Thread get failed");
				exit(EXIT_FAILURE);
			}

			cout << "Finished handling get" << endl;
            close(returnVal);
    	}
        else if(token == "send") {
            int pos1 = remaining.find_first_of('$');
            string keyFound = remaining.substr(0, pos1);
            remaining = remaining.substr(pos1 + delim1.length());
            pos1 = remaining.find_first_of('$');
            string val = remaining.substr(0, pos1);
            cout << "---------Key-Value Pair Found in the Distributed Hash Table---------\n\n";
            cout << keyFound << " " << val << endl;
            value_found = 1;
            close(returnVal);
        }
    	else if(token == "nothing") {
			// accept the full routing table.
			// make temp vectors and map and insert them into node.
			
			string con = "connection";
			int posCon = remaining.find_first_of('c');
			string details = remaining.substr(0, posCon);
			
			int pos1 = details.find_first_of('$');
			string receivedIP = details.substr(0, pos1);
			details = details.substr(pos1+delim1.length());
			pos1 = details.find_first_of('$');
			string receivedPort = details.substr(0, pos1);
            
            string hashedDetails = md5(receivedIP + receivedPort);
           	hashedDetails = hashedDetails.substr(0, 4);

           	checkAndInsert(hashedDetails, receivedIP, receivedPort);

			remaining = remaining.substr(posCon + con.length() + delim1.length());
			
            int posLSend = remaining.find_first_of("LSend");
			remaining = remaining.substr(0, posLSend);
			if(remaining != "") {
			
    			vector<string> data = split(remaining);

    			for(unsigned int i = 0; i < data.size()-1; i+=2) {
    				node.leaf_set.push_back(make_pair(data[i], stoi(data[i+1])));
    			}
            }
				
			int newpos = buf.find_first_of("L");
			string remain = buf.substr(newpos+8);
			int posNSend = remain.find_first_of("N");
			remain = remain.substr(0, posNSend);

            if(remain != "") {
				vector<string> data2 = split(remain);
				for(unsigned int i = 0; i < data2.size()-1; i+=2) {
                    node.neighbour_set.push_back(make_pair(data2[i], stoi(data2[i+1])));
				}

                uniqueNSet.clear();

                for(auto it = node.neighbour_set.begin(); it != node.neighbour_set.end(); it++) {
                    uniqueNSet.insert({(*it).first, (*it).second});
                } 
                
                node.neighbour_set.clear();

                for(auto it = uniqueNSet.begin(); it != uniqueNSet.end(); it++) {
                    node.neighbour_set.push_back({(*it).first, (*it).second});
                }

                uniqueNSet.clear();
            }

			int newerpos = buf.find_first_of("N");
            string rtstring = buf.substr(newerpos+8);
			int Zpos = rtstring.find_first_of("Z");
            rtstring = rtstring.substr(0, Zpos);
			
			if(rtstring != "") {
				vector<string> data3 = split(rtstring);
				for(unsigned int i = 0; i < data3.size();) {
					int rowend_detect = find(data3.begin()+i, data3.end(), "RowEnd")-data3.begin();
					vector<pair<string, int>> temp;

					int j = i+1;
					for(; j < rowend_detect; j+=2) {
						temp.push_back(make_pair(data3[j], stoi(data3[j+1])));
					}

					node.routing_set.insert(make_pair(data3[i], temp));

					i = rowend_detect+1;
					if(data3[i] == "ZEnd")
						break;
				}
			}
            cout << "Added to Pastry Network" << endl;
            close(returnVal);
		}
        else if(token == "quit") {
            if(remaining != "") {            
                vector<string> data4 = split(remaining);

                for(unsigned int i = 0; i < data4.size()-1; i+=2) {
                    if(data4[i] != "QSend") {
                        string k = md5(data4[i+1]);
                        k = k.substr(0,2);
                        dht.DHT.insert({nodeID+k, data4[i+1]}); 
                    }
                }
            }
            close(returnVal);
        }
        else if(token == "death") {

            string con = "connection";
            int posCon = remaining.find_first_of('c');
            string details = remaining.substr(0, posCon);
            
            int pos1 = details.find_first_of('$');
            string deathIP = details.substr(0, pos1);
            details = details.substr(pos1+delim1.length());
            pos1 = details.find_first_of('$');
            string deathPort = details.substr(0, pos1);
            
            vector<pair<string, int> > temp;
            for(auto i = node.neighbour_set.begin(); i != node.neighbour_set.end(); i++) {
                if((*i).first == deathIP && (*i).second == stoi(deathPort)) {
                    // Do Nothing
                }
                else {                    
                    temp.push_back({(*i).first, (*i).second});
                }
            }
            
            node.neighbour_set.clear();

            for(auto i = temp.begin(); i != temp.end(); i++) {
               node.neighbour_set.push_back({(*i).first, (*i).second});
            }

            close(returnVal);
        }
        else if(token == "shutdown") {
            dht.DHT.clear();
            cout << "Closing the Pastry Network!" << endl;
            cout << "\nHave a nice day!! :)\n" << endl;
            close(returnVal);
            exit(0);
        }
	}
	pthread_exit(0);
}
