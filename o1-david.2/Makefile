

CC = gcc
CFLAGS = -o
OPTIONS = 

all: master producer consumer

master: main.c
	$(CC) $(CFLAGS) master main.c utility.h $(OPTIONS)

producer: producer.c 
	$(CC) $(CFLAGS) producer producer.c utility.h $(OPTIONS)

consumer: consumer.c
	$(CC) $(CFLAGS) consumer consumer.c utility.h $(OPTIONS)

clean:
	rm -rf *.o *.log  master consumer producer prod $(OPTIONS)


