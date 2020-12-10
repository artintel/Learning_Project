#include <pthread.h>
#include <time.h>
#include <string.h>
#include <fcntl.h>
#include <sys/select.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>



#define nullptr NULL
#define BUFFER_SIZE 4096
#define HTTP_VERSION "HTTP/1.1"
#define CONNECTION_TYPE "Connection: close\r\n"

// API -- gethostbyname()
char* host_to_ip(const char* hostname){

	struct hostent* host_entry = gethostbyname(hostname);

	// 14.215.177.39 --> unsigned int

	// inet_ntoa() unsigned int --> char*
	if(host_entry){
		return inet_ntoa(*(struct in_addr*)*host_entry->h_addr_list);
	}
	return nullptr;
}


int http_create_socket(char* ip){

	int sockfd = socket(AF_INET, SOCK_STREAM, 0);


	struct sockaddr_in sin = { 0 };
	sin.sin_family = AF_INET;
	sin.sin_port = htons(80);
	sin.sin_addr.s_addr = inet_addr(ip);


	if(0 != connect(sockfd, (struct sockaddr*)&sin, sizeof(struct sockaddr_in))){
		return -1;
	}

	fcntl(sockfd, F_SETFL, O_NONBLOCK);

	return sockfd;

}

// hostname: github.com
// resource: /artintel
char* http_send_request(const char* hostname, const char* resource){

	char* ip = host_to_ip(hostname);
	int sockfd = http_create_socket(ip);

	char buffer[BUFFER_SIZE] = { 0 };
	sprintf(buffer, 
"GET %s %s\r\n\
Host: %s\r\n\
%s\r\n\
\r\n",

	resource, HTTP_VERSION,
	hostname,
	CONNECTION_TYPE
	);

	send(sockfd, buffer, strlen(buffer), 0);

	//recv()
	//select()
	fd_set fdread;
	FD_ZERO(&fdread);
	FD_SET(sockfd, &fdread);

	struct timeval tv;
	tv.tv_sec = 5;
	tv.tv_usec = 0;


	char* result = malloc(sizeof(int));
	memset(result, 0, sizeof(int));

	while(1){
		int selection = select(sockfd + 1, &fdread, nullptr, nullptr, &tv);
		if(!selection || !FD_ISSET(sockfd, &fdread)){
			break;
		}
		else {
			memset(buffer, 0, BUFFER_SIZE);
			int len = recv(sockfd, buffer, BUFFER_SIZE, 0);
			if(len == 0){ //disconnect
				break;
			}

			result = realloc(result, (strlen(result) + len+ 1) * sizeof(char));
			strncat(result, buffer, len);
		}

	}

	return result;
	

}




int main(int argc, char* argv[]){

	if( argc < 3 ) return -1;

	char* response = http_send_request(argv[1], argv[2]);

	printf("response: %s\n", response);

	free(response);
	
}































