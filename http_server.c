#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <regex.h>
#include "http.h"

#define BUFFER_SIZE 1024
#define BACKLOG 5

void debug_regmatch(regmatch_t match) {
	printf("start of match: %lld\n", match.rm_so);
	printf("end of match  : %lld\n", match.rm_eo);
}


int main() {	
  int addr_status, parse_status, sockfd, bind_status, new_fd, msg_len, bytes_sent, bytes_recvd;
	struct addrinfo hints, *res, *current;
	struct sockaddr_storage their_addr;
	char incoming_msg[BUFFER_SIZE];
	socklen_t addr_size;
	

	// get local addr
  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;

  if ((addr_status = getaddrinfo(NULL, "3490", &hints, &res)) != 0) {
      fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(addr_status));
      exit(1);
  }
	// check for valid results?
	for(current = res;current != NULL;current = current->ai_next) {

		// get socket
		if ((sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) == -1) {
			perror("socket error");
	    continue;
		};
		
		// bind to socket
		if ((bind_status = bind(sockfd, res->ai_addr, res->ai_addrlen)) == -1) {
			perror("bind error");
			close(sockfd);
	 		continue;
		}

		break;
	}

	free(res);

	
	
	while(1) {
		// listen
		if(listen(sockfd, BACKLOG) == -1) {
			perror("listen error");
			exit(1);
		} 

		// accept
		addr_size = sizeof their_addr;
		if((new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &addr_size)) == -1) {
			perror("accept error");
	    exit(1);
		}
		//set socket option for keepalive

		//receive
		do {

			if((bytes_recvd = recv(new_fd, incoming_msg, BUFFER_SIZE - 1, 0)) == -1) {
				perror("receive error");
			}
			incoming_msg[bytes_recvd] = '\0';


			if(bytes_recvd == 0) {
				printf("client closed connection");
				break;
			}
			
			HTTPRequest *request = malloc(sizeof(HTTPRequest));

			parse_status = parse_http_request(incoming_msg, request);

			char response[BUFFER_SIZE];
			size_t response_len;
			response_len = construct_response(request->uri, response, BUFFER_SIZE);

			if((bytes_sent = send(new_fd, response, response_len, 0)) == -1) {
				perror("send error");
		    exit(1);
			}

			if(bytes_sent == response_len) {
				close(new_fd);
				break;
			}

		} while(1);

	}
	close(sockfd);
}