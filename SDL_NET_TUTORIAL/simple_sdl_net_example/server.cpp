/**
 * This is a very simple example with SDL_net. There is no error checking atm.
 * Run client.exe or telnet 127.0.0.1 1234 (local client) to test
 **/


#include <iostream>
#include <cstring>
#include "SDL.h"
#include "SDL_net.h"


typedef char Buffer[1024];


int main(int argc, char **argv)
{
	SDL_Init(SDL_INIT_EVERYTHING);
	SDLNet_Init();

    // Allocate memory for the server's ip
	IPaddress ip;

    // Resolve the ip for the server (i.e, localhost since we're the server).
    // NULL is used here because we want to be the server.
    // Essentially, this creates a listener on port 1234
	SDLNet_ResolveHost(&ip, NULL, 1234);

	// Create Server Socket (TCP) that listens for incoming connections
	TCPsocket server = SDLNet_TCP_Open(&ip);

    // This is the socket that will accept connections and transfer data
	TCPsocket client;

    // This is the buffer that we will send to the clients
	Buffer to_client = "HELLO CLIENT!";

    // This is the buffer that we will receive data into from the clients
    Buffer from_client;

    // Keep listening for incoming connections till you receive a message
	while(1)
	{
        // Attempt to accept incoming connection from client
		client = SDLNet_TCP_Accept(server);

		if (client)
		{
			// the connection has been established and we can communicate

            // Send message to client
            SDLNet_TCP_Send(client, to_client, strlen(to_client) + 1);

            // Receive message from client
            SDLNet_TCP_Recv(client, from_client, 100);
            std::cout << from_client << std::endl;

            // Close the socket
            SDLNet_TCP_Close(client);

            // Quit
			break;
		}
	}

    // Close the socket. Must close to free up the port before reboot.
	SDLNet_TCP_Close(server); 

    // Quit SDL and SDLNet
	SDLNet_Quit();
	SDL_Quit();

	return 0;
}
