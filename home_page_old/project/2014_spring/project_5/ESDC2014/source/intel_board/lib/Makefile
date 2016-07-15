all: message.o

message.o: message.cpp message.h Makefile
	$(CC) -o message.o -c message.cpp $(CPPFLAGS)
	$(MV) message.o ../

clean:
	rm message.o
