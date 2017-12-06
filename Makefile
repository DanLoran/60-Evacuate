evac.out : evac.o EvacRunner.o #LinkedList.o
	g++ -ansi -Wall -g -o evac.out evac.o EvacRunner.o

evac.o : evac.cpp evac.h LinkedList.h
	g++ -ansi -Wall -g -c evac.cpp

#LinkedList.o : LinkedList.h LinkedList.cpp
#	g++ -ansi -Wall -g -c LinkedList.h

EvacRunner.o : EvacRunner.cpp CPUTimer.h EvacRunner.h evac.h
	g++ -ansi -Wall -g -c EvacRunner.cpp

clean :
	rm -f evac.out evac.o  EvacRunner.o #LinkedList.o
