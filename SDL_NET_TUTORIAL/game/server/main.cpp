/******************************************************************************
 * This is a very simple example with SDL_net. There is no error checking atm.
 * Run client.exe or telnet 127.0.0.1 1234 (local client) to test
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
 * Global Constants
 *****************************************************************************/
const int ACTIVE = 1;
const int INACTIVE = 0;
const int MAXLEN = 1024;
const float PI = 3.14159265;
const int MAP_X = W;//5000;
const int MAP_Y = H;//5000;

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
    Client(TCPsocket s=NULL, std::string n="", int _id = -1,
           float x1=0.0f, float y1=0.0f, float w1 = 50.0f, 
           float h1 = 50.0f, int t1 = 0, int s1=ACTIVE)
        : sock(s), name(n), id(_id), x(x1), y(y1), w(w1), h(h1), state(s1), t(t1)
    {
    	bullet = new Bullet(0, x, y, 3, 3, t);
    }
    
	TCPsocket sock;
	std::string name;
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
void reconnect(std::string name);//, std::string password)
void add_client(TCPsocket sock, std::string name);


// Converts a float to string
std::string ftos(float f)
{
	std::ostringstream buff;
	buff << f;

	return buff.str();
}


// Converts an integer to string
std::string itos(int i)
{
	std::ostringstream buff;
	buff << i;

	return buff.str();
}


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
    return SDLNet_TCP_Send(sock, buff, MAXLEN);
}


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


// Handles logging in
void handle_login(TCPsocket sock, std::string name)
{
    if(!name.length())
	{
		send_message("Invalid Nickname...bye bye!", sock);
		SDLNet_TCP_Close(sock);
		return;
	}

    int cindex = find_client_name(name);

    if (cindex == -1)
    {
        add_client(sock, name);
        return;
    }
    
    if (clients[cindex].state)
    {
        send_message("Duplicate Nickname... bye bye!", sock);
        SDLNet_TCP_Close(sock);
        return;
    }

    std::cout << "cindex: " << cindex << std::endl;

    clients[cindex].sock = sock;
    clients[cindex].state = ACTIVE;
    return;
}


// Add a client to the list of clients
void add_client(TCPsocket sock, std::string name)
{	
	Client c;

	c.name 	= name;
	c.sock 	= sock;
	c.id   	= num_clients;
	c.x 	= rand() % W;
	c.y 	= rand() % H;
	c.w 	= 50;
	c.h 	= 50;
	c.t 	= 0;
    c.state = ACTIVE;

	clients.push_back(c);

	num_clients++;

	std::cout << "inside add client" << std::endl;
	std::cout << "num clients: " << num_clients << std::endl;

	// Send an acknowledgement
    std::string player_number = "N";
	player_number += itos(num_clients - 1);
	player_number += ";#";
	// send client their player number

	std::cout << "player_number: " << player_number << std::endl;
	send_client(num_clients - 1, player_number);
}


/* closes the socket of a disconnected client */
void handle_disconnect(int i)
{
	std::string name=clients[i].name;

	if(i<0 || i>=num_clients)
		return;
	
	/* close the old socket, even if it's dead... */
	SDLNet_TCP_Close(clients[i].sock);
    clients[i].state = INACTIVE;
    //std::cout << "Removed client # " << i << std::endl;
    //std::cin.ignore();
}


/* Reconnects a client */
void reconnect_client(std::string name)//, std::string password)
{
    clients[find_client_name(name)].state = ACTIVE;
    // pass for now
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
	for(int i=0; i < num_clients; i++)
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
        {
            std::cout << "errr what \n";
            handle_disconnect(i);
        }
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
    	std::cout << "bullet active" << std::endl;
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
	if (clients[i].x < 0) {
		clients[i].x = 0;
	}

	// right wall
	if (clients[i].x + clients[i].w > MAP_X) {
		clients[i].x = MAP_X - clients[i].w;
	}

	// top wall
	if (clients[i].y < 0) {
		clients[i].y = 0;
	}

	// bottom wall
	if (clients[i].y + clients[i].h > MAP_Y) {
		clients[i].y = MAP_Y - clients[i].h;
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
		//printf("SDL_Init: %s\n",SDL_GetError());
		std::cout << "ERROR" << std::endl;
		exit(1);
	}

	/* initialize SDL_net */
	if(SDLNet_Init()==-1)
	{
		//printf("SDLNet_Init: %s\n",SDLNet_GetError());
		std::cout << "ERROR" << std::endl;
		SDL_Quit();
		exit(2);
	}

	/* get the port from the commandline */
	port=(Uint16)strtol(argv[1],NULL,0);

	/* Resolve the argument into an IPaddress type */
	if(SDLNet_ResolveHost(&ip,NULL,port)==-1)
	{
		std::cout << "SDLNet_ResolveHost: ERROR" << std::endl;
		SDLNet_Quit();
		SDL_Quit();
		exit(3);
	}

	/* open the server socket */
	server=SDLNet_TCP_Open(&ip);
	if(!server)
	{
		std::cout << "SDLNet_TCP_Open ERROR" << std::endl;
		SDLNet_Quit();
		SDL_Quit();
		exit(4);
	}

	while(1)
	{
		int numready;
        //std::cout << "HERE " << std::endl;
		set = create_sockset();
        //std::cout << "ALSO HERE " << std::endl;
		numready = SDLNet_CheckSockets(set, (Uint32)1000);
        //std::cout << "Numready " << numready << std::endl;
		if(numready == -1)
		{
			std::cout << "SDLNet_CheckSockets ERROR" << std::endl;
			break;
		}
		if(numready == 0)
			continue;
        
		if(SDLNet_SocketReady(server))
		{
			numready--;

			sock=SDLNet_TCP_Accept(server);
			if(sock)
			{
				std::string name;
                
				name = recv_message(sock);
				std::cout << "name: " << name << std::endl;

                handle_login(sock,name);
            }
            else
                SDLNet_TCP_Close(sock);
		}


		//---------------------------------------------------------------------
		// LOOP THROUGH CLIENTS
		//---------------------------------------------------------------------
		for(int i = 0; numready > 0 && i < num_clients; i++)
		{
			// collisions
			bullet_collision_with_wall(i);
			player_collide_map_bounds(i);
			bullet_collide_with_player(i);
			player_collide_with_player(i);


			// fire bullets
			if (clients[i].bullet->state == INACTIVE)
			{
				clients[i].bullet->x = clients[i].x + clients[i].w/2 + 1 * cos((PI*(clients[i].t + 90)) / 180);
				clients[i].bullet->y = clients[i].y + clients[i].h/2 - 1 * sin((PI*(clients[i].t + 90)) / 180);
			}
			else if (clients[i].bullet->state == ACTIVE){
				float x_pos = 5 * cos((PI*(clients[i].bullet->t + 90)) / 180);
				float y_pos = 5 * sin((PI*(clients[i].bullet->t + 90)) / 180);

				clients[i].bullet->x += x_pos;
				clients[i].bullet->y -= y_pos;
			}

			message = "";
            if (clients[i].state == ACTIVE)
            {
                if(SDLNet_SocketReady(clients[i].sock))
                {
                    //---------------------------------------------------------
                    // GET DATA FROM CLIENT
                    //---------------------------------------------------------
                    message = recv_message(clients[i].sock);
                    
                    if(message > "") 
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

/*
int main()
{
    num_clients++;
    TCPsocket sock;
    clients.push_back(Client(sock, "Ujjwal", 100, 200, false));
    //std::cout << "Finding Ujjwal " << find_client_name("Ujjwal") << std::endl;
    //std::cout << "Is Ujjwal available as a nick? " << is_nick_available("Ujjwal") << std::endl;
    add_client(sock, "Ujjwal");
    return 0;
}
*/
