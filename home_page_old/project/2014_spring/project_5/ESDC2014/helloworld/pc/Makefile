CC=gcc
LDFLAG=`pkg-config --libs --cflags opencv`  -lstdc++
DEBUG_FLAG=-g
TARGETS=hello hello_videocapture hello_objectdetect videocapture object_detect 
TARGETS:=$(TARGETS) pedestrain_detect facerec_eigenfaces face_detect
TARGETS:=$(TARGETS) sub_image
TARGETS:=$(TARGETS) concat_img SkinDetector
TARGETS:=$(TARGETS) tracking
TARGETS:=$(TARGETS) videowriter

all: $(TARGETS)

hello: helloworld.cpp
	$(CC) -o hello helloworld.cpp $(LDFLAG)

hello_videocapture: helloworld_videocapture.cpp
	$(CC) -o hello_videocapture helloworld_videocapture.cpp  $(LDFLAG)

hello_objectdetect: hello_objectdetect.cpp
	$(CC) -o hello_objectdetect hello_objectdetect.cpp $(LDFLAG)

videocapture: videocapture.cpp
	$(CC) -o videocapture videocapture.cpp $(LDFLAG)

object_detect: object_detect.cpp
	$(CC) -o object_detect object_detect.cpp $(LDFLAG)

pedestrain_detect: pedestrain.cpp
	$(CC) -o pedestrain_detect pedestrain.cpp $(LDFLAG)

highgui: highgui.cpp
	$(CC) -o highgui highgui.cpp $(LDFLAG)

facerec_eigenfaces: facerec_eigenfaces.cpp
	$(CC) -o facerec_eigenfaces facerec_eigenfaces.cpp $(LDFLAG)

face_detect: face_detect.cpp
	$(CC) -o face_detect face_detect.cpp $(LDFLAG)
sub_image: sub_image.cpp
	$(CC) -o sub_image sub_image.cpp $(LDFLAG) $(DEBUG_FLAG)

concat_img: concat_img.cpp
	$(CC) -o concat_img concat_img.cpp $(LDFLAG) $(DEBUG_FLAG)

SkinDetector: SkinDetector.o test_SkinDetector.o
	$(CC) -o SkinDetector SkinDetector.o test_SkinDetector.o $(LDFLAG) $(DEBUG_FLAG)

test_SkinDetector.o: test_SkinDetector.cpp
	$(CC) -o test_SkinDetector.o -c test_SkinDetector.cpp $(DEBUG_FLAG)

SkinDetector.o: SkinDetector.h SkinDetector.cpp
	$(CC) -o SkinDetector.o -c SkinDetector.cpp $(DEBUG_FLAG)

Tracking: tracking.cpp
	$(CC) -o tracking tracking.cpp $(LDFLAG) $(DEBUG_FLAG)
videowriter: videowriter.cpp
	$(CC) -o videowriter videowriter.cpp $(LDFLAG) $(DEBUG_FLAG)
clean:
	rm *.o
	rm $(TARGETS)
