#include <stdio.h>
#include <stdlib.h>
#ifndef _MSC_VER
#include <unistd.h>
#endif
#include <string.h>
#include "SDL.h"
#include "SDL_net.h"

int main(int argc, char **argv)
{

	SDL_Init(SDL_INIT_EVERYTHING);
	SDLNet_Init();

	TCPsocket sock;
	char message[1024];
	int len;
	
	IPaddress ip;
	SDLNet_ResolveHost(&ip, "127.0.0.1", 1234);

	/* open the server socket */
	sock=SDLNet_TCP_Open(&ip);
	if(!sock)
	{
		printf("SDLNet_TCP_Open: %s\n",SDLNet_GetError());
		exit(4);
	}

	/* read the buffer from stdin */
	printf("Enter Message, or Q to make the server quit:\n");
	fgets(message,1024,stdin);
	len=strlen(message);

	/* strip the newline */
	message[len-1]='\0';
	
	if(len)
	{
		int result;
		
		/* print out the message */
		printf("Sending: %.*s\n",len,message);

		result=SDLNet_TCP_Send(sock,message,len); /* add 1 for the NULL */
		if(result<len)
			printf("SDLNet_TCP_Send: %s\n",SDLNet_GetError());
	}

	SDLNet_TCP_Close(sock);
	
	/* shutdown SDL_net */
	SDLNet_Quit();

	/* shutdown SDL */
	SDL_Quit();

	return(0);
}
