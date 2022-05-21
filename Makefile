# https://cs.colby.edu/maxwell/courses/tutorials/maketutor/
CC=g++

#-I. = look to local directory for files
CFLAGS=-I.

#DEPS sets dependencies not listed in the files to compile
DEPS = unorderedmap.h dawg.h

## WORKING VERSION ##
wordlSolver: main.cpp unorderedmap.cpp dawg.cpp
	$(CC) -std=c++17 -o wordls main.cpp unorderedmap.cpp dawg.cpp $(CFLAGS)

# NON-WORKING CLEAN STATEMENT
#clean : -rm *.o $(objects) wordlSolver

# NON-WORKING VERSION THAT ATTEMPTS TO BUILD .o FILES AND THEN COMPILE THOSE
#%.o: %.c $(DEPS)
#	$(CC) -std=c++17 -c -o $@ $< $(CFLAGS)
#
#wordlSolver: main.o unorderedmap.o
#	$(CC) -std=c++17 -o wordlSolver main.o unorderedmap.o $(CFLAGS)
