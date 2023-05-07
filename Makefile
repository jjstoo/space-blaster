LIBS = -lSDL2 -lSDL2_ttf -lSDL2_net
FLAGS = -Wall -std=c++2a
DEBUGFLAGS = -Wall -std=c++2a -DDEBUG=1 -g
SERVERFLAGS = -Wall -std=c++2a -g
SERVERLIBS = -lSDL2 -lSDL2_net

OUTNAME = main.out
SERVERNAME = server.out

SRC = \
src/*.cpp \
src/game/*.cpp \
src/game/weapons/*.cpp \
src/networking/*.cpp \
src/messaging/*.cpp \
src/resources/*.cpp \
src/physics/*.cpp \
src/rendering/*.cpp

SERVER_SRC = \
src/server/*.cpp \
src/networking/*.cpp \
src/messaging/*.cpp


all: clean
	g++ $(FLAGS) $(SRC) $(LIBS) -o build/$(OUTNAME)

debug: clean
	g++ $(DEBUGFLAGS) $(SRC) $(LIBS) -o build/$(OUTNAME)

server:
	g++ $(SERVERFLAGS) $(SERVER_SRC) $(SERVERLIBS) -o build/$(SERVERNAME)

clean:
	rm -f build/$(OUTNAME)
