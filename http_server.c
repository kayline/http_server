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

#define BUFFER_SIZE 1024

void debug_regmatch(regmatch_t match) {
	printf("start of match: %lld\n", match.rm_so);
	printf("end of match  : %lld\n", match.rm_eo);
}

int main() {	
  int addr_status, sockfd, bind_status, new_fd, msg_len, bytes_sent, bytes_recvd;
	struct addrinfo hints, *res, *current;
	struct sockaddr_storage their_addr;
	char incoming_msg[BUFFER_SIZE];
	socklen_t addr_size;
	regex_t get_regex;
	regmatch_t get_matches[3];

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
			}

			if(bytes_recvd == 0) {
				printf("client closed connection");
				break;
			}
			// parse incoming_msg for HTTP method
			regcomp(&get_regex, "^(GET|POST|HEAD) /([^ ]*) HTTP/1", REG_EXTENDED);


			printf("%s\n", incoming_msg);

			int match_results;
			const char *url_encoded_file_name;

			if((match_results = regexec(&get_regex, incoming_msg, 3, get_matches, 0)) == 0) {
				incoming_msg[get_matches[1].rm_eo] = '\0';
				const char *method = incoming_msg + get_matches[1].rm_so;
				printf("method: %s\n", method);

				incoming_msg[get_matches[2].rm_eo] = '\0';
				url_encoded_file_name = incoming_msg + get_matches[2].rm_so;
				printf("file name: %s\n", url_encoded_file_name);
			} else {
				printf("match failed: %d\n", match_results);
			}

			size_t headers_len;
			char response[BUFFER_SIZE];
			strlcpy(response, "HTTP/1.0 200 OK\r\nContent-Type: text/html\r\n\r\n", BUFFER_SIZE);
			headers_len = strlen(response);

			int file_fd; 
			if((file_fd = open(url_encoded_file_name, O_RDONLY)) == -1) {
				perror("file open error");
				exit(1);
			};

			ssize_t bytes_read;
			if((bytes_read = read(file_fd, response + headers_len, BUFFER_SIZE - headers_len)) == -1) {
				perror("error reading file");
				exit(1);
			}
			close(file_fd);

			size_t response_len;
			response_len = strlen(response);

			if((bytes_sent = send(new_fd, response, response_len, 0)) == -1) {
				perror("send error");
		    exit(1);
			}

		} while(1);
		close(new_fd);

	}
	close(sockfd);
}