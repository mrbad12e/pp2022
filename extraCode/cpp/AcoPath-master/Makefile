all: build

build:
	g++ -c adaptivesystem.cpp
	g++ -c antsystem.cpp
	g++ -c main.cpp
	g++ -o out main.o antsystem.o adaptivesystem.o
	./out
c:
	rm *.o
	rm out
