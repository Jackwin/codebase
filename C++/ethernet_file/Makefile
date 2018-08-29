CC=g++
CFLAGS=-c -Wall
LDFLAGS=
SOURCES=mld_server.cpp
#SOURCES=mld_rx.cpp reader.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=mld_server
#EXECUTABLE=mld_rx

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf $(EXECUTABLE) $(OBJECTS)
