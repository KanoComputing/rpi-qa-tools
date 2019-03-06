#
#  Makefile
#
#  Generic Makefile to build all software tools.
#

all: qa-findimage qa-hid

qa-hid:
	cd tools/qa-hid && make

qa-findimage:
	cd tools/qa-findimage && make
