
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <pthread.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <unistd.h>




#define nullptr NULL
#define BUFFER_LENGTH 1024
#define EPOLL_SIZE	1024

#define MAX_PORT 100


void* client_routine(void* arg){

	int clientfd = *(int*)arg;

	while(1){
		char buffer[BUFFER_LENGTH] = { 0 };
		int len = recv(clientfd, buffer, BUFFER_LENGTH, 0);
		if ( len < 0 ){
			close(clientfd);
			break;
		}
		else if( len == 0 ){ // client disconnect
			close(clientfd);
			break;
		}
		else {
			printf("Recv : %s, %d byte(s)\n", buffer, len);
		}
		
	}

}


int islistenfd(int fd, int* fds){

	int i = 0;
	for(; i < MAX_PORT; i++){
		if(fd == *(fds + i)) return fd;
	}
	return 0;

}




// ./tcp_server 8888

int main(int argc, char* argv[]){

	if(argc < 2) {
		printf("Param Error\n");
		return -1;
	}
	int port = atoi(argv[1]);// start
	int sockfds[MAX_PORT] = { 0 };
	int epfd = epoll_create(1);

	int i = 0;
	for(i = 0; i <MAX_PORT; i++){
		int sockfd = socket(AF_INET, SOCK_STREAM, 0);

		struct sockaddr_in addr;
		memset(&addr, 0, sizeof(struct sockaddr_in));
		addr.sin_family = AF_INET;
		addr.sin_port = htons(port + i);
		addr.sin_addr.s_addr = INADDR_ANY;

		if(bind(sockfd, (struct sockaddr*)&addr, sizeof(struct sockaddr_in)) < 0){
			perror("bind");
			return 2;
		}
		if(listen(sockfd, 5) < 0){
			perror("listen");
			return 3;
		}
		
		struct epoll_event ev;
		ev.events = EPOLLIN | EPOLLET;
		ev.data.fd = sockfd;
		epoll_ctl(epfd, EPOLL_CTL_ADD, sockfd, &ev);

		sockfds[i] = sockfd;
	}

#if 0

	while(1) {

		struct sockaddr_in client_addr;
		memset(&client_addr, 0, sizeof(struct sockaddr_in));
		socklen_t client_len = sizeof(client_addr);
		
		int clientfd = accept(sockfd, (struct sockaddr*)&client_addr, &client_len);

		pthread_t threadid;
		pthread_create(&threadid, nullptr, client_routine, &clientfd);

	}

#endif


	struct epoll_event events[EPOLL_SIZE] = { 0 };

	while(1){
		
		int nready = epoll_wait(epfd, events, EPOLL_SIZE, 5);

		if(nready < 0){
			continue;
		}

		int i = 0;
		for(; i < nready; i++){

			int sockfd = islistenfd(events[i].data.fd, sockfds);
			if (sockfd != 0){ //listen
				
				struct sockaddr_in client_addr;
				memset(&client_addr, 0, sizeof(struct sockaddr_in));
				socklen_t client_len = sizeof(client_addr);
				
				int clientfd = accept(sockfd, (struct sockaddr*)&client_addr, &client_len);

				fcntl(clientfd, F_SETFL, O_NONBLOCK);

				int reuse = 1;
				setsockopt(clientfd, SOL_SOCKET, SO_REUSEADDR, (char *)&reuse, sizeof(reuse));

				
				struct epoll_event ev;
				ev.events = EPOLLIN | EPOLLET;
				ev.data.fd = clientfd;
				epoll_ctl(epfd, EPOLL_CTL_ADD, clientfd, &ev);
				
			}
			else {

				int clientfd = events[i].data.fd;
				char buffer[BUFFER_LENGTH] = { 0 };
				int len = recv(clientfd, buffer, BUFFER_LENGTH, 0);
				if ( len < 0 ){
				
					close(clientfd);

					struct epoll_event ev;
					ev.events =EPOLLIN;
					ev.data.fd = clientfd;
					epoll_ctl(epfd, EPOLL_CTL_DEL, clientfd, &ev);
					
				}
				else if( len == 0 ){ // client disconnect
				
					close(clientfd);

					struct epoll_event ev;
					ev.events =EPOLLIN;
					ev.data.fd = clientfd;
					epoll_ctl(epfd, EPOLL_CTL_DEL, clientfd, &ev);

				}
				else {
					printf("Recv : %s, %d byte(s), clientfd: %d\n", buffer, len, clientfd);
				}
				
			}

		}

	}



	return 0;

}











