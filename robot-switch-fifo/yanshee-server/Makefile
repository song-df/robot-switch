

LDFLAGS=-export-dynamic -lpthread -L./ -llog -ltaskList
CFLAGS+=-std=gnu1x -O3 -I ../liblog -I ../libtaskList

CC=gcc

OBJECT=${patsubst %.c, %.o, ${wildcard *.c}}

all:yanshee_server

yanshee_server:${OBJECT} 
	$(CC) $^ -o $@ $(CFLAGS) $(LDFLAGS)
	rm -f *.o


.PHONY:clean
clean:
	rm -f *.o yanshee_server
