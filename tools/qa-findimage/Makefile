#
# qa-findimage Makefile
#
# A tool to locate images on the RaspberryPI desktop.
#
# call "make NORPI=1" to build on non-Raspberry PI systems
#

DEFS="-DRPI"

RPI_INCS=-I/opt/vc/include -I/opt/vc/include/interface/vcos/pthreads -I/opt/vc/include/interface/vmcs_host/linux
RPI_LIBS=-L/opt/vc/lib -lbcm_host

OPENCV_LIBS=-lopencv_highgui -lopencv_core -lopencv_imgproc

all: qa-findimage

qa-findimage: qa-findimage.cpp rpi-screenshot.cpp rpi-screenshot.h

# allow to build on non RPI systems
ifdef NORPI
	$(eval DEFS = "-URPI")
	$(eval RPI_INCS = "")
	$(eval RPI_LIBS = "")
endif

	gcc $(DEFS) qa-findimage.cpp rpi-screenshot.cpp -Wall -o qa-findimage \
	$(RPI_INCS) $(RPI_LIBS) -lstdc++ $(OPENCV_LIBS)
