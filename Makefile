CC = gcc
CFLAGS = -g

all: ep1sh ep1 ep12

ep1sh: ep1sh.c
	$(CC) ep1sh.c -o ep1sh $(CFLAGS) -lreadline  
ep1: ep1_giovana.c
	$(CC) ep1_giovana.c -o ep1 $(CFLAGS) -lpthread
ep12: ep1_cond.c
	$(CC) ep1_cond.c -o ep12 $(CFLAGS) -lpthread

