CC = g++
CFLAGS = -std=c++23 -O2 -ffast-math -Wall -Wextra -static-libstdc++ -static-libgcc -DVERBOSE -DDEBUG_WORDS
LDFLAGS = -I/usr/include -I/usr/local/include

LIBS = -lpugixml -lm -ldl -pthread -ldpp -static-libstdc++ -static-libgcc

SOURCES = main.cpp src/font.cpp
OBJECTS = $(SOURCES:.cpp=.o)

all: prgm

prgm: $(OBJECTS)
	$(CC) $(OBJECTS) $(LIBS) -o prgm

%.o: %.cpp
	$(CC) $(CFLAGS) $(LDFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) prgm


