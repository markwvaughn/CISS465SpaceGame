all: client.cpp server.cpp
	g++ server.cpp -g -O2 -Wall `sdl-config --cflags --libs` -lSDL_image -lSDL_mixer -lSDL_ttf -lSDL_net -o server
	g++ client.cpp -g -O2 -Wall `sdl-config --cflags --libs` -lSDL_image -lSDL_mixer -lSDL_ttf -lSDL_net -o client

clean:
	rm server client

new: clean all

client.o: util.h
server.o: util.h
