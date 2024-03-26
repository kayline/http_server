#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>

int main() {	
  int addr_status, sockfd, bind_status, new_fd, msg_len, bytes_sent;
	struct addrinfo hints, *res;
	struct sockaddr_storage their_addr;
	char *msg = "Hi I'm a datetime	";
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
	
	// get socket
	if ((sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) !=0) {
		fprintf(stderr, "socket error: %s\n", gai_strerror(sockfd));
    exit(1);
	};
	
	// bind to socket
	if ((bind_status = bind(sockfd, res->ai_addr, res->ai_addrlen)) !=0) {
		fprintf(stderr, "bind error: %s\n", gai_strerror(bind_status));
    exit(1);
	}

	// listen
	listen(sockfd, 5); //error handling?

	// accept
	addr_size = sizeof their_addr;
	if((new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &addr_size)) !=0) {
		fprintf(stderr, "accept error: %s\n", gai_strerror(new_fd));
    exit(1);
	}

	msg_len = strlen(msg);
	bytes_sent = send(sockfd, msg, msg_len, 0);

	if((bytes_sent = send(sockfd, msg, msg_len, 0)) != 0) {
		fprintf(stderr, "send error: %s\n", gai_strerror(bytes_sent));
    exit(1);
	}

	if(bytes_sent != msg_len) {
		fprintf(stderr, "failed to send complete message");
	}
}