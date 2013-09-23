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
int player_number = -2;

class Player {

public:
	Player(float x1, float y1, int num = -1)
		: x(x1), y(y1), number(num)
	{

	}

	float x, y;
	int number;
};

std::vector<Player> players;

std::string recv_message(TCPsocket sock) {

    char buff[MAXLEN] = {' '};
    SDLNet_TCP_Recv(sock, buff, MAXLEN);

    //std::cout << "buffer: " << buff << std::endl;
    // std::cout << "length: " << strlen(buff) << std::endl;

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

void parse_player_data(std::string message) {

	int j = 0;
	while (j < message.length()) {
		int i = 0;
		std::string x_pos;
		std::string y_pos;
		std::string num;

		// find next player
		while (message[i] != 'p') {
			i++;
		}
		i++;

		while (message[i] != ':') {
			num += message[i];
			i++;
		}
		i++;

		// x pos
		while (message[i] != ',') {
			x_pos += message[i];
			i++;
		}
		i++;

		// y pos
		while (message[i] != ';') {
			y_pos += message[i];
			i++;
		}

		float _x = atof(x_pos.c_str());
		float _y = atof(y_pos.c_str());
		int   _p = atoi(num.c_str());

		// assign to player
		Player player(_x,_y,_p);
		players.push_back(player);

		j++;
	}
		
}

void recv_player_number(std::string message) {

	int i = 0;
	std::string temp_num = "";

	if (message[0] == 'N') {
		i++;
		while (message[i] != ';') {
			temp_num += message[i];
			i++;
		}
	}

	player_number = atoi(temp_num.c_str());

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
	
	std::string player_num_string = recv_message(sock);
	recv_player_number(player_num_string);

	std::cout << "player num: " << player_number << std::endl;

	//-----------------------------------------------------------------------------------
	// GAME SEGMENT
	//-----------------------------------------------------------------------------------

	Surface surface(W, H);
	Event event;

	while(1)
	{
		
		if (event.poll() && event.type() == QUIT) break;

		KeyPressed keypressed = get_keypressed();

		to_server = "";
		if (keypressed[UPARROW]) {
			to_server = "1";
			send_message(to_server, sock);
			//std::cout << "up arrow" << std::endl;
		}
		else if (keypressed[DOWNARROW]) {
			to_server = "2";
			send_message(to_server, sock);
			//std::cout << "down arrow" << std::endl;
		}
		else if (keypressed[LEFTARROW]) {
			to_server = "3";
			send_message(to_server, sock);
			//std::cout << "left arrow" << std::endl;
		}
		else if (keypressed[RIGHTARROW]) {
			to_server = "4";
			send_message(to_server, sock);
			//std::cout << "right arrow" << std::endl;
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

			//std::cout << "from server: " << from_server << std::endl;

			if (from_server[0] == 'p') {
				parse_player_data(from_server);
			}
		}


		FD_ZERO(&fdset);
		memset(&tv, 0, sizeof(tv));
	
		surface.fill(WHITE);

		for (int i = 0; i < players.size(); i++) {
			
			if (players[i].number == player_number) {
				surface.lock();
				surface.put_rect(players[i].x, players[i].y, 2, 2, 0, 255, 0);
				surface.unlock();
			}
			else {
				surface.lock();
				surface.put_rect(players[i].x, players[i].y, 2, 2, 255, 0, 0);
				surface.unlock();
			}
		}

		surface.flip();


		delay(1);
	}

	SDLNet_Quit();
	SDL_Quit();

	return(0);
}
