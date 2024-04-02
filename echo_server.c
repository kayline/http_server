#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>

#define BUFFER_SIZE 100

int main() {	
  int addr_status, sockfd, bind_status, new_fd, msg_len, bytes_sent, bytes_recvd;
	struct addrinfo hints, *res, *current;
	struct sockaddr_storage their_addr;
	char *msg = "Hi I'm a datetime";
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
		if(listen(sockfd, 5) == -1) {
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

			if((bytes_recvd = recv(new_fd, incoming_msg, BUFFER_SIZE, 0)) == -1) {
				perror("receive error");
		    exit(1);
			}
			printf("got some bytes: %d\n", bytes_recvd);

			if((bytes_sent = send(new_fd, incoming_msg, bytes_recvd, 0)) == -1) {
				perror("send error");
		    exit(1);
			}
		} while(1);

		close(new_fd);
	}
}