# makefile for proc_fan

CC = gcc
CFLAGS = -o

all: proc_fan runsim testsim

proc_fan: proc_fan.c
	$(CC) $(CFLAGS) proc_fan proc_fan.c

runsim: runsim.c ./makeargv.c
	$(CC) $(CFLAGS) runsim runsim.c ./makeargv.c

testsim: testsim.c 
	$(CC) $(CFLAGS) testsim testsim.c

clean:
	rm -rf *.o proc_fan runsim testsim

