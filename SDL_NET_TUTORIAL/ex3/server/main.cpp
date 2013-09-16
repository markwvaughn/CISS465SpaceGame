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

// #include <string.h>
// #include <stdarg.h>
// #include <stdlib.h>
// #include <unistd.h>

#include <stdio.h>
#include <stdlib.h>

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

struct Client {
	TCPsocket sock;
	std::string name;
	int x, y;
};

int running=1;
Client *clients=NULL;
int num_clients=0;
TCPsocket server;

void send_all(std::string buf);
int find_client_name(std::string name);

std::string mformat(char *format,...)
{
	va_list ap;
	Uint32 len=0;
	static char *str=NULL;
	char *p, *s;
	char c;
	int d;
	unsigned int u;

	if(str)
	{
		free(str);
		str=NULL;
	}
	if(!format)
		return(NULL);
	va_start(ap,format);
	for(p=format; *p; p++)
	{
		switch(*p)
		{
			case 's': /* string */
				s=va_arg(ap, char*);
				str=(char*)realloc(str,((len+strlen(s)+4)/4)*4);
				sprintf(str+len,"%s",s);
				break;
			case 'c': /* char */
				c=(char)va_arg(ap, int);
				str=(char*)realloc(str,len+4);
				sprintf(str+len,"%c",c);
				break;
			case 'd': /* int */
				d=va_arg(ap, int);
				str=(char*)realloc(str,((len+64)/4)*4);
				sprintf(str+len,"%d",d);
				break;
			case 'u': /* unsigned int */
				u=va_arg(ap, unsigned int);
				str=(char*)realloc(str,((len+64)/4)*4);
				sprintf(str+len,"%u",u);
				break;
		}
		/* set len to the new string length */
		if(str)
			len=strlen(str);
		else
			len=0;
	}
	va_end(ap);

	std::string ret(str);

	return ret;
}

/* test for nice name uniqueness among already connected users */
int unique_nick(std::string s)
{
	return(find_client_name(s)==-1);
}

/* add a client into our array of clients */
Client *add_client(TCPsocket sock, std::string name)
{
	if(!name.length())
	{
		send_message("Invalid Nickname...bye bye!", sock);
		SDLNet_TCP_Close(sock);
		return(NULL);
	}
	if(!unique_nick(name))
	{
		send_message("Duplicate Nickname...bye bye!", sock);
		SDLNet_TCP_Close(sock);
		return(NULL);
	}
	
	clients=(Client*)realloc(clients, (num_clients+1)*sizeof(Client));
	clients[num_clients].name=name;
	clients[num_clients].sock=sock;
	clients[num_clients].x=0;
	clients[num_clients].y=0;

	num_clients++;
	/* server side info */
	std::cout << "--> " << name << std::endl;
	/* inform all clients, including the new one, of the joined user */
	send_all(mformat("ss","--> ",name.c_str()));
	return(&clients[num_clients-1]);
}

/* find a client in our array of clients by it's socket. */
/* the socket is always unique */
int find_client(TCPsocket sock)
{
	int i;
	for(i=0;i<num_clients;i++)
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
	if(num_clients>i)
		memmove(&clients[i], &clients[i+1], (num_clients-i)*sizeof(Client));
	
	clients=(Client*)realloc(clients, num_clients*sizeof(Client));
	
	/* server side info */
	std::cout << "<-- " << name << std::endl;
	/* inform all clients, excluding the old one, of the disconnected user */
	
	send_all(mformat("ss","<-- ",name.c_str()));
	
}

/* create a socket set that has the server socket and all the client sockets */
SDLNet_SocketSet create_sockset()
{
	static SDLNet_SocketSet set=NULL;
	int i;

	if(set)
		SDLNet_FreeSocketSet(set);
	set=SDLNet_AllocSocketSet(num_clients+1);
	if(!set) {
		printf("SDLNet_AllocSocketSet: %s\n", SDLNet_GetError());
		exit(1); /*most of the time this is a major error, but do what you want. */
	}
	SDLNet_TCP_AddSocket(set,server);
	for(i=0;i<num_clients;i++)
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
		/* putMsg is in tcputil.h, it sends a buffer over a socket */
		/* with error checking */
		if(send_message(buf, clients[cindex].sock))
			cindex++;
		else
			remove_client(cindex);
	}
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
			std::string info(mformat("sssssdsdsdsdsd","--- ",clients[i].name.c_str(),
					" ",host?host:"",
					"[",ip>>24,".", (ip>>16)&0xff,".", (ip>>8)&0xff,".", ip&0xff,
					"] port ",(Uint32)ipaddr->port));

			ip=SDL_SwapBE32(ipaddr->host);
			host=SDLNet_ResolveIP(ipaddr);
			send_message(info, client->sock);
		}
	}

	send_message("--- End /WHO", client->sock);
	return;
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
	
	//int p1x = 0, p1y = 0;

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
		//printf("SDLNet_ResolveHost: %s\n",SDLNet_GetError());
		std::cout << "ERROR" << std::endl;
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
		//printf("SDLNet_TCP_Open: %s\n",SDLNet_GetError());
		std::cout << "ERROR" << std::endl;
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
			//printf("SDLNet_CheckSockets: %s\n",SDLNet_GetError());
			std::cout << "ERROR" << std::endl;
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
					Client *client;
					client=add_client(sock,name);
					if(client)
						who_command(client);
				}
				else
					SDLNet_TCP_Close(sock);
			}
		}
		//-------------------------------------------------------------------------------
		// LOOP THROUGH CLIENTS
		//-------------------------------------------------------------------------------
		for(int i=0; numready && i<num_clients; i++)
		{
			if(SDLNet_SocketReady(clients[i].sock))
			{
				//-----------------------------------------------------------------------
				// GET DATA FROM CLIENT
				//-----------------------------------------------------------------------
				message = "";
				message = recv_message(clients[i].sock);

				if(message > "")
				{
					std::string str;
					
					numready--;
					std::cout << "<" << clients[i].name << ">" << " " << message;
					
					// interpret message
					if (message == "1") {

						clients[i].y--;
					}
					else if (message == "2") {

						clients[i].y++;
					}
					else if (message == "3") {

						clients[i].x--;
					}
					else if (message == "4") {

						clients[i].x++;
					}
		
					str=mformat("cdcdcc",'p',clients[i].x,',',clients[i].y,';','#');
					if(str > "")
						send_all(str);				
				}
				else
					remove_client(i);
			}
		}
	}

	/* shutdown SDL_net */
	SDLNet_Quit();

	/* shutdown SDL */
	SDL_Quit();

	return(0);
}

