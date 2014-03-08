CC=gcc
DEBUG=-g
FLAGS=-O3 -std=c99 -Wall $(DEBUG)

all : getaddrinfo pthread

getaddrinfo : getaddrinfo.c
	$(CC) $(FLAGS) -o $@ $^

pthread : pthread.c
	$(CC) $(FLAGS) -lpthread -o $@ $^

clean :
	rm -f getaddrinfo pthread
