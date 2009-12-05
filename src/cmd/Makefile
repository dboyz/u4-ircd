# Temporaty makefile for compiling test modules --commx
CC = g++
CXXFLAGS = -std=c++0x -Wall -export-dynamic -fPIC -I../../include
LIBS =
SOURCES = $(wildcard *.cpp)
OBJECTS = $(SOURCES:.cpp=.o)

all: $(OBJECTS)
	@echo "[`date +%H:%M:%S`] *** Building commands completed."

%.o: %.cpp
	@echo "[`date +%H:%M:%S`] *** Building command: $*"
	@$(CC) $(CXXFLAGS) -c $< -o $*.o
	@$(CC) -shared $(CXXFLAGS) -o ../../lib/cmd/$*.so $*.o

clean:
	@echo "[`date +%H:%M:%S`] *** Cleaning up."
	@rm -f $(OBJECTS) ../../lib/cmd/*.so
