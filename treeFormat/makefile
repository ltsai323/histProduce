CC=g++
ROOTFLAG=`root-config --libs --cflags`

MYHEADERDIR=../..
MYLIBDIR=.

CFLAG=$(ROOTFLAG) -I$(MYHEADERDIR)

#   src=$(wildcard src/*.cc)  # src/part1.c src/part2.ccc
#   obj=$(patsubst %.cc, %.cc.o, $(src)) # src/part1.cc -> src/part1.cc.o
#   lib=$(notdir $(src)) # src/part.cc -> part.cc
#   lib=$(patsubst %.cc, lib/lib%.so, $(lib)) # part.cc ->lib/libpart.so
#   bin=$(wildcard bin/*.cc) # bin/main.cc



#usefulFuncs.o: interface/usefulFuncs.h src/usefulFuncs.cc
#	g++ -c src/usefulFuncs.cc -o obj/usefulFuncs.o -I$(MYLIB) $(gccROOTFLAG)
#
#a: main.o part.o
#	g++ -o a obj/main.o obj/part.o
#main.o: bin/test.cc
#	g++ -c bin/test.cc -o obj/main.o  -I$(MYLIB)
#
#b: main2.o part.o
#	g++ -o b obj/main2.o obj/part.o
#main2.o: bin/test2.cc
#	g++ -c bin/test2.cc -o obj/main2.o  -I$(MYLIB)
#
#part.o: src/part.cc
#	g++ -c src/part.cc -o obj/part.o -I$(MYLIB)
#tmpCC=src/formatTreeArray.cpp src/formatTreeVector.cpp
tmpCC=src/formatTreeArray.cpp
hihih=$(patsubst %.cc, %.cc.o, $(tmpCC))
tmpGL=$(nodir $(hihih))
#all:  $(tmpGL)
all:  array.o vector.o
	ls

$(tmpGL):$(tmpCC)
	$(CC) $(CFLAG) -c $< -o $@
array.o: src/formatTreeArray.cpp
	echo $(tmpGL)
	$(CC) $(CFLAG) -c $< -o $@
vector.o: src/formatTreeVector.cpp
	$(CC) $(CFLAG) -c $< -o $@



## compile all src file
#%.cc.o: %.cc
#	echo $(tmpGL)
#	$(CC) $(CFLAG) -c $< -o $@

 clean:
 	@mv -t ${HOME}/.trash/ `ls | grep .o` > /dev/null 2>&1 && echo 'CLEANING: .o files are moved to trash.' || echo 'CLEANING: .o file not found.'

# clean:
# 	@mv -t ${HOME}/.trash/ $(obj) > /dev/null 2>&1 && echo 'CLEANING: .o files are moved to trash.' || echo 'CLEANING: .o file not found.'
# cleanfig:
# 	@mv -t ${HOME}/.trash/ *.png *.pdf *.jpg > /dev/null 2>&1 && echo 'CLEANING: tmp figures are moved to trash.' || echo 'CLEANING: figure not found.'
# cleanall: clean cleanfig
# 	@rm ${HOME}/.trash -rf ; mkdir ${HOME}/.trash; echo 'CLEANING: Trashes are eliminated from your PC.'

# help:
# 	@echo 'object compile command': g++ -c src/parts.cc -o src/parts.cc.o'
# .PHONY: clean all help clean
