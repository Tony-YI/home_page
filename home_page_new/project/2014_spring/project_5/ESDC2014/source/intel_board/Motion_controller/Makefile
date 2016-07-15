all: Motion_controller.o Controller_Com.o PIDcontroller.o

Motion_controller.o: Motion_controller.cpp Makefile Motion_controller.h
	$(CC) -o Motion_controller.o -c Motion_controller.cpp $(CPPFLAGS)
	$(MV) Motion_controller.o ../
Controller_Com.o: Controller_Com.h Controller_Com.cpp Makefile
	$(CC) -o Controller_Com.o -c Controller_Com.cpp $(CPPFLAGS)
	$(MV) Controller_Com.o ../
PIDcontroller.o: PIDcontroller.cpp PIDcontroller.h Makefile
	$(CC) -o PIDcontroller.o -c PIDcontroller.cpp $(CPPFLAGS)
	$(MV) PIDcontroller.o ../
clean:
	rm *.o
