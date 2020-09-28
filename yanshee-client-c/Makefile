

LDFLAGS=-export-dynamic -lpthread -L./libs -lrobot
CFLAGS+=-std=gnu1x -O3 -I ./include

CC=gcc

OBJECT=${patsubst %.c, %.o, ${wildcard *.c}}

all:yanshee_client

yanshee_client:${OBJECT} 
	$(CC) $^ -o $@ $(CFLAGS) $(LDFLAGS)
	rm -f *.o


.PHONY:clean
clean:
	rm -f *.o yanshee_client
