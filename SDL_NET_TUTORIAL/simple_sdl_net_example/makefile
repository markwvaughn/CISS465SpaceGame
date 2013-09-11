all:	server.cpp
	g++ server.cpp `sdl-config --cflags --libs` -lSDL_image -lSDL_mixer -lSDL_ttf -lSDL_net -o server.exe
	g++ client.cpp `sdl-config --cflags --libs` -lSDL_image -lSDL_mixer -lSDL_ttf -lSDL_net -o client.exe

e:	edit
edit:	server.cpp client.cpp 
	xemacs server.cpp &
	xemacs client.cpp &

s:	server
server:	server.exe
	./server.exe

c:	client
client:	client.exe
	./client.exe

w:	wipe
wipe:	server.exe client.exe 
	rm server.exe client.exe *~
