
LDFLAGS=-export-dynamic -lpthread -L./ -llog
CFLAGS+=-O3 -std=gnu1x -I ../liblog 

CC=gcc

OBJECT=${patsubst %.c, %.o, ${wildcard *.c}}

all:server

server:${OBJECT} 
	$(CC) $^ -o $@ $(CFLAGS) $(LDFLAGS)
	rm -f *.o


.PHONY:clean
clean:
	rm -f *.o server
