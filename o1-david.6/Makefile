CC = gcc
CFLAGS = -g

OSSO = oss.o
OSS = oss 

USERO = user.o
USER = user


.SUFFIXES: .c .o

all: $(OSS) $(USER)

$(OSS): $(OSSO)
	$(CC) -o $@ $(OSSO)

$(USER): $(USERO)
	$(CC) -o $@ $(USERO)

.c.o:
	$(CC) -c -o $@ $<

clean:
	rm *.o $(USER) $(OSS) *.log
