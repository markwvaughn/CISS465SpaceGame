/**
 * This is a very simple example with SDL_net. There is no error checking atm.
 * Run client.exe or telnet 127.0.0.1 1234 (local client) to test
 **/

#include <iostream>
#include <cmath>
#include <vector>
#include <string>
#include <fstream>
#include <ctime>
#include <cmath>
#include <cstdlib>
#include <sstream>
#include <string> 
#include "Includes.h"
#include "Constants.h"
#include "compgeom.h"
#include "Surface.h"
#include "Event.h"
#include "SDL_net.h"

#include <stdio.h>
#include <stdlib.h>

const int MAXLEN = 1024;

struct Client {
	TCPsocket sock;
	std::string name;
	float x, y;
};

int running=1;
std::vector<Client> clients;
int num_clients=0;
TCPsocket server;

void send_client(int, std::string);
void send_all(std::string buf);
int find_client_name(std::string name);

std::string ftos(float f) {
	std::ostringstream buff;
	buff << f;

	return buff.str();
}

std::string itos(int i) {
	std::ostringstream buff;
	buff << i;

	return buff.str();
}

std::string recv_message(TCPsocket sock) {

    char buff[MAXLEN] = {' '};
    SDLNet_TCP_Recv(sock, buff, MAXLEN);

    // std::cout << "buffer: " << buff << std::endl;
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

int unique_nick(std::string s)
{
	return(find_client_name(s)==-1);
}

void add_client(TCPsocket sock, std::string name)
{
	if(!name.length())
	{
		send_message("Invalid Nickname...bye bye!", sock);
		SDLNet_TCP_Close(sock);
		return;
	}
	if(!unique_nick(name))
	{
		send_message("Duplicate Nickname...bye bye!", sock);
		SDLNet_TCP_Close(sock);
		return;
	}
	
	std::cout << "inside add client" << std::endl;
	std::cout << "num clients: " << num_clients << std::endl;

	Client c;

	c.name=name;
	c.sock=sock;
	c.x= rand() % W;
	c.y= rand() % H;

	clients.push_back(c);

	num_clients++;

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
	for(int i=0;i<num_clients;i++)
		if(clients[i].sock==sock)
			return(i);
	return(-1);
}

/* find a client in our array of clients by it's socket. */
/* the name is always unique */
int find_client_name(std::string name)
{
	for(int i=0; i < num_clients;i++) {
		if (clients[i].name == name)
			return i;
	}
		
	return -1;
}

/* remove a client from our array of clients */
void remove_client(int i)
{
	std::string name=clients[i].name;

	if(i<0 && i>=num_clients)
		return;
	
	/* close the old socket, even if it's dead... */
	SDLNet_TCP_Close(clients[i].sock);
	
	num_clients--;
	clients.erase(clients.begin() + i);
	
	/* server side info */
	std::cout << "<-- " << name << std::endl;
	/* inform all clients, excluding the old one, of the disconnected user */
	//send_all(mformat("ss","<-- ",name.c_str()));
	
}

/* create a socket set that has the server socket and all the client sockets */
SDLNet_SocketSet create_sockset()
{
	static SDLNet_SocketSet set=NULL;

	if(set)
		SDLNet_FreeSocketSet(set);
	set=SDLNet_AllocSocketSet(num_clients+1);
	if(!set) {
		printf("SDLNet_AllocSocketSet: %s\n", SDLNet_GetError());
		exit(1); /*most of the time this is a major error, but do what you want. */
	}
	SDLNet_TCP_AddSocket(set,server);
	for(int i=0;i<num_clients;i++)
		SDLNet_TCP_AddSocket(set,clients[i].sock);
	return(set);
}

/* send a buffer to all clients */
void send_all(std::string buf)
{
	int cindex;

	if(buf == "" || !num_clients)
		return;
	cindex=0;
	while(cindex<num_clients)
	{
		if(send_message(buf, clients[cindex].sock))
			cindex++;
		else
			remove_client(cindex);
	}
}

void send_client(int i, std::string buf) {

	if (buf == "") {
		return;
	}

	send_message(buf, clients[i].sock);
}

void who_command(Client *client) {

	int i;
	IPaddress *ipaddr;
	Uint32 ip;
	const char *host=NULL;
	
	send_message("--- Begin /WHO ", client->sock);
	for(i=0;i<num_clients;i++)
	{
		ipaddr=SDLNet_TCP_GetPeerAddress(clients[i].sock);
		if(ipaddr)
		{
			// std::string info(mformat("sssssdsdsdsdsd","--- ",clients[i].name.c_str(),
			// 		" ",host?host:"",
			// 		"[",ip>>24,".", (ip>>16)&0xff,".", (ip>>8)&0xff,".", ip&0xff,
			// 		"] port ",(Uint32)ipaddr->port));

			// ip=SDL_SwapBE32(ipaddr->host);
			// host=SDLNet_ResolveIP(ipaddr);
			// send_message(info, client->sock);
		}
	}

	send_message("--- End /WHO", client->sock);
	return;
}

std::string format_pos_string(int i) {
	
	std::string str="p";
	str += itos(i);
	str += ":";
	str += ftos(clients[i].x);
	str += ",";
	str += ftos(clients[i].y);
	str += ";";

	return str;
}

std::string update_position(int i, std::string message) {

	// interpret message
	if (message == "1") {

		clients[i].y -= 0.5f;
	}
	else if (message == "2") {

		clients[i].y += 0.5f;
	}
	else if (message == "3") {

		clients[i].x -= 0.5f;
	}
	else if (message == "4") {

		clients[i].x += 0.5f;
	}

	std::string str = format_pos_string(i);
	return str;

}

std::string other_positions(int p) {

	std::string ret;
	for (int i = 0; i < num_clients; i++) {

		if (i != p) {
			std::string str = format_pos_string(i);
			ret += str;
		}
	}

	return ret;

}


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

	/* perform a byte endianess correction for the next printf */
	ipaddr=SDL_SwapBE32(ip.host);

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

	std::cout << "H: " << host << std::endl;
	std::cout << "S: " << server << std::endl;
	std::cout << "I: " << ipaddr << std::endl;
	std::cout << "P: " << port << std::endl;


	while(1)
	{
		int numready;
		set=create_sockset();
		numready=SDLNet_CheckSockets(set, (Uint32)-1);
		if(numready==-1)
		{
			std::cout << "SDLNet_CheckSockets ERROR" << std::endl;
			break;
		}
		if(!numready)
			continue;
		if(SDLNet_SocketReady(server))
		{
			numready--;

			sock=SDLNet_TCP_Accept(server);
			if(sock)
			{
				std::string name;
				std::cout << sock << std::endl;
				name = recv_message(sock);

				if(name > "")
				{
					std::cout << "name: " << name << std::endl;

					add_client(sock,name);
				}
				else
					SDLNet_TCP_Close(sock);
			}
		}

		//std::cout  << "client loop" << std::endl;
		//-------------------------------------------------------------------------------
		// LOOP THROUGH CLIENTS
		//-------------------------------------------------------------------------------
		for(int i=0; numready && i<num_clients; i++)
		{
			message = "";
			std::string str = "";
			if(SDLNet_SocketReady(clients[i].sock))
			{
				//-----------------------------------------------------------------------
				// GET DATA FROM CLIENT
				//-----------------------------------------------------------------------
				message = recv_message(clients[i].sock);
				//std::cout << "message: " << message << std::endl;
				if(message > "")
				{					
					numready--;
					//std::cout << "<" << clients[i].name << ">" << " " << message << std::endl;
					str += update_position(i, message);

				}
			}

			str += other_positions(i);
			str += "#";

			//std::cout << "message to client: " << str << std::endl;

			if (str[0] == 'p') {
				send_all(str);
			}

		}
	}
	/* shutdown SDL_net */
	SDLNet_Quit();

	/* shutdown SDL */
	SDL_Quit();

	return(0);
}

