/****************************************************************************
 Yihsiang Liow
 Copyright
 ****************************************************************************/
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

void server_loop()
{
	Surface surface(640, 480);
	Event event;

	SDL_Init(SDL_INIT_EVERYTHING);
	SDLNet_Init();

	IPaddress ip;
	SDLNet_ResolveHost(&ip, NULL, 1234); // ip of server, NULL if server ip otherwise, port #

	// Create Server Socket (TCP)
	TCPsocket server = SDLNet_TCP_Open(&ip);
	TCPsocket client; // make this a dynamic array later

	while (1)
	{
		if (event.poll() && event.type() == QUIT) break;

		//-----------------------------------------------------------------------------------------------------------
		// RECEIVE DATA FROM CLIENT
		//-----------------------------------------------------------------------------------------------------------

		char data[100];
		SDLNet_TCP_Recv(server, data, 100);

		//-----------------------------------------------------------------------------------------------------------
		// DO GAME LOGIC
		//-----------------------------------------------------------------------------------------------------------

		char * text = "";
		if (data[0] == '1') {
			text = "You Pressed the Left Arrow Key!";
		}
		else if (data[0] == '2') {
			text = "You Pressed the Up Arrow Key!";
		}
		else if (data[0] == '3') {
			text = "You Pressed the Right Arrow Key!";
		}
		else if (data[0] == '4') {
			text = "You Pressed the Down Arrow Key!";
		}
		else {
			text = "You did not press an arrow key.";
		}

		//-----------------------------------------------------------------------------------------------------------
		// SEND DATA TO CLIENT
		//-----------------------------------------------------------------------------------------------------------

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

		delay(10);
	}

	SDLNet_Quit();
	SDL_Quit();
}


int main(int argc, char* argv[])
{
    server_loop();

	return 0;	
}