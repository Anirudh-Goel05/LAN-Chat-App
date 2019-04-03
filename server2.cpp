// To compile g++ sever.cpp -pthread
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include<iostream>
#include<bits/stdc++.h>
#include <pthread.h> 
using namespace std;
#define PORT "3000"
#define MAX_LISTEN 10
 
class msg{
public:
	int sid,rid;
	string message;
};

map <int,int> port_num;
static int client_number = 0;

void sigchld_handler(int s)
{
	int saved_errno = errno;
	while(waitpid(-1, NULL, WNOHANG) > 0);
	errno = saved_errno;
}


void* add_send_req(void* args){
	while(true)
	{
		for(auto x:port_num){
			

			string str = "Checking if online ......";
			char char_str[1024];
			bzero(char_str,1024);
			strcpy(char_str,str.c_str());
			
			char rec_msg[1024]; bzero(rec_msg,1024);
			int bytes_read = read(x.second,rec_msg,sizeof(rec_msg));

			if(bytes_read<=0){
				cout<<"No current message from " << x.first << endl;
				continue;
			}

			string rec_str = rec_msg;			
			istringstream ss(rec_str); 
  
    	    string word; 
		    ss >> word; 

		    if(word == "quit"){
				cout<<"This socket has quit";
				string str = std::to_string(x.first) + " has quit the application\n\n";
				for(auto y:port_num){
					if(y == x)
						continue;

					strcpy(rec_msg,str.c_str());
					int bytes_sent = send(y.second,(void*)rec_msg,1024,0);
				}
				// TO DELETE FROM MAP;
				auto it=port_num.find(x.first);
  				port_num.erase(it);    
				continue;
			}


		  	int to_send_id = atoi(word.c_str());


		  	// cout<<"Message \"" << rec_msg<< "\" to send from " << x.first << " " << to_send_id<<endl;
			if(to_send_id == 0){
				for(auto y:port_num){
					if(y == x)
						continue;
					send(y.second,(void*)rec_msg,1024,0);
				}
				continue;
			}

		    int to_send_socketid = port_num[to_send_id];
			int bytes_sent = send(to_send_socketid,(void*)rec_msg,1024,0);

			rec_str = "Sent by: " + std::to_string(x.first);
			strcpy(rec_msg,rec_str.c_str());
			bytes_sent = send(to_send_socketid,(void*)rec_msg,1024,0);
			// cout<<"Message sent from" << x.first << " " << to_send_id<<endl;
				
		}
	}	
}

int main(){
	
	struct addrinfo hints;
	struct addrinfo* servinfo,*p;

	memset(&hints,0,sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE; 	
	int sockfd;
	int yes=1;
	struct sigaction sa;
	socklen_t sin_size;

	
	int status = getaddrinfo(NULL,PORT, &hints, &servinfo);
	if(status != 0){
		std::cout<<"Couldn't load the server\n";
		exit(1);
	}
	
	//Adding timeout to read instruction
	
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
			p->ai_protocol)) == -1) {
			perror("server: socket");
			continue;
		}
		if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,sizeof(int)) == -1){
			perror("setsockopt");
			exit(1);
		}
		if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockfd);
			perror("server: bind");
			continue;
		}
		break;
	}
	freeaddrinfo(servinfo);
	if (p == NULL) {
		fprintf(stderr, "server: failed to bind\n");
		exit(1);
	}
	
	int l_listen = listen(sockfd,MAX_LISTEN);
	if(l_listen == -1){
		cout<<"Listen failed\n";
		exit(1);
	}
	/* Killing any unused ports*/
	bool first_time = true;
	while(true){
		sa.sa_handler = sigchld_handler; // reap all dead processes
		sigemptyset(&sa.sa_mask);
		sa.sa_flags = SA_RESTART;
		if (sigaction(SIGCHLD, &sa, NULL) == -1) {
			perror("sigaction");
			exit(1);
		}
			
		struct sockaddr_storage client;
		socklen_t client_size = sizeof (client);

			// cout<<"Waiting for a new connection...\n";
			int new_s = accept(sockfd, (struct sockaddr *)&client, &client_size);
			if(new_s < 0){
				cout<<"Failed to accept at server";
				exit(1);
			}

			struct timeval tv;
			tv.tv_sec = 1;
			tv.tv_usec = 0;
			setsockopt(new_s,SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));	

			struct sockaddr_in peer;
			socklen_t peer_size = sizeof(peer);

			int peer_status =  getpeername(new_s, (struct sockaddr*)&peer, &peer_size);
			if(peer_status == -1){

			}
			char peer_ip4[INET_ADDRSTRLEN]; 
			inet_ntop(AF_INET, &(peer.sin_addr), peer_ip4, INET_ADDRSTRLEN);
			// cout<<peer_ip4 <<"is connected\n";

			client_number ++;

			string str="Current active users are: ";
			for(auto y:port_num){
				str += std::to_string(y.first)+ " ";
			}

			char char_str[1024];
			bzero(char_str,1024);
			strcpy(char_str,str.c_str());
			int bytes_sent = send(new_s,(void*)char_str,1024,0);
			port_num.insert({client_number,new_s});

			str = std::to_string(client_number) + " is online\n";
				for(auto y:port_num){
					if(y.first == client_number)
						continue;


					strcpy(char_str,str.c_str());
					int bytes_sent = send(y.second,(void*)char_str,1024,0);
			}
				

			
			if(client_number == 1){
				pthread_t thread_id; 
			    pthread_create(&thread_id, NULL, add_send_req, NULL); 
			    // pthread_join(thread_id, NULL); 	
			}
	}
}
