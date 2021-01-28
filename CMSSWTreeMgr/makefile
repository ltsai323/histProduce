all: usefulFuncs.o

MYLIB = .
gccROOTFLAG=`root-config --libs --cflags`


usefulFuncs.o: interface/usefulFuncs.h src/usefulFuncs.cc
	g++ -c src/usefulFuncs.cc -o obj/usefulFuncs.o -I$(MYLIB) $(gccROOTFLAG)

a: main.o part.o
	g++ -o a obj/main.o obj/part.o
main.o: bin/test.cc
	g++ -c bin/test.cc -o obj/main.o  -I$(MYLIB)

b: main2.o part.o
	g++ -o b obj/main2.o obj/part.o
main2.o: bin/test2.cc
	g++ -c bin/test2.cc -o obj/main2.o  -I$(MYLIB)

part.o: src/part.cc
	g++ -c src/part.cc -o obj/part.o -I$(MYLIB)

clean:
	/bin/rm -f a b obj/*
