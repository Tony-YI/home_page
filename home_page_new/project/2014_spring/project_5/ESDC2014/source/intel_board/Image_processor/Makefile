#$(CC),$(MV) are inherited from the parent Makefile
all: Image_processor.o camshiftdemo.o fourPointAlgo.o 

Image_processor.o: Image_processor.cpp Image_processor.h Makefile
	$(CC) -o Image_processor.o -c Image_processor.cpp $(CPPFLAGS)
	echo "Image_processor.o: moving to parent directory intel_board"
	$(MV) Image_processor.o ../

camshiftdemo.o: camshiftdemo.cpp
	$(CC) -o  camshiftdemo.o -c camshiftdemo.cpp $(CPPFLAGS)
	$(MV) camshiftdemo.o ../

fourPointAlgo.o: fourPointAlgo.cpp
	$(CC) -o fourPointAlgo.o -c fourPointAlgo.cpp $(CPPFLAGS)
	$(MV) fourPointAlgo.o ../

clean:
	rm *.o

hello: fourPointAlgo.cpp camshiftdemo.cpp smain.cpp
	g++ `pkg-config --cflags opencv` -o hello fourPointAlgo.cpp camshiftdemo.cpp smain.cpp `pkg-config --libs opencv`
