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


test: test-qa-hid test-qa-findimage

test-qa-hid:
	# FIXME: temporarily disabled due to sudo requirements
	#cd test && python -m pytest -s qa-hid

test-qa-findimage:
	cd test && python -m pytest qa-findimage
