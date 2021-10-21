CC=g++
CFLAGS=-g -Wall -I. -fopenmp

output: main.o executor.o
	$(CC) $(CFLAGS) main.o executor.o -o pvs.exe

main.o: main.cpp
	$(CC) $(CFLAGS) -c main.cpp

executor.o: executor.cpp executor.h
	$(CC) $(CFLAGS) -c executor.cpp

clean:
	rm -f *.o pvs.exe
