CC = cc
LIBS = -lsocket -lnsl -lrt

all: server client

server: server.c
	$(CC) server.c -o server $(LIBS)

client: client.c
	$(CC) client.c -o client $(LIBS)

clean:
	rm -f server client task32.sock
