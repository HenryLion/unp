all : client server
.PHONY : all
client : client.c
	gcc -o $@ $^ -lunp
server : server.c
	gcc -o $@ $^ -lunp

clean:
	rm client server
