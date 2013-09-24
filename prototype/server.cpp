/****************************************************************************
 Yihsiang Liow
 Copyright
 ****************************************************************************/
#include <iostream>
#include <cmath>
#include <vector>
#include <cstring>
#include <fstream>
#include <ctime>
#include <cmath>
#include <cstdlib>

#include "Includes.h"
#include "Constants.h"
#include "compgeom.h"


/*****************************************************************************
This is just a prototype of a space shooter kind of game.
*****************************************************************************/
void fancygreet(char * text);


// This is just a typedef for a buffer
typedef char Buffer[1024];

struct Network_event
{
    bool received;
};

// This is the buffer that we will send to the clients
Buffer to_client = "HELLO CLIENT!";

// This is the buffer that we will receive data into from the clients
Buffer from_client = "";

// This is used to see if something has been received
Network_event * ne;

/**
 * This just gets a message from the client
 **/
char * get_greetings_client()
{
    // Allocate memory for the server's ip
    IPaddress ip;

    // Resolve the ip for the server (i.e, localhost since we're the server).
    // NULL is used here because we want to be the server.
    // Essentially, this creates a listener on port 1234
    SDLNet_ResolveHost(&ip, NULL, 1234);
    
    // Create Server Socket (TCP) that listens for incoming connections
    TCPsocket server = SDLNet_TCP_Open(&ip);
    
    // This is the socket that will accept connections and transfer data
    TCPsocket client;
    
    // Keep listening for incoming connections till you receive "Quit"
    while(1)
    {
        // Attempt to accept incoming connection from client
        client = SDLNet_TCP_Accept(server);
        
        if (client)
        {
            // generate network received event
            ne = new Network_event;
            ne->received = true;

            SDL_UserEvent e;
            e.type = SDL_USEREVENT;
            SDL_Pushevent(&e);
            // the connection has been established and we can communicate
            
            // Send message to client
            SDLNet_TCP_Send(client, to_client, strlen(to_client) + 1);
            
            // Receive message from client
            SDLNet_TCP_Recv(client, from_client, 100);
            std::cout << from_client << std::endl;
            fancygreet(from_client);

            // Close the socket
            SDLNet_TCP_Close(client);

            // Quit
            if (!strcmp(from_client, "Quit"))
                break;
        }
    }

    // Close the socket. Must close to free up the port before reboot.
    SDLNet_TCP_Close(server);

    return from_client;
}


/**
 * This is just a demo of fancy text effects across a network
 **/
void fancygreet(char * text)
{
	Surface surface(W, H);
	Sound sound("sounds/laser.wav");
	Music music("sounds/GameLoop.ogg");
	music.play();
	Event event;

	Font font("fonts/FreeSans.ttf", 48);
	Image image(font.render(text, WHITE));
	Rect rect = image.getRect();

	int r = 0;
	int g = 0;
	int b = 255;
	int dr = 1;
	int dg = 2;
	int db = 3;
	Color c = {0, 100, 200};

	int dx = 2;
	int dy = 3;

	//sound.off();
	//music.off();
	int RATE = 1000 / 60;

	while (1)
	{
		if (event.poll() && event.type() == QUIT) break;
        if (event.poll() && event.type() == TCP_recv)
        {
            
        }

		int start = getTicks();

		r += dr;
		if (r < 0)
		{
			r = 0;
			dr = - dr;
		}
		else if (r > 255)
		{
			r = 255;
			dr = -dr;
		}

		g += dg;
		if (g < 0)
		{
			g = 0;
			dg = - dg;
		}
		else if (g > 255)
		{
			g = 255;
			dg = -dg;
		}

		db += db;
		if (b < 0)
		{
			b = 0;
			db = -db;
		}
		else if (b > 255)
		{
			b = 255;
			db = -db;
		}
		Color c = {r, g, b};
		Image image(font.render(text, c));

		rect.x += dx;
		if (rect.x < 0)
		{
			sound.play();
			rect.x = 0;
			dx = -dx;
		}
		else if (rect.x + rect.w > W - 1)
		{
			sound.play();
			rect.x = W - 1 - rect.w;
			dx = -dx;
		}

		rect.y += dy;
		if (rect.y < 0)
		{
			sound.play();
			rect.y = 0;
			dy = -dy;
		}
		else if (rect.y + rect.h > H - 1)
		{
			sound.play();
			rect.y = H - 1 - rect.h;
			dy = -dy;
		}

		surface.lock();
		surface.fill(BLACK);
		surface.put_image(image, rect);
		surface.unlock();
		surface.flip();

		int end = getTicks();
		int dt = RATE - end + start;
		if (dt > 0) delay(dt);
	}
	return;
}


/*****************************************************************************
For our programs involving graphics and sound, the template is this:

int main(int argc, char* argv[])
{
    ... PROGRAM ...

    return 0;
}

Our main() is made up of calling various functions. This is the first time you
are actually seeing the *code* of functions. Before this, you have been 
*using* functions, i.e. *calling* the functions.

For instance the first function call is

	test_event();

When you run this program, the program will execute a function call to 
test_event(). This means that the program will look for "text_event" and 
execute the code until it sees the statement "return". Executing return
will cause the program to go back to where it came from.

After running the program, comment out the call to test_event(), uncomment
the call to test_pixel() and run the program. Etc.
*****************************************************************************/
int main(int argc, char* argv[])
{
    SDLNet_Init();
    
    get_greetings_client();
    
	return 0;	
}
