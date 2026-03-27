CC = g++
CFLAGS = -std=c++23 -Wall -Wextra -static-libstdc++ -static-libgcc
LDFLAGS = -I/usr/include -I/usr/local/include

LIBS = -lpugixml -lm -ldl -pthread -ldpp -static-libstdc++ -static-libgcc

SOURCES = main.cpp src/font.cpp
OBJECTS = $(SOURCES:.cpp=.o)

all: prgm

debug: $(OBJECTS)
	$(CC) $(OBJECTS) $(LIBS) -g -DVERBOSE -DDEBUG_WORDS -o prgm

prgm: $(OBJECTS)
	$(CC) $(OBJECTS) $(LIBS) -O2 -ffast-math -o prgm

%.o: %.cpp
	$(CC) $(CFLAGS) $(LDFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) prgm


