
ESDC_LDOBJECT = intel_board/intel_board.o intel_board/Image_processor.o intel_board/camera.o intel_board/Motion_controller.o intel_board/Controller_Com.o intel_board/message.o intel_board/ui.o intel_board/camshiftdemo.o intel_board/fourPointAlgo.o
ESDC_LDLIB = -lopencv_highgui -lopencv_imgproc -lopencv_core -lopencv_objdetect

SUBDIRS = intel_board
bin_PROGRAMS = ESDC
ESDC_SOURCES = ESDC.cpp
ESDC_LDADD = $(ESDC_LDOBJECT) $(ESDC_LDLIB)

AM_CXXFLAGS = @ESDC_CFLAGS@
AM_LDFLAGS = @ESDC_LIBS@

CLEANFILES = *~
