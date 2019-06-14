#
# qa-hid - makefile
#

APP:=qa-hid
HIDMAIN:=hidmain
HIDLIB:=hidlib
LIBS:=/usr/lib/arm-linux-gnueabihf/libdocopt.a

CFLAGS := -std=c++11
LDFLAGS :=

all: $(APP)

$(APP): $(HIDMAIN).o $(HIDLIB).o
	$(CXX) $(LDFLAGS) $(HIDMAIN).o $(HIDLIB).o -o $(APP) $(LIBS)

$(HIDMAIN).o: $(HIDMAIN).cpp
	$(CXX) $(CFLAGS) -I.. -c $(HIDMAIN).cpp

$(HIDLIB).o: $(HIDLIB).cpp
	$(CXX) $(CFLAGS) -c $(HIDLIB).cpp
