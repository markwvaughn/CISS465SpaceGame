/******************************************************************************
 * This is the server side of the Space Shooter project for CISS465.
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
#include "Network.h"

// Miscellaneous Helpers
#include "Misc.h"

/******************************************************************************
 * END Includes.
 *****************************************************************************/


/******************************************************************************
 * Global Constants
 *****************************************************************************/
const float PI = 3.14159265;
const int FPS = 1000 / 60;
const int MAP_WIDTH = 5000;
const int MAP_HEIGHT = 5000;
const int ACTIVE = 1;
const int INACTIVE = 0;
int player_number = -1;


/******************************************************************************
 * Class definitions.
 *****************************************************************************/
class Bullet 
{

public:
	Bullet();
	Bullet(int _id, float x1, float y1, float w1 = 3, float h1 = 3, int t1 = 0, int s = INACTIVE)
		: id(_id), x(x1), y(y1), w(w1), h(h1), t(t1), state(s)
	{
	
	}

	~Bullet();

	int id;
	float x, y, w, h;
	int t;
	int state;
};

class Client
{
public:    
    Client(TCPsocket s=NULL, std::string n="", std::string pw_="",
           int _id = -1,
           float x1=0.0f, float y1=0.0f, float w1 = 50.0f, 
           float h1 = 50.0f, int t1 = 0, int s1=ACTIVE)
        : sock(s), name(n), pw(pw_), id(_id),
        x(x1), y(y1), w(w1), h(h1), state(s1), t(t1)
    {
    	bullet = new Bullet(0, x, y, 3, 3, t);
    }
    
	TCPsocket sock;
	std::string name;
    std::string pw;
	int id;
	float x, y, w, h;
    int state;
    int t;
    Bullet * bullet;
};


/******************************************************************************
 * Global Variables.
 *****************************************************************************/
std::vector<Client> clients;
int num_clients=0;
TCPsocket server;


/******************************************************************************
 * Functions
 *****************************************************************************/
void send_client(int, std::string);
void send_all(std::string buf);
int find_client_name(std::string name);
void reconnect_client(TCPsocket sock, int cindex);
void add_client(TCPsocket sock, std::string name, std::string pw);


/* find a client in our array of clients by it's socket. */
/* the socket is always unique */
int find_client(TCPsocket sock)
{
	for(int i = 0; i < num_clients; i++)
		if(clients[i].sock == sock)
			return(i);

    return -1;
}


/* find a client in our array of clients by it's name. */
/* the name is always unique */
int find_client_name(std::string name)
{
	for(int i=0; i < num_clients;i++)
		if (clients[i].name == name)
			return i;
		
	return -1;
}


// Handles registration
void handle_registration(TCPsocket sock, std::string username, std::string pw)
{
    if (username.empty() || username == " ")
	{
		send_message("ERROR. Invalid Username.", sock);
		SDLNet_TCP_Close(sock);
        
		return;
	}

    int cindex = find_client_name(username);

    if (cindex == -1)
    {
        add_client(sock, username, pw);
        send_message("Registration successful.", sock);

        return;
    }
    else
    {
        send_message("ERROR. User already exists.", sock);
        SDLNet_TCP_Close(sock);
        
        return;
    }
}


// Handles logging in
void handle_login(TCPsocket sock, std::string username, std::string pw)
{
    if (username.empty() || username == " ")
	{
		send_message("ERROR. Invalid Username.", sock);
		SDLNet_TCP_Close(sock);
        
		return;
	}

    int cindex = find_client_name(username);

    if (cindex == -1)
    {
        send_message("ERROR. User not found.", sock);
        SDLNet_TCP_Close(sock);
        
        return;
    }
    
    else if (clients[cindex].state == ACTIVE)
    {
        send_message("ERROR. User is active.", sock);
        SDLNet_TCP_Close(sock);
        
        return;
    }

    else if (clients[cindex].pw != pw)
    {
        send_message("ERROR. Wrong Password.", sock);
        SDLNet_TCP_Close(sock);

        return;
    }

    else
    {
        reconnect_client(sock, cindex);

        return;
    }

    return;
}


// Add a client to the list of clients
void add_client(TCPsocket sock, std::string name, std::string pw)
{	
	Client c;

	c.name 	= name;
    c.pw = pw;
	c.sock 	= sock;
	c.id   	= num_clients;
	c.x 	= rand() % MAP_WIDTH;
	c.y 	= rand() % MAP_HEIGHT;
	c.w 	= 50;
	c.h 	= 50;
	c.t 	= 0;
    c.state = INACTIVE;

	clients.push_back(c);

	num_clients++;

// 	std::cout << "inside add client" << std::endl;
// 	std::cout << "num clients: " << num_clients << std::endl;

	// Send an acknowledgement
    std::string player_number = "N";
	player_number += to_str(num_clients - 1);
	player_number += ";#";
    
	// send client their player number
	//std::cout << "player_number: " << player_number << std::endl;
	send_client(num_clients - 1, player_number);
}


/* closes the socket of a disconnected client */
void handle_disconnect(int i)
{
	if(i<0 || i>=num_clients)
		return;
	
	/* close the old socket, even if it's dead... */
	SDLNet_TCP_Close(clients[i].sock);
    clients[i].state = INACTIVE;
    num_clients--;
    //std::cout << "Removed client # " << i << std::endl;
    //std::cin.ignore();
}


/* Reconnects a client */
void reconnect_client(TCPsocket sock, int cindex)
{
    clients[cindex].sock = sock;
    clients[cindex].state = ACTIVE;
    num_clients++;
}


/* create a socket set that has the server socket and all the client sockets */
SDLNet_SocketSet create_sockset()
{
	static SDLNet_SocketSet set=NULL;

	if(set)
		SDLNet_FreeSocketSet(set);
                                      
	set = SDLNet_AllocSocketSet(num_clients + 1);
	if(!set)
    {
        std::cerr << "SDLNet_AllocSocketSet: " << SDLNet_GetError() << std::endl;
        return 0;
	}
	SDLNet_TCP_AddSocket(set, server);
	for(int i=0; i < clients.size(); i++)
        if (clients[i].state == ACTIVE)
            SDLNet_TCP_AddSocket(set, clients[i].sock);

    return(set);
}


/* send a buffer to all clients */
void send_all(std::string buf)
{
	if(buf == "" || num_clients == 0)
		return;
    
    for (int i = 0; i < num_clients; i++)
	{
		if(!send_message(buf, clients[i].sock))
            handle_disconnect(i);
	}
}


// Send a string to a particular client
void send_client(int i, std::string buf)
{
	if (buf == "")
        return;

	send_message(buf, clients[i].sock);
}


// Generate the string to be sent or something
std::string generate_string_for_clients()
{
    std::ostringstream ret;

    ret << num_clients << ' ';
    for (int i = 0; i < num_clients; i++)
    {
        ret << clients[i].id << ' '
        	<< clients[i].x << ' ' 
        	<< clients[i].y << ' ' 
        	<< clients[i].w << ' '
        	<< clients[i].h << ' ' 
        	<< clients[i].t << ' ' 
        	<< clients[i].state << ' '
        	<< clients[i].bullet->id << ' '
        	<< clients[i].bullet->x << ' ' 
        	<< clients[i].bullet->y << ' ' 
        	<< clients[i].bullet->w << ' '
        	<< clients[i].bullet->h << ' ' 
        	<< clients[i].bullet->t << ' ' 
        	<< clients[i].bullet->state << ' ';
	}

    return ret.str();
}


// Update the position of a client
void update_position(int i, std::string message)
{
	float x_pos = 10 * cos((PI*(clients[i].t + 90)) / 180);
	float y_pos = 10 * sin((PI*(clients[i].t + 90)) / 180);

	if (message == "1") 
	{
		clients[i].y -= y_pos;
		clients[i].x += x_pos;		
	}
	else if (message == "2")
	{
		clients[i].t += 10;

		if (clients[i].t > 350)
			clients[i].t = 0;
	}
	else if (message == "3")
	{
	   	clients[i].t -= 10;
	   	if (clients[i].t < 0)
			clients[i].t = 350;
	}
    else if (message == "4")
    {
    	clients[i].bullet->t = clients[i].t;
    	clients[i].bullet->state = ACTIVE;
    	//std::cout << "bullet active" << std::endl;
    }
}


void bullet_collision_with_wall(int i)
{
	// left wall
	if (clients[i].bullet->x < 0) {
		clients[i].bullet->state = INACTIVE;
	}

	// right wall
	if (clients[i].bullet->x > W) {
		clients[i].bullet->state = INACTIVE;
	}

	// top wall
	if (clients[i].bullet->y < 0) {
		clients[i].bullet->state = INACTIVE;
	}

	// bottom wall
	if (clients[i].bullet->y > H) {
		clients[i].bullet->state = INACTIVE;
	}
}


void player_collide_map_bounds(int i)
{
	// left wall
	if (clients[i].x < 0)
    {
		clients[i].x = 0;
	}

	// right wall
	if (clients[i].x + clients[i].w > MAP_WIDTH)
    {
		clients[i].x = MAP_WIDTH - clients[i].w;
	}

	// top wall
	if (clients[i].y < 0)
    {
		clients[i].y = 0;
	}

	// bottom wall
	if (clients[i].y + clients[i].h > MAP_HEIGHT)
    {
		clients[i].y = MAP_HEIGHT - clients[i].h;
	}
}

void bullet_collide_with_player(int i)
{

	if (clients[i].bullet->state == INACTIVE)
		return;

	for (int j = 0; j < num_clients; j++)
	{
		if (clients[i].bullet->x > clients[j].x && clients[i].bullet->x < clients[j].x + clients[j].w &&
			clients[i].bullet->y > clients[j].y && clients[i].bullet->y < clients[j].y + clients[j].h && i != j)
		{
			clients[j].state = INACTIVE;
			clients[i].bullet->state = INACTIVE;
			return;
		}
	}

	return;
}

void player_collide_with_player(int i)
{
	for (int j = 0; j < num_clients; j++)
	{
		if (clients[i].x > clients[j].x && clients[i].x < clients[j].x + clients[j].w &&
			clients[i].y > clients[j].y && clients[i].y < clients[j].y + clients[j].h &&
			clients[i].state == ACTIVE && clients[j].state == ACTIVE)
		{
			clients[i].state = INACTIVE;
			clients[j].state = INACTIVE;
			return;
		}
	}

	return;
}


// Point of entry
int main(int argc, char **argv)
{
	IPaddress ip;
	TCPsocket sock;
	SDLNet_SocketSet set;
	
	std::string message;
	
	Uint32 ipaddr;
	Uint16 port;
	
	/* check our commandline */
	if(argc < 2)
	{
		std::cout << argv[0] << "port\n";
		exit(0);
	}
	
	/* initialize SDL */
	if(SDL_Init(0)==-1)
	{
		std::cerr << "SDL_Init ERROR: " << SDL_GetError() << std::endl;
		exit(1);
	}

	/* initialize SDL_net */
	if(SDLNet_Init()==-1)
	{
		std::cerr << "SDLNet_Init ERROR: " << SDLNet_GetError() << std::endl;
		SDL_Quit();
		exit(2);
	}

	/* get the port from the commandline */
	port=(Uint16)strtol(argv[1],NULL,0);

	/* Resolve the argument into an IPaddress type */
	if(SDLNet_ResolveHost(&ip,NULL,port)==-1)
	{
		std::cerr << "SDLNet_ResolveHost ERROR:"
                  << SDLNet_GetError() << std::endl;
		SDLNet_Quit();
		SDL_Quit();
		exit(3);
	}

	/* open the server socket */
	server=SDLNet_TCP_Open(&ip);
	if(!server)
	{
		std::cerr << "SDLNet_TCP_Open ERROR: "
                  << SDLNet_GetError() << std::endl;
		SDLNet_Quit();
		SDL_Quit();
		exit(4);
	}

	while(1)
	{
		int numready;

		set = create_sockset();

		numready = SDLNet_CheckSockets(set, (Uint32)1000);

		if (numready == -1)
		{
			std::cerr << "SDLNet_CheckSockets ERROR: "
                      << SDLNet_GetError() << std::endl;
			break;
		}
		if (numready == 0)
			continue;
        
		if (SDLNet_SocketReady(server))
		{
			numready--;

			sock=SDLNet_TCP_Accept(server);
			if (sock)
			{
				std::string user;
                std::string pw;
                
                message = recv_message(sock);
                
                int pos = message.find_first_of(':');
                user = message.substr(0, pos);
                pw = message.substr(pos + 1);
                
				std::cout << "name: " << user << std::endl;
                std::cout << "pw: " << pw << std::endl;

                if (message[0] == '$')
                    handle_registration(sock, user, pw);
                else if (message[0] == '#')
                    handle_login(sock, user, pw);
                else
                {
                    std::cerr << "User data compromised." << std::endl;
                    exit(5);
                }
            }
            else
                SDLNet_TCP_Close(sock);
		}


		//---------------------------------------------------------------------
		// LOOP THROUGH CLIENTS
		//---------------------------------------------------------------------
		for (int i = 0; numready > 0 && i < clients.size(); i++)
		{
			// collisions
			bullet_collision_with_wall(i);
			player_collide_map_bounds(i);
			bullet_collide_with_player(i);
			player_collide_with_player(i);


			// fire bullets
			if (clients[i].bullet->state == INACTIVE)
			{
				clients[i].bullet->x = clients[i].x + clients[i].w/2
                    + 1 * cos((PI*(clients[i].t + 90)) / 180);
				clients[i].bullet->y = clients[i].y + clients[i].h/2
                    - 1 * sin((PI*(clients[i].t + 90)) / 180);
			}
			else if (clients[i].bullet->state == ACTIVE)
            {
				float x_pos = 5 * cos((PI*(clients[i].bullet->t + 90)) / 180);
				float y_pos = 5 * sin((PI*(clients[i].bullet->t + 90)) / 180);

				clients[i].bullet->x += x_pos;
				clients[i].bullet->y -= y_pos;
			}

			message = "";
            if (clients[i].state == ACTIVE)
            {
                if (SDLNet_SocketReady(clients[i].sock))
                {
                    //---------------------------------------------------------
                    // GET DATA FROM CLIENT
                    //---------------------------------------------------------
                    message = recv_message(clients[i].sock);
                    
                    if (message > "") 
                        update_position(i, message);
                    
                    numready--;
                }
            }
        }
        
        send_all(generate_string_for_clients());
	}
    
	/* shutdown SDL_net */
	SDLNet_Quit();

	/* shutdown SDL */
	SDL_Quit();

	return(0);
}