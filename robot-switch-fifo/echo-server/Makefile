
LDFLAGS=-export-dynamic -lpthread -L./ -llog -ltaskList
CFLAGS+=-O3 -std=gnu1x -I ../liblog -I ../libtaskList 

CC=gcc

OBJECT=${patsubst %.c, %.o, ${wildcard *.c}}

all:echo-server

echo-server:${OBJECT} 
	$(CC) $^ -o $@ $(CFLAGS) $(LDFLAGS)
	rm -f *.o


.PHONY:clean
clean:
	rm -f *.o echo-server
