

CC = gcc
CFLAGS = -o
OPTIONS =

.SUFFIXES: .c .o

all: oss user

oss: oss.c
	$(CC) $(CFLAGS) oss oss.c utility.h $(OPTIONS)

user: user.c
	$(CC) $(CFLAGS) user user.c utility.h $(OPTIONS)

.c.o:
	$(CC) $(CFLAGS) $(CPPFLAGS)  $<

clean:
	rm -rf *.o *.log  oss user $(OPTIONS)



