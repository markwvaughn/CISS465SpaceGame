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
#include "SDL_net.h"

const int MAXLEN = 1024;

std::string recv_message(TCPsocket sock) {

    char buff[MAXLEN];
	SDLNet_TCP_Recv(sock, buff, MAXLEN);

	std::cout << "buffer: " << buff << std::endl;
	std::cout << "length: " << strlen(buff) << std::endl;

    if (buff == NULL) {
    	std::string ret = "";
    	return ret;
    }
    std::string ret(buff, MAXLEN);
    ret = ret.substr(0, strlen(buff));
    return ret;
}

int send_message(std::string msg, TCPsocket sock) {

	char * buff = (char *)msg.c_str();		
	SDLNet_TCP_Send(sock, buff, MAXLEN);

    return 1;
}


int main(int argc, char **argv)
{
	IPaddress ip;
	TCPsocket sock;
	char message[MAXLEN];
	int numready;
	Uint16 port;
	SDLNet_SocketSet set;
	fd_set fdset;
	int result;
	struct timeval tv;

	std::string name;
	std::string to_server;
	std::string from_server;

	/* check our commandline */
	if(argc<4)
	{
		std::cout << "commandline ERROR" << std::endl;
		return 0;
	}
	
	name=argv[3];
	
	/* initialize SDL */
	if(SDL_Init(0)==-1)
	{
		std::cout << "SDL_Init ERROR" << std::endl;
		return 0;
	}

	/* initialize SDL_net */
	if(SDLNet_Init()==-1)
	{
		std::cout << "SDLNet_Init ERROR" << std::endl;
		SDL_Quit();
		return 0;
	}

	set=SDLNet_AllocSocketSet(1);
	if(!set)
	{
		std::cout << "SDLNet_AllocSocketSet ERROR" << std::endl;
		SDLNet_Quit();
		SDL_Quit();
		return 0;
	}

	/* get the port from the commandline */
	port=(Uint16)strtol(argv[2],NULL,0);
	
	/* Resolve the argument into an IPaddress type */
	std::cout << "connecting to " << argv[1] << " port " << port << std::endl;

	if(SDLNet_ResolveHost(&ip,argv[1],port)==-1)
	{
		std::cout << "SDLNet_ResolveHost ERROR" << std::endl;
		SDLNet_Quit();
		SDL_Quit();
		return 0;
	}

	/* open the server socket */
	sock=SDLNet_TCP_Open(&ip);
	if(!sock)
	{
		std::cout << "SDLNet_TCP_Open ERROR" << std::endl;
		SDLNet_Quit();
		SDL_Quit();
		return 0;
	}
	
	if(SDLNet_TCP_AddSocket(set,sock)==-1)
	{
		std::cout << "SDLNet_TCP_AddSocket ERROR" << std::endl;
		SDLNet_Quit();
		SDL_Quit();
		return 0;
	}

	send_message(name, sock);
	std::cout << "Logged in as: " << name << std::endl;

	//-----------------------------------------------------------------------------------
	// GAME SEGMENT
	//-----------------------------------------------------------------------------------

	Surface surface(W, H);
	Event event;

	int rect_x;
	int rect_y;
	int rect_w;
	int rect_h;

	rect_x = 0;
	rect_y = 0;
	rect_w = 30;
	rect_h = 30;

	while(1)
	{
		
		if (event.poll() && event.type() == QUIT) break;

		KeyPressed keypressed = get_keypressed();

		to_server = "";
		if (keypressed[UPARROW]) {
			to_server = "1";
			send_message(to_server, sock);
			std::cout << "up arrow" << std::endl;
		}
		else if (keypressed[DOWNARROW]) {
			to_server = "2";
			send_message(to_server, sock);
			std::cout << "down arrow" << std::endl;
		}
		else if (keypressed[LEFTARROW]) {
			to_server = "3";
			send_message(to_server, sock);
			std::cout << "left arrow" << std::endl;
		}
		else if (keypressed[RIGHTARROW]) {
			to_server = "4";
			send_message(to_server, sock);
			std::cout << "right arrow" << std::endl;
		}

		numready=SDLNet_CheckSockets(set, 100);
		if(numready==-1)
		{
			std::cout << "SDLNet_CheckSockets ERROR" << std::endl;
			break;
		}

		//-------------------------------------------------------------------------------
		// GET DATA FROM SERVER
		//-------------------------------------------------------------------------------
		from_server = "";
		if(numready && SDLNet_SocketReady(sock))
		{
			
			from_server = recv_message(sock);
			if (from_server[0] == 'p') {

				std::string x_pos;
				std::string y_pos;	

				int i = 1;
				while (from_server[i] != ',') {
					x_pos += from_server[i];
					i++;
				}

				i++;
				while (from_server[i] != ';') {
					y_pos += from_server[i];
					i++;
				}

				std::cout << x_pos << std::endl;
				std::cout << y_pos << std::endl;

				rect_x = atoi(x_pos.c_str());
				rect_y = atoi(y_pos.c_str());
			}
		}


		FD_ZERO(&fdset);
		memset(&tv, 0, sizeof(tv));
	
		surface.lock();
		surface.fill(BLACK);
		surface.put_rect(rect_x, rect_y, rect_w, rect_h, 0, 255 , 0);
		surface.unlock();
		surface.flip();

		delay(10);
	}

	SDLNet_Quit();
	SDL_Quit();

	return(0);
}
