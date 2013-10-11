/******************************************************************************
 * This is the client side of the Space Shooter project for CISS465
 *****************************************************************************/


/******************************************************************************
 * BEGIN Includes.
 *****************************************************************************/

// Standard includes
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

// SDL wrapper from Dr. Liow
#include "Includes.h"
#include "Event.h"
#include "compgeom.h"
#include "Constants.h"
#include "Surface.h"

// SDL net
#include "SDL_net.h"


/******************************************************************************
 * END Includes.
 *****************************************************************************/


/******************************************************************************
 * Global Constants.
 *****************************************************************************/
const float PI = 3.14159265;
const int MAP_WIDTH = 5000;
const int MAP_HEIGHT = 5000;
const int ACTIVE = 1;
const int INACTIVE = 0;
const int MAXLEN = 1024;
int player_number = -1;


// Conversion functions
std::string ftos(float f) 
{
	std::ostringstream buff;
	buff << f;

	return buff.str();
}


std::string itos(int i) 
{
	std::ostringstream buff;
	buff << i;

	return buff.str();
}


/******************************************************************************
 * Class definitions.
 *****************************************************************************/
class Bullet 
{

public:
	Bullet();
	Bullet( int _id, float x1, float y1, float w1 = 3, float h1 = 3,
			int t1 = 0, int s = INACTIVE)
		: id(_id), x(x1), y(y1), w(w1), h(h1), t(t1), state(s)
	{
	
	}

	~Bullet();

	void draw(Surface &);

	float x, y, w, h;
	int t;
	int id;
	int state;

};


void Bullet::draw(Surface & surface) 
{
	if (state == ACTIVE)
		surface.put_rect(x, y, w, h, 255, 0, 0);
}


class Player 
{

public:
	Player(	int _id, float x1, float y1, float w1, float h1,
		   	int s = ACTIVE, int t1 = 0)
		: id(_id), x(x1), y(y1), w(w1), h(h1), t(t1), state(s)
	{
		bullet = new Bullet(0, x, y, 3, 3, t);
	}

	void draw(Surface &);
	void draw_bullet(Surface &);

	float x, y, w, h;
	int id;
	int t;
	Bullet * bullet;
	int state;
};
std::vector<Player> players;


void Player::draw(Surface & surface) 
{
	if (state != ACTIVE)
		return;

	std::string sprite_path = "images/newships/";
	sprite_path += itos(id);
	sprite_path += ".png";

	Image sprite(sprite_path.c_str());
	
	Rect display(x, y, 50, 50);
	Rect source((t/10)*50, 0, 50, 50);
	surface.put_image(sprite, source, display);
}


void Player::draw_bullet(Surface & surface) 
{
	bullet->draw(surface);
}


/******************************************************************************
 * Global Variables.
 *****************************************************************************/
SDL_Thread *net_thread=NULL, *local_thread=NULL;


/******************************************************************************
 * Functions
 *****************************************************************************/

// Receive a string over TCP/IP
std::string recv_message(TCPsocket sock)
{
    char buff[MAXLEN] = {' '};
    SDLNet_TCP_Recv(sock, buff, MAXLEN);

    if (buff == NULL)
    {
        std::string ret = "";
        return ret;
    }
    
    std::string ret(buff, strlen(buff));
    return ret;
}


// Send a string over TCP/IP
int send_message(std::string msg, TCPsocket sock)
{
    char * buff = (char *)msg.c_str();      
    SDLNet_TCP_Send(sock, buff, MAXLEN);

    return 1;
}


void parse_player_data(std::string message)
{
    std::stringstream message_stream(message);
    
    int num_players = 0;
    message_stream >> num_players;
	for (int i = 0; i < num_players; i++)
    {
    	int 	id  = -1;
    	float 	x 	= -1;
		float 	y 	= -1;
		float 	w 	= -1;
		float 	h 	= -1;
		int 	t 	= -1;
		int 	s 	= -1;
		
		int 	bid = -1;
    	float	bx 	= -1;
		float 	by 	= -1;
		float 	bw 	= -1;
		float	bh 	= -1;
		int 	bt 	= -1;
		int 	bs 	= -1;

    	message_stream >> id;
    	message_stream >> x;
     	message_stream >> y;
     	message_stream >> w;
     	message_stream >> h;
     	message_stream >> t;
     	message_stream >> s;
     	message_stream >> bid;
     	message_stream >> bx;
     	message_stream >> by;
     	message_stream >> bw;
     	message_stream >> bh;
     	message_stream >> bt;
     	message_stream >> bs;

     	//std::cout << "id: " << id << std::endl;

 		if (i >= players.size())
    	{
    		//std::cout << "creating player" << std::endl;
    		Player player(id, x, y, w, h, t, s);
    		player.bullet->id = bid;
    		player.bullet->x = bx;
    		player.bullet->y = by;
    		player.bullet->w = bw;
    		player.bullet->h = bh;
    		player.bullet->t = bt;
    		player.bullet->state = bs;
    		players.push_back(player);
    	}
    	else
    	{
    		players[i].id = id;
    		players[i].x = x;
    		players[i].y = y;
    		players[i].w = w;
    		players[i].h = h;
    		players[i].t = t;
    		players[i].state = s;

    		players[i].bullet->id = bid;
    		players[i].bullet->x = bx;
    		players[i].bullet->y = by;
    		players[i].bullet->w = bw;
    		players[i].bullet->h = bh;
    		players[i].bullet->t = bt;
    		players[i].bullet->state = bs;	
    	}
	}		
}


void recv_player_number(std::string message)
{
	int i = 0;
	std::string temp_num = "";

	if (message[0] == 'N')
    {
		i++;
		while (message[i] != ';')
        {
			temp_num += message[i];
			i++;
		}
	}

	player_number = atoi(temp_num.c_str());
}

void draw_mini_map(Surface & surface)
{
	Rect p1_mini_map(W - 50, H - 50, 2, 2);
}


int main(int argc, char **argv)
{
	IPaddress ip;
	TCPsocket sock;
  
	int numready;
	Uint16 port;
	SDLNet_SocketSet set;	

	std::string name;
	std::string to_server;
	std::string from_server;

	/* check our commandline */
	if(argc < 4)
	{
		std::cout << "Usage: " << argv[0] << " host_ip host_port username" << std::endl;
		return 0;
	}
	
	name = argv[3];
	
	/* initialize SDL */
	if(SDL_Init(SDL_INIT_EVERYTHING) == -1)
	{
		std::cout << "SDL_Init ERROR" << std::endl;
		return 0;
	}

	/* initialize SDL_net */
	if(SDLNet_Init() == -1)
	{
		std::cout << "SDLNet_Init ERROR" << std::endl;
		SDL_Quit();
		return 0;
	}

	set = SDLNet_AllocSocketSet(1);
	if(!set)
	{
		std::cout << "SDLNet_AllocSocketSet ERROR" << std::endl;
		SDLNet_Quit();
		SDL_Quit();
		return 0;
	}

	/* get the port from the commandline */
	port = (Uint16)strtol(argv[2],NULL,0);
	
	/* Resolve the argument into an IPaddress type */
	std::cout << "connecting to " << argv[1] << " port " << port << std::endl;

	if(SDLNet_ResolveHost(&ip,argv[1],port) == -1)
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

	//std::cout << "Logged in as: " << name << std::endl;
	
	recv_player_number(recv_message(sock));

	// std::cout << "player num: " << player_number << std::endl;
	// std::cout << "players.size() is " << players.size() << std::endl;

	//-------------------------------------------------------------------------
	// GAME SEGMENT
	//-------------------------------------------------------------------------
	Surface surface(W, H);
	Event event;

	Image background_image("images/map/map-01.png");
	Rect background_source;
	background_source.x = 0;
	background_source.y = 0;
	background_source.w = 5000;
	background_source.h = 5000;

	Rect mini_map(W - 50, H - 50, 50, 50);

	while(1)
	{
		numready=SDLNet_CheckSockets(set, 100);
        if(numready == -1)
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
            //std::cout << from_server << std::endl;

            parse_player_data(from_server);
		}

		if (event.poll() && event.type() == QUIT) break;

		KeyPressed keypressed = get_keypressed();

		to_server = "";

		if (players[player_number].bullet->state == ACTIVE) 
		{
			to_server = "5";
			send_message(to_server, sock);
		}

		if (keypressed[UPARROW])
        {
			to_server = "1";
			send_message(to_server, sock);
		}
		else if (keypressed[LEFTARROW])
        {
			to_server = "2";
			send_message(to_server, sock);
		}
		else if (keypressed[RIGHTARROW])
        {
			to_server = "3";
			send_message(to_server, sock);
		}
		else if (keypressed[SPACE] && players[player_number].bullet->state == INACTIVE)
  		{
			to_server = "4";
			send_message(to_server, sock);
		}

		surface.fill(WHITE);

		Rect background_display;
    	background_display.x = 0;// - W/2 + players[player_number].w/2;
    	background_display.y = 0;// - H/2 + players[player_number].h/2;
    	background_display.w = W;
    	background_display.h = H;

    	surface.put_image(background_image, background_source, background_display);

		for (int i = 0; i < players.size(); i++)
        {
        	//std::cout << i << ' ' << players[i].x << ' ' << players[i].y << std::endl;

            if (players[i].state == ACTIVE)
            {
        		//if (players[i].x + players[i].w > background_display.x && players[i].x < background_display.x + background_display.w &&
           		//players[i].y > background_display.y && players[i].y < background_display.y + background_display.h)
               	//{
               		if (i != player_number) std::cout << "here" << std::endl;
	                surface.lock();
	                if (players[i].bullet->state == ACTIVE)
	               	{
	               		players[i].draw_bullet(surface);
	               	}
	               	players[i].draw(surface);
                	surface.unlock();
	            //}
	            std::cout << "not here" << std::endl;
            }
        }

        surface.lock();
	    surface.put_rect(mini_map, WHITE);
	    surface.unlock();
        for (int i = 0; i < players.size(); i++)
        {
    		surface.lock();
			Rect mini_map_player;
			mini_map_player.x = mini_map.x + (players[i].x / 100);
			mini_map_player.y = mini_map.y + (players[i].y / 100);
			mini_map_player.w = 3;
			mini_map_player.h = 3;
			if (players[i].id == player_number) surface.put_rect(mini_map_player, GREEN);
			else surface.put_rect(mini_map_player, RED);
			surface.unlock();
        }

		surface.flip();


		delay(1);
	}

	SDLNet_Quit();
	SDL_Quit();

	return(0);
}
