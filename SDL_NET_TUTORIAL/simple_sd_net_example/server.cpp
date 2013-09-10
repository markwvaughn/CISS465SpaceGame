/*
	http://www.youtube.com/watch?v=LNSqqxIKX_k
*/
	
#include <iostream>
#include "SDL.h"
#include "SDL_net.h"
#include <string.h>

//g++ server.cpp `sdl-config --cflags --libs` -lSDL_image -lSDL_mixer -lSDL_ttf -lSDL_net -o server
// telnet 127.0.0.1 1234 (local client)

int main(int argc, char **argv)
{
	SDL_Init(SDL_INIT_EVERYTHING);
	SDLNet_Init();

	IPaddress ip;
	SDLNet_ResolveHost(&ip, NULL, 1234); // ip of server, NULL if server ip otherwise, port #

	// Create Server Socket (TCP)
	TCPsocket server = SDLNet_TCP_Open(&ip);
	TCPsocket client; // make this a dynamic array later

	const char * text = "HELLO CLIENT!";
	while(1)
	{
		client = SDLNet_TCP_Accept(server);
		if (client)
		{
			// here you can communicate with the client
			SDLNet_TCP_Send(client, text, strlen(text) + 1); // text is a pointer
			SDLNet_TCP_Close(client);
			break;
		}
	}

	SDLNet_TCP_Close(server); // important! if you don't close you cannot connect to the same port again
	
	SDLNet_Quit();
	SDL_Quit();

	return 0;
}