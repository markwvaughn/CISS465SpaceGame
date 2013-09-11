#include <string.h>
#include "SDL.h"
#include "SDL_net.h"
#include <iostream>

typedef struct {
	TCPsocket sock;
	char *name;
} Client;

int main(int argc, char **argv) {

	IPaddress ip;
	TCPsocket sock;
	SDLNet_SocketSet set;
	char *message=NULL;
	const char *host=NULL;

	Uint32 ipaddr;
	Uint16 port;

	/* check commandline args */
	if (argc < 2) {
		std::cout << "Too few commandline args" << std::endl;
		return 0;
	}

	/* initialize SDL */
	if (SDL_Init(0)==-1) {
		std::cout << "SDL not initialized" << std::endl;
		return 0;
	}

	/* initialize SDL_net*/
	if (SDLNet_Init()==-1) {
		std::cout << "SDLNet not initialized" << std::endl;
		return 0;
	}

	/* get port from the commandline */
	port = (Uint16)strtol(argv[1],NULL,0);

	/* Resolve the argument into an IPaddress type */
	if (SDLNet_ResolveHost(&ip,NULL,port)==-1) {
		std::cout << "problems resolving host" << std::endl;
		SDLNet_Quit();
		SDL_Quit();		
		return 0;
	}

	/* resolve the hostname for the IPaddress */
	host = SDLNet_ResolveIP(&ip);

	/* open the server socket */
	server = SDLNet_TCP_Open(&ip);
	if (!server) {
		std::cout << "Server Error" << std::endl;
		return 0;
	}

	while(1) {

		int numready;
		set=create_sockset();
		numready=SDLNet_CheckSockets(set, (Uint32)-1);
		if (numready == -1) {
			std::cout << "Error with Sockets" << std::endl;
			break;
		}
		if (!numready) {
			continue;
		}

		//--------------------------------------------------------------------
		// 
		//--------------------------------------------------------------------

		if (SDLNet_SocketReady(server)) {
			numready--;
			sock = SDLNet_TCP_Accept(server);
			if (sock) {
				char *name=NULL;

				if (getMsg(sock, &name)) {
					Client *client;
					client=add_client(sock,name);
					if (client)
						do_command("WHO", client);
				}
				else
					SDLNet_TCP_Close(sock);
			}

		}

		//--------------------------------------------------------------------
		// 
		//--------------------------------------------------------------------
		for (int i = 0; numready && i < num_clients; i++) {
			if (SDLNet_SocketReady(clients[i].sock) {
				if (getMsg(clients[i].sock, &message)) {
					char *str;

					numready--;
					std::cout << "<" << clients[i].name << ">" << message << std::endl;

					/* interpret commands */
					if (message[0] == '/' && strlen(message) > 1) {
						do_command(message+1, &clients[i]);
					}
					else {
						/*it's a regular message */
						/* forward message to ALL clients..*/
						str=mformat("ssss","<",clients[i].name,">",message);
						if(str)
							send_all(str);
					}
					free(message);
					message=NULL;
				}
				else
					remove_client(i);
			}
		}

	}

	SDLNet_Quit();
	SDL_Quit();

	return 0;
}