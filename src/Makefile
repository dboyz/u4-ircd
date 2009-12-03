TARGET   = ../bin/ircd
CC       = g++
CXXFLAGS = -std=c++0x -pipe -O3 -Wall -I../include
LIBS     =
SOURCES  = $(wildcard *.cpp)
OBJECTS  = $(SOURCES:.cpp=.o)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(TARGET) $(LIBS)

%.o: %.cpp
	$(CC) $(CXXFLAGS) -c $< -o $*.o

clean:
	rm -f $(TARGET) $(OBJECTS)
