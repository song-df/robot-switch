
LDFLAGS=-export-dynamic -lpthread -L./ -llog -ltulingApi -ltaskList -lcurl
CFLAGS+=-std=gnu1x -O3 -I ../liblog -I ../libtulingApi -I ../libtaskList 

CC=gcc

OBJECT=${patsubst %.c, %.o, ${wildcard *.c}}

all:tuling_server

tuling_server:${OBJECT} 
	$(CC) $^ -o $@ $(CFLAGS) $(LDFLAGS)
	rm -f *.o


.PHONY:clean
clean:
	rm -f *.o tuling_server
