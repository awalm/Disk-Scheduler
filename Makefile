# makefile for my final project (disk scheduling program)
# usage: 
# "make" or "make disk_sched" to make a compiled file with my program named
# "main".
# "make clean" to clean the compiled files.

CC=gcc
CFLAGS=-Wall

disk_sched: main.c
	$(CC) $(CFLAGS) -o main main.c

clean:
	rm -rf *.o
	rm -rf main

