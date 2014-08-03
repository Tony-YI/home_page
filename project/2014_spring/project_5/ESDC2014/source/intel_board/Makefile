INTEL_BOARD_LD = -l Image_processor/Image_processor.o
INTEL_BOARD_DEPEND = intel_board.cpp intel_board.h Image_processor.o camera.o Motion_controller.o message.o ui.o Makefile 
INTEL_BOARD_DEPEND_OBJECT = Image_processor.o camera.o Motion_controller.o
INTEL_BOARD_DEPEND_OBJECT+= message.o ui.o camshiftdemo.o Controller_Com.o

export RM=rm
export CP=cp
export MV=mv
export CPPFLAGS = -lstdc++ -lm
all: intel_board.o

intel_board.o: $(INTEL_BOARD_DEPEND)
	echo "intel_board.o: Compiling"
	$(CC) -o intel_board.o -c intel_board.cpp $(CPPFLAGS)
	echo "intel_board.o: removing dependent object code"
	
Image_processor.o: Image_processor/Image_processor.cpp Image_processor/Image_processor.h Image_processor/Makefile
	echo "Image_processor.o: Compiling"
	cd Image_processor;make all
	
camera.o: camera/camera.cpp camera/camera.h camera/Makefile
	echo "camera.o: Compiling"
	cd camera;make all

Motion_controller.o: Motion_controller/Motion_controller.cpp Motion_controller/Motion_controller.h Motion_controller/Makefile
	echo "Motion_controller.o: Compiling"
	cd Motion_controller;make all
message.o: lib/message.h lib/message.cpp lib/Makefile
	echo "message.o: Compiling"
	cd lib;make all
ui.o: User_interface/ui.h User_interface/ui.cpp User_interface/Makefile
	echo "ui.o: Compiling"
	cd User_interface;make all
clean:
	rm *.o
