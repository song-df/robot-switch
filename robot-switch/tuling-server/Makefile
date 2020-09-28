
LDFLAGS=-export-dynamic -lpthread -L./ -llog -L./ -ltulingApi -lcurl
CFLAGS+=-O3 -std=gnu1x -I ../liblog -I ../libtulingApi

CC=gcc

OBJECT=${patsubst %.c, %.o, ${wildcard *.c}}

all:tuling-server

tuling-server:${OBJECT} 
	$(CC) $^ -o $@ $(CFLAGS) $(LDFLAGS)
	rm -f *.o

.PHONY:clean
clean:
	rm -f *.o tuling-server
