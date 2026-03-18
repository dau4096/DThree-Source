CC = g++
CFLAGS = -std=c++23 -O2 -ffast-math
LDFLAGS = -I/usr/include \
         -I/usr/include/GL \
         -I/usr/include/glm \
         -I/usr/local/include

LIBS = -lglfw -lGLEW -lGL -lpugixml -lm -ldl -pthread -ldpp

SOURCES = main.cpp #Plus any extra C++ files.
OBJECTS = $(SOURCES:.cpp=.o)

all: prgm

prgm: $(OBJECTS)
	$(CC) $(OBJECTS) $(LIBS) -o prgm

%.o: %.cpp
	$(CC) $(CFLAGS) $(LDFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) prgm


