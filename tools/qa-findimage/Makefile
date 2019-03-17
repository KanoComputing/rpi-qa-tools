#
# qa-findimage Makefile
#
# A tool to locate images on the RaspberryPI desktop.
#
# call "make NORPI=1" to build on non-Raspberry PI systems
#

DEFS =


RPI_INCS=-I/opt/vc/include -I/opt/vc/include/interface/vcos/pthreads -I/opt/vc/include/interface/vmcs_host/linux
RPI_LIBS=-L/opt/vc/lib -lbcm_host

OPENCV_LIBS=-lopencv_highgui -lopencv_core -lopencv_imgproc

CFLAGS := -std=c++11 -Wall $(OPENCV_LIBS)
LDFLAGS :=


all: qa-findimage

qa-findimage: qa-findimage.cpp rpi-screenshot.cpp rpi-screenshot.h

# allow to build on non RPI systems
ifndef NORPI
	$(eval DEFS += "-DRPI")
	$(eval CFLAGS += $(RPI_INCS))
	$(eval LDFLAGS += $(RPI_LIBS))
endif

	$(CXX) -I.. $(CFLAGS) $(DEFS) $(LDFLAGS) \
		qa-findimage.cpp rpi-screenshot.cpp \
		-o qa-findimage
