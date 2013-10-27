all:	server client

e:	edit
edit:	server.cpp client.cpp 
	xemacs server.cpp &
	xemacs client.cpp &

s:	server
server:	server.cpp
	g++ server.cpp -g -O2 src/*.cpp src/*.c `sdl-config --cflags --libs` -lSDL_image -lSDL_mixer -lSDL_ttf -lSDL_net -Iincludes -o server.exe

c:	client
client:	client.cpp
	g++ client.cpp -g -O2 src/*.cpp src/*.c `sdl-config --cflags --libs` -lSDL_image -lSDL_mixer -lSDL_ttf -lSDL_net -Iincludes -o client.exe

w:	wipe
wipe:	server.exe client.exe
	rm server.exe client.exe *~;~
