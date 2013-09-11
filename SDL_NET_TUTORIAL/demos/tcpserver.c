#include <stdlib.h>
#ifndef _MSC_VER
#include <unistd.h>
#endif
#include "SDL.h"
#include "SDL_net.h"

int main(int argc, char **argv)
{
	SDL_Init(SDL_INIT_EVERYTHING);
	SDLNet_Init();

	TCPsocket server,client;
	char message[1024];
	int len;
	
	IPaddress ip;
	SDLNet_ResolveHost(&ip, NULL, 1235); // ip of server, NULL if server ip otherwise, port #

	/* open the server socket */
	server=SDLNet_TCP_Open(&ip);
	if(!server)
	{
		printf("SDLNet_TCP_Open: %s\n",SDLNet_GetError());
		exit(4);
	}

	while(1)
	{

		//-------------------------------------------------------------------------------------------------------------
		// RECEIVING MESSAGE
		//-------------------------------------------------------------------------------------------------------------

		/* try to accept a connection */
		client=SDLNet_TCP_Accept(server);
		if(!client)
		{ /* no connection accepted */
			/*printf("SDLNet_TCP_Accept: %s\n",SDLNet_GetError()); */
			SDL_Delay(100); /*sleep 1/10th of a second */
			continue;
		}
		
		/* get the clients IP and port number */
		// remoteip=SDLNet_TCP_GetPeerAddress(client);
		// if(!remoteip)
		// {
		// 	printf("SDLNet_TCP_GetPeerAddress: %s\n",SDLNet_GetError());
		// 	continue;
		// }
		
		/* read the buffer from client */
		len=SDLNet_TCP_Recv(client,message,1024);
		SDLNet_TCP_Close(client);
		if(!len)
		{
			printf("SDLNet_TCP_Recv: %s\n",SDLNet_GetError());
			continue;
		}

		/* print out the message */
		printf("Received: %.*s\n",len,message);

		if(message[0]=='Q')
		{
			printf("Quitting on a Q received\n");
			break;
		}

		char * ret_message = "Got it.";
	
		client=SDLNet_TCP_Accept(server);
		if(!client)
		{ /* no connection accepted */
			/*printf("SDLNet_TCP_Accept: %s\n",SDLNet_GetError()); */
			SDL_Delay(100); /*sleep 1/10th of a second */
			continue;
		}
		SDLNet_TCP_Send(client,ret_message,strlen(ret_message) + 1);
		SDLNet_TCP_Close(client);
		


	}
	
	/* shutdown SDL_net */
	SDLNet_Quit();

	/* shutdown SDL */
	SDL_Quit();

	return(0);
}
