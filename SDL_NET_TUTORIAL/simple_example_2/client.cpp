/*
	http://www.youtube.com/watch?v=LNSqqxIKX_k
*/

#include <iostream>
#include "SDL.h"
#include "SDL_net.h"
#include <string.h>

//g++ client.cpp `sdl-config --cflags --libs` -lSDL_image -lSDL_mixer -lSDL_ttf -lSDL_net -o client

const int W = 400;
const int H = 400;

void game_loop()
{
	Surface surface(W, H);
	Event event;
	
	SDL_Init(SDL_INIT_EVERYTHING);
	
	//-----------------------------------------------------------------------------------------
	// NETWORKING
	//-----------------------------------------------------------------------------------------
	SDLNet_Init();

	IPaddress ip;
	SDLNet_ResolveHost(&ip, "127.0.0.1", 1234); // ip of server, NULL if server ip otherwise, port #

	TCPsocket client = SDLNet_TCP_Open(&ip); // make this a dynamic array later

	char data[100];

	// send request to server to get information

	SDLNet_TCP_Recv(client, data, 100);
	std::cout << data << std::endl;

	int count = 0;

	//-----------------------------------------------------------------------------------------
	// GAME LOOP
	//-----------------------------------------------------------------------------------------
	while (1)
	{
		if (event.poll() && event.type() == QUIT) break;

		//-----------------------------------------------------------------------------------------------------------
		// Input
		//-----------------------------------------------------------------------------------------------------------

		


		char data[100];
		SDLNet_TCP_Recv(server, data, 100);

		//-----------------------------------------------------------------------------------------------------------
		// Get Data from Silver
		//-----------------------------------------------------------------------------------------------------------


		std::cout << "Input " << count << ": ";

		delay(10);
	}


	SDLNet_TCP_Close(client); // important! if you don't close you cannot connect to the same port again
	
	SDLNet_Quit();
	SDL_Quit();
	
	return;
}

int main(int argc, char **argv)
{
	game_loop();

	return 0;
}