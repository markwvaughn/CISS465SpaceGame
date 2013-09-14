/**
 * This is a very simple example with SDL_net. There is no error checking atm.
 * Run client.exe or telnet 127.0.0.1 1234 (local client) to test
 **/


#include <iostream>
#include <cmath>
#include <vector>
#include <string>
#include <fstream>
#include <ctime>
#include <cmath>
#include <cstdlib>
#include <sstream>
#include <string> 
#include "Includes.h"
#include "Constants.h"
#include "compgeom.h"
#include "Surface.h"
#include "Event.h"
#include "SDL_net.h"

void send_data(std::string data, TCPsocket & client) {
	SDLNet_TCP_Send(client, data.c_str(), data.length());
}

std::string receive_data(TCPsocket & client) {
	char buffer[100];
	std::string data;
	SDLNet_TCP_Recv(client, buffer, 100);

	char buffer2[100];
	int i = 0;
	while (buffer[i] != '#') {
		buffer2[i] = buffer[i];
		i++;
	}

	std::cout << buffer2 << std::endl;

	data.assign(buffer2, strlen(buffer2) + 1);

	return data;
}

int main(int argc, char **argv)
{
	SDL_Init(SDL_INIT_EVERYTHING);
	SDLNet_Init();

	IPaddress ip;
	SDLNet_ResolveHost(&ip, NULL, 1235);
	TCPsocket server = SDLNet_TCP_Open(&ip);
	TCPsocket client;
	std::string to_client;
    std::string from_client;

	Rect player(100,100,20,20);
	player.y = 100;
	player.x = 100;

	while(1)
	{
		client = SDLNet_TCP_Accept(server); // change to list

		// loop over clients
		if (client)
		{

			//--------------------------------------------------------------------------------------
			// RECV DATA FROM CLIENT
			//--------------------------------------------------------------------------------------
            from_client = receive_data(client);

			if (from_client == "1") {
				player.y--;
			}
			else if (from_client == "2") {
				player.y++;
			}
			else if (from_client == "3") {
				player.x--;
			}
			else if (from_client == "4") {
				player.x++;
			}

			std::cout << "from client: " << from_client << std::endl;

			std::string pos_x;
			std::stringstream x_stream;
			x_stream << player.x;
			pos_x = x_stream.str();

			std::string pos_y;
			std::stringstream y_stream;
			y_stream << player.y;
			pos_y = y_stream.str();

			std::string pos_packet = "p";
			pos_packet += pos_x;
			pos_packet += ",";
			pos_packet += pos_y;
			pos_packet += ";#";

			//collect up all packets
			to_client += pos_packet;

			std::cout << "to_client: " << to_client << std::endl;
            //--------------------------------------------------------------------------------------
			// SEND DATA TO CLIENT
			//--------------------------------------------------------------------------------------
           	send_data(to_client, client);
            break;

            SDLNet_TCP_Close(client);
		}
	}

	SDLNet_TCP_Close(server); 
	SDLNet_Quit();
	SDL_Quit();

	return 0;
}
