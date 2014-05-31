all: build

build: lib/CMPS10/CMPS10.h lib/CMPS10/CMPS10.cpp
	ino build

clean:
	ino clean

install: build
	ino upload

lib/CMPS10/CMPS10.h lib/CMPS10/CMPS10.cpp:
	git submodule init
	git submodule update
