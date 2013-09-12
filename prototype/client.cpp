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

    // Resolve the ip for the server (i.e., localhost on the same machine)
    // Server's ip is used here because we want to be the client.
    // Essentially, this creates a sender on port 1234
	SDLNet_ResolveHost(&ip, "127.0.0.1", 1234);

	// Create Server Socket (TCP) to connect to server
	TCPsocket server = SDLNet_TCP_Open(&ip);

    // This is the buffer that we will send to the server
	Buffer to_server;

    // Get User's message to send.
    std::cin.getline(to_server, 1024);

    // This is the buffer that we will receive data into from the server
    Buffer from_server;
    
    if (server)
    {
        // the connection has been established and we can communicate
        
        // Send message to client
        SDLNet_TCP_Send(server, to_server, strlen(to_server) + 1);
        
        // Receive message from client
        SDLNet_TCP_Recv(server, from_server, 100);
        std::cout << from_server << std::endl;
        
        // Close the socket
        SDLNet_TCP_Close(server);
    }
    
    // Quit SDL and SDLNet
	SDLNet_Quit();
	SDL_Quit();

	return 0;
}
