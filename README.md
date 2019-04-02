LAN CHATTING APP

This is a linux-based lan chat app using socket programming in C/C++.
This was developed as an assignment of course CEC13-Computer Networks

Requirements to run: g++ compiler and lib-pthread

Steps to run:

1.First compile the server by typing 'g++ server2.cpp -lpthread -o server' in the terminal in the same directory as the file(Optional you can directly go to step 3)

2. Similarly compile the client(Optional you can directly go to step 3) 

3. Start the server by ./server

4.How to run a client:
	
	1. Execute by './client {ip address of the server in your lan'}
	
	2. Create multiple clients by the above command
	
	3. Each client is provided a unique id
	
	4. Upon connecting with the server the client is provided with a list of currently online clients in the network. 
	
	5. Send a message by {client_id} {your_message}
	
	6. Quit the app by sending a 'quit' message
