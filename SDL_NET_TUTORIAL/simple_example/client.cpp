/*
	http://www.youtube.com/watch?v=LNSqqxIKX_k
*/

#include <iostream>
#include "SDL.h"
#include "SDL_net.h"
#include <string.h>

//g++ client.cpp `sdl-config --cflags --libs` -lSDL_image -lSDL_mixer -lSDL_ttf -lSDL_net -o client

int main(int argc, char **argv)
{
	SDL_Init(SDL_INIT_EVERYTHING);
	SDLNet_Init();

	IPaddress ip;
	SDLNet_ResolveHost(&ip, "127.0.0.1", 1234); // ip of server, NULL if server ip otherwise, port #

	TCPsocket client = SDLNet_TCP_Open(&ip); // make this a dynamic array later

	char text[100];

	// send request to server to get information

	SDLNet_TCP_Recv(client, text, 100);
	std::cout << text << std::endl;

	SDLNet_TCP_Close(client); // important! if you don't close you cannot connect to the same port again
	
	SDLNet_Quit();
	SDL_Quit();

	return 0;
}