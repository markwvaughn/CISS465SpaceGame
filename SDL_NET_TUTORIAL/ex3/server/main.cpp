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
 * Class definitions.
 *****************************************************************************/
class Client
{
public:
	TCPsocket sock;
	std::string name;
	float x, y;
    bool active;
};


/******************************************************************************
 * Global Constants.
 *****************************************************************************/
const int MAXLEN = 1024;


/******************************************************************************
 * Global Variables.
 *****************************************************************************/
int running=1;
std::vector<Client> clients;
int num_clients=0;
TCPsocket server;


/******************************************************************************
 * Functions
 *****************************************************************************/
void send_client(int, std::string);
void send_all(std::string buf);
int find_client_name(std::string name);


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
    SDLNet_TCP_Send(sock, buff, MAXLEN);

    return 1;
}


// Check if the desired nickname from the client is available
bool is_nick_available(std::string s)
{
	return(find_client_name(s)==-1);
}


// Add a client to the list of clients
void add_client(TCPsocket sock, std::string name)
{
	if(!name.length())
	{
		send_message("Invalid Nickname...bye bye!", sock);
		SDLNet_TCP_Close(sock);
		return;
	}
	if(!is_nick_available(name))
	{
		send_message("Duplicate Nickname...bye bye!", sock);
		SDLNet_TCP_Close(sock);
		return;
	}
	
	Client c;

	c.name = name;
	c.sock = sock;
	c.x= rand() % W;
	c.y= rand() % H;
    c.active = true;

	clients.push_back(c);

	num_clients++;

	std::cout << "inside add client" << std::endl;
	std::cout << "num clients: " << num_clients << std::endl;

	// Send an acknowledgement
    std::string player_number = "N";
	player_number += itos(num_clients - 1);
	player_number += ";#";
	// send client their player number
	send_client(num_clients - 1, player_number);
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


/* closes the socket of a disconnected client */
void handle_disconnect(int i)
{
	std::string name=clients[i].name;

	if(i<0 || i>=num_clients)
		return;
	
	/* close the old socket, even if it's dead... */
	SDLNet_TCP_Close(clients[i].sock);
    clients[i].active = false;
}


/* Reconnects a client */
void reconnect(std::string name, std::string password)
{
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
	for(int i=0; i<num_clients; i++)
		SDLNet_TCP_AddSocket(set, clients[i].sock);
	return(set);
}


/* send a buffer to all clients */
void send_all(std::string buf)
{
	int cindex;

	if(buf == "" || num_clients == 0)
		return;
    
	cindex = 0;

    while(cindex < num_clients)
	{
		if(send_message(buf, clients[cindex].sock))
			cindex++;
		else
			handle_disconnect(cindex);
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
std::string format_pos_string(int i)
{
	std::string str="p";
	str += itos(i);
	str += ":";
	str += ftos(clients[i].x);
	str += ",";
	str += ftos(clients[i].y);
	str += ";";

	return str;
}


// Update the position of a client
void update_position(int i, std::string message)
{
	if (message == "1") 
		clients[i].y -= 0.5f;
	else if (message == "2")
		clients[i].y += 0.5f;
	else if (message == "3")
		clients[i].x -= 0.5f;
	else if (message == "4")
        clients[i].x += 0.5f;
}


// Generates the string to be sent to all clients
std::string calculate_positions()
{
	std::string ret;
	for (int i = 0; i < num_clients; i++)
    {
        std::string str = format_pos_string(i);
        ret += str;
    }

	return ret;
}


// Point of entry
int main(int argc, char **argv)
{
	IPaddress ip;
	TCPsocket sock;
	SDLNet_SocketSet set;
	
	std::string message;
	
	const char *host=NULL;
	Uint32 ipaddr;
	Uint16 port;
	
	/* check our commandline */
	if(argc<2)
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

	/* resolve the hostname for the IPaddress */
	host=SDLNet_ResolveIP(&ip);

	/* open the server socket */
	server=SDLNet_TCP_Open(&ip);
	if(!server)
	{
		std::cout << "SDLNet_TCP_Open ERROR" << std::endl;
		SDLNet_Quit();
		SDL_Quit();
		exit(4);
	}

	std::cout << "Host: " << host << std::endl;
    std::cout << "IP: " << ipaddr << std::endl;
	std::cout << "Port: " << port << std::endl;


	while(1)
	{
		int numready;
		set = create_sockset();
		numready = SDLNet_CheckSockets(set, (Uint32)-1);
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

                add_client(sock,name);
            }
            else
                SDLNet_TCP_Close(sock);
		}


        std::string str = "";
		//---------------------------------------------------------------------
		// LOOP THROUGH CLIENTS
		//---------------------------------------------------------------------
		for(int i = 0; numready > 0 && i < num_clients; i++)
		{
			message = "";
            if (clients[i].active)
            {
                if(SDLNet_SocketReady(clients[i].sock))
                {
                    //---------------------------------------------------------
                    // GET DATA FROM CLIENT
                    //---------------------------------------------------------
                    message = recv_message(clients[i].sock);
                    //std::cout << "message: " << message << std::endl;
                    if(message > "")
                    {					
                        //std::cout << "<" << clients[i].name << ">" << " " << message << std::endl;
                        update_position(i, message);
                        
                    }
                    numready--;
                }
            }
        }
        str += calculate_positions();
        str += "#";
      
        send_all(str);
	}
    
	/* shutdown SDL_net */
	SDLNet_Quit();

	/* shutdown SDL */
	SDL_Quit();

	return(0);
}

