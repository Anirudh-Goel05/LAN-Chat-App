// Pass the ip address in command line
// to send a message 'to_send_id message'
// to exit type 'quit' 
#include<iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <string.h>
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 
#include <unistd.h>
#include <pthread.h>
#include<csignal> 
using namespace std;
#define PORT "3000"

int sockfd;

void* start_chat(void* args){
	while(true){
		char message[1024];
		bzero(message, 1024);
		cout <<"$$$";
		fgets(message,1024,stdin);
		int bytes_sent = send(sockfd ,(void *)message, 1024,0); 
	}
}


int main(int argv,char* args[]){
	
	struct addrinfo hints;
	struct addrinfo* servinfo,*p;
	
	memset(&hints,0,sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	// hints.ai_flags = AI_PASSIVE; 
	//hints.ai_protocol = 0; //find number for IPV4
	
	int status = getaddrinfo(args[1],PORT, &hints, &servinfo);
	if(status != 0){
		std::cout<<"Couldn't load the server\n";
		exit(1);
	}
	
	for(p = servinfo; p != NULL; p = p->ai_next) {
	    if ((sockfd = socket(p->ai_family, p->ai_socktype,
	            p->ai_protocol)) == -1) {
	        perror("socket");
	        continue;
	    }

	    if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
	        perror("connect");
	        close(sockfd);
	        continue;
	    }

	    break; // if we get here, we must have connected successfully
	}

	if (p == NULL) {
		fprintf(stderr, "client: failed to connect\n");
		return 2;
	}
	
	cout <<"Connected to server\n";
	freeaddrinfo(servinfo);

	char message[1024];
	pthread_t thread_id; 
	pthread_create(&thread_id, NULL,start_chat, NULL); 
		
	while(1){
	   bzero(message,1024);
	   int bytes_read=read(sockfd, message, sizeof(message));
	   string rec = message;
	   if(bytes_read > 0){
	   	  if(rec == "Checking if online ......")
	   	  	continue;	
	   	  cout<<"Received message: "<<message << endl;
	   }
	}
		
	freeaddrinfo(servinfo);
}
