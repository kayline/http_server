CFLAGS := -g -O0

all: datetime_server echo_server http_server

datetime_server:
	gcc -o datetime_server datetime_server.c

echo_server:
	gcc -o echo_server echo_server.c

http_server:
	gcc $(CFLAGS) -c http_server_helper.c -o http_server_helper.o
	gcc $(CFLAGS) -c http_server.c -o http_server.o
	gcc $(CFLAGS) http_server.o http_server_helper.o -o http_server

clean:
	rm -f datetime_server echo_server http_server