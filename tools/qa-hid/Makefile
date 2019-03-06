#
# qa-hid - makefile
#

APP:=qa-hid
HIDMAIN:=hidmain
HIDLIB:=hidlib
LIBS:=/usr/lib/arm-linux-gnueabihf/libdocopt.a

all: $(APP)

$(APP): $(HIDMAIN).o $(HIDLIB).o
	gcc $(HIDMAIN).o $(HIDLIB).o -o $(APP) -lstdc++ $(LIBS)

$(HIDMAIN).o: $(HIDMAIN).cpp
	g++ -c $(HIDMAIN).cpp

$(HIDLIB).o: $(HIDLIB).cpp
	g++ -c $(HIDLIB).cpp
