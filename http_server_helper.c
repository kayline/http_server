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

int construct_response(char *file_name, char *response, int max_response_len) {
	printf("Constructing response with file name: %s\n", file_name);
	size_t headers_len;
	headers_len = strlcpy(response, "HTTP/1.0 200 OK\r\nContent-Type: text/html\r\n\r\n", max_response_len);

	int file_fd; 
	if((file_fd = open(file_name, O_RDONLY)) == -1) {
		headers_len = strlcpy(response, "HTTP/1.0 404 Not Found\r\n\r\n", max_response_len);
		return headers_len;
	};

	ssize_t bytes_read;
	if((bytes_read = read(file_fd, response + headers_len, max_response_len - headers_len)) == -1) {
		perror("error reading file");
		exit(1);
	}
	close(file_fd);

	return(strlen(response));
}

int parse_http_request(char *incoming_msg, HTTPRequest *request) {
	regex_t request_regex;
	regmatch_t request_matches[3];
	int match_results;

	regcomp(&request_regex, "^(GET|POST|HEAD) /([^ ]*) HTTP/1", REG_EXTENDED);

	if((match_results = regexec(&request_regex, incoming_msg, 3, request_matches, 0)) == 0) {
		incoming_msg[request_matches[1].rm_eo] = '\0';
		request->method = incoming_msg + request_matches[1].rm_so;

		incoming_msg[request_matches[2].rm_eo] = '\0';
		request->uri = incoming_msg + request_matches[2].rm_so;
		return(0);
	} else {
		printf("match failed: %d\n", match_results);
		return(-1);
	}
}