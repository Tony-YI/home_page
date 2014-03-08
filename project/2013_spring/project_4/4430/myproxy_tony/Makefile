CC = gcc
CFLAGS = -lpthread
all: myproxy

myproxy:myproxy.c
	gcc -o $@ $< $(CFLAGS)

clean:
	rm myproxy
