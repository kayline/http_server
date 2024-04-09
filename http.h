#ifndef HTTP_H
#define HTTP_H
typedef struct _HTTPRequest {
	char *method;
	char *uri;
} HTTPRequest;
int construct_response(char *file_name, char *response, int max_response_len);
int parse_http_request(char *incoming_msg, HTTPRequest *request);

#endif