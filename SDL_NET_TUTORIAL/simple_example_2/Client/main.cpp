#include <iostream>
#include <cmath>
#include <vector>
#include <string>
#include <fstream>
#include <ctime>
#include <cmath>
#include <cstdlib>
#include <sstream>

#include "Includes.h"
#include "Constants.h"
#include "compgeom.h"
#include "Surface.h"
#include "Event.h"
#include "SDL_net.h";

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
	TCPsocket server; // make this a dynamic array later

	char * data;
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

		KeyPressed keypressed = get_keypressed();
		if (keypressed[LEFTARROW])
		{
			data = "1";
		}
		else if (keypressed[UPARROW])
		{
			data = "2";
		}
		else if (keypressed[RIGHTARROW])
		{
			data = "3";
		}
		else if (keypressed[DOWNARROW])
		{
			data = "4";
		}

		//-----------------------------------------------------------------------------------------------------------
		// SEND DATA TO SERVER
		//-----------------------------------------------------------------------------------------------------------
		while(1)
		{
			server = SDLNet_TCP_Accept(client);
			if (server)
			{
				// here you can communicate with the server
				SDLNet_TCP_Send(server, data, strlen(data) + 1); // text is a pointer
				SDLNet_TCP_Close(server);
				break;
			}
		}

		SDLNet_TCP_Close(client); // important! if you don't close you cannot connect to the same port again

		//-----------------------------------------------------------------------------------------------------------
		// Get Data from Silver
		//-----------------------------------------------------------------------------------------------------------
		data = "";
		SDLNet_TCP_Recv(server, data, 100);

		std::cout << "Input " << count << ": " << data << std::endl;

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