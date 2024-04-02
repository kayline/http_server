all: datetime_server echo_server http_server

datetime_server:
	gcc -o datetime_server datetime_server.c

echo_server:
	gcc -o echo_server echo_server.c

http_server:
	gcc -o http_server http_server.c

clean:
	rm -f datetime_server echo_server http_server