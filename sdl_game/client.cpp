/******************************************************************************
 * This is the client side of the Space Shooter project for CISS465.
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
 * Global Constants.
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
enum GameStates
{
    GAME_NULL, // Nothing. Keep the current state. Actually not used right now.
    GAME_INTRO, // Intro sequence.
    GAME_TITLE, // Splash screen.
    GAME_IDLE, // Player is idling on title screen too long.
    GAME_REGISTRATION, // User is trying to register for the first time.
    GAME_LOGIN_SCREEN, // User is trying to login.
    GAME_LOGIN_FEEDBACK,
    GAME_RUNNING, // Primary state, game is on.
    GAME_OVER, // Somebody has died too much or whatever other game over seq.
    GAME_DISCONNECT, // Server not found????
    GAME_EXIT // Player just quit.
};


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
	sprite_path += to_str(id);
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
GameStates GameState;

std::vector<Image> gallery; // Gallery

// Network stuff
IPaddress ip;
Uint16 port;
TCPsocket sock;
int numready;
SDLNet_SocketSet set;	

//SDL_Thread *net_thread=NULL, *local_thread=NULL;


/******************************************************************************
 * Helper Functions
 *****************************************************************************/
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


/******************************************************************************
 * Core Functions
 *****************************************************************************/

// Wannabe cool intro
void intro(Surface & surface, Event & event, Font & font)
{
    // This text can be modified, serves only to present a message for now.
    DynamicText welcome(font, "Mark and Ujjwal present ...", GREEN);

    // Load the fancy background iamge and set up camera for it.
    Image background("images/map/bg_intro.png");
    Rect camera = background.getRect();
    SDL_Rect screen = {0, 0, W, H};

    // Get time so that we can time updates to the message etc.
    int start = getTicks(), current = 0;
    int dx = 20;
    int dy = 16;
    while (1)
    {
        if (event.poll())
        {
            switch (event.type())
            {
                // Player Quit -> Exit game
                case QUIT:
                    GameState = GAME_EXIT;
                    goto EXIT_INTRO;
                // Any key pressed -> Title Splash Screen
                case SDL_KEYDOWN:
                    GameState = GAME_TITLE;
                    goto EXIT_INTRO;
            }
        }

        // Get current time and compare with the start time.
        current = getTicks() - start;

        // Just fancy nonsense to move around the map.
        camera.x >= 0 && camera.x + W <= camera.w ? dx : dx = -dx;
        camera.x += dx;

        camera.y >= 0 && camera.y + H <= camera.h ? dy : dy = -dy;
        camera.y += dy;

        // Do different things based on how much time has passed.
        if (current >= 8000)
        {
            GameState = GAME_TITLE;
            goto EXIT_INTRO;
        }
        else if (current >= 6000)
        {
            welcome.set_text(font, "SHOOT or Get shot.");
            welcome.set_color(RED);
            welcome.set_x(W / 4);
            welcome.set_y(H / 4);
        }
        else if (current >= 4000)
        {
            welcome.set_text(font, "A game set in cold space ...");
            welcome.set_color(WHITE);
            welcome.set_x(W / 6);
            welcome.set_y(H / 6);
        }        
        else if (current >= 2000)
        {
            welcome.set_text(font, "On Behalf of Dr. Yihsiang Liow ...");
            welcome.set_color(CYAN);
            welcome.set_x(W / 10);
            welcome.set_y(H / 10);
        }
        
        surface.lock();
        surface.fill(BLACK);
        surface.put_image(background, camera, screen); 
        welcome.draw(surface);
        surface.unlock();
        surface.flip();
		delay(10); // yield 10 milliseconds to other programs
    }
EXIT_INTRO:
    return;
}
                    

// The Splash Screen.
void title(Surface & surface, Event & event, Font & font)
{
    std::string prompt_str = "Press any key to start.";

    DynamicText welcome(font, prompt_str, WHITE);

    welcome.set_x(W / 3);
    welcome.set_y(H - H / 6);

    // Load the fancy background image and set up camera for it.
    Image background("images/splash/SpaceShootout.png");
    SDL_Rect screen = {0, 0, W, H};
    
    int start = getTicks(), current = 0;
    while (1)
    {
        if (event.poll())
        {
            switch (event.type())
            {
                case QUIT:
                    GameState = GAME_EXIT;
                    goto EXIT_TITLE;
                case SDL_KEYDOWN:
                    GameState = GAME_LOGIN_SCREEN;
                    goto EXIT_TITLE;
            }
        }

        current = getTicks() - start;
        
        if (current >= 10000)
        {
            GameState = GAME_IDLE;
            surface.fill(BLACK);
            surface.flip();
            goto EXIT_TITLE;
        }
        else if (current >= 8000)
        {
            welcome.set_color(RED);
            welcome.set_text(font, prompt_str);
        }
        else if (current >= 7000)
            welcome.set_text(font, " ");
        else if (current >= 6000)
        {
            welcome.set_color(ORANGE);
            welcome.set_text(font, prompt_str);
        }
        else if (current >= 5000)
            welcome.set_text(font, " ");
        else if (current >= 4000)
        {
            welcome.set_color(GREEN);
            welcome.set_text(font, prompt_str);
        }
        else if (current >= 3000)
            welcome.set_text(font, " ");
        else if (current >= 2000)
        {
            welcome.set_color(BLUE);
            welcome.set_text(font, prompt_str);
        }
        else if (current >= 1000)
            welcome.set_text(font, " ");
        
        surface.lock();
        surface.fill(BLACK);
        surface.put_image(background, screen);
        welcome.draw(surface);
        surface.unlock();
        surface.flip();
		delay(10); // yield 10 milliseconds to other programs
    }
EXIT_TITLE:
    return;
}


// Gallery images are displayed on IDLE
void idle(Surface & surface, Event & event, Font & font)
{
    int bgindex = rand() % 18 + 1;
    
    // Load the fancy background image and set up camera for it.
    Image background = gallery[bgindex];
    Rect screen = background.getRect();
    SDL_Surface * temp_surface = surface.get();
    temp_surface = SDL_SetVideoMode(screen.w, screen.h, BPP, SDL_FULLSCREEN);
    
    int start = getTicks(), current = 0;
    while (1)
    {
        if (event.poll())
        {
            switch (event.type())
            {
                case QUIT:
                    GameState = GAME_EXIT;
                    goto EXIT_IDLE;
                case SDL_KEYDOWN:
                    GameState = GAME_TITLE;
                    goto EXIT_IDLE;
            }
        }

        current = getTicks() - start;
        
        if (current >= 60000)
        {
            GameState = GAME_INTRO;
            surface.fill(BLACK);
            surface.flip();
            goto EXIT_IDLE_WITH_RESET;
        }
        else if (current >= 50000)
        {
            GameState = GAME_IDLE;
            goto EXIT_IDLE;
        }
        else if (current >= 40000)
        {
            GameState = GAME_IDLE;
            goto EXIT_IDLE;
        }
        else if (current >= 30000)
        {
            GameState = GAME_IDLE;
            goto EXIT_IDLE;
        }
        else if (current >= 20000)
        {
            GameState = GAME_IDLE;
            goto EXIT_IDLE;
        }
        else if (current >= 10000)
        {
            GameState = GAME_IDLE;
            goto EXIT_IDLE;
        }
        surface.lock();
        surface.fill(BLACK);
        surface.put_image(background, screen);
        surface.unlock();
        surface.flip();
		delay(10); // yield 10 milliseconds to other programs
    }
EXIT_IDLE:
    return;
EXIT_IDLE_WITH_RESET:
    temp_surface = SDL_SetVideoMode(W, H, BPP, SDL_ANYFORMAT);
    return;
}


// TODO: Write a simple widget class for this kind of stuff.

// Lets the user register.
void register_screen(Surface & surface, Event & event, Font & font,
                     std::string & user, std::string & pw)
{
    // TODO???
}


// Prompts user for username and password.
void login_screen(Surface & surface, Event & event, Font & font,
                  std::string & user, std::string & pw)
{
    // Load the fancy background image and set up camera for it.
    Image background("images/map/bg_login.jpg");
    SDL_Rect screen = {0, 0, W, H};
    
    Image welcome(font.render("Welcome to Space Shootout.", BLUE));
    Rect welcome_rect = welcome.getRect();
    welcome_rect.x = W / 10;
    welcome_rect.y = H / 5;

    Image prompt_name = Image(font.render("Username: ", RED));
    Rect prompt_name_rect = prompt_name.getRect();
    prompt_name_rect.x = welcome_rect.x;
    prompt_name_rect.y = welcome_rect.y + H / 10;

    Image prompt_pw = Image(font.render("Password: ", RED));
    Rect prompt_pw_rect = prompt_pw.getRect();
    prompt_pw_rect.x = prompt_name_rect.x;
    prompt_pw_rect.y = prompt_name_rect.y + H / 10;

    TextInput username(font, " ", WHITE,
                       prompt_name_rect.x + prompt_name_rect.w + W / 40,
                       prompt_name_rect.y);
    TextInput password(font, " ", WHITE,
                       prompt_pw_rect.x + prompt_pw_rect.w + W / 40,
                       prompt_pw_rect.y);

    DynamicText pwlen = password.get_text();

    while (1)
	{
        if (!(username.active() || password.active()))
        {
            GameState = GAME_LOGIN_FEEDBACK;
            goto EXIT_LOGIN_SCREEN;
        }
        if (event.poll())
        {
            switch (event.type())
            {
                case QUIT:
                    GameState = GAME_EXIT;
                    goto EXIT_LOGIN_SCREEN;
                case SDL_KEYDOWN:
                    if (username.active())
                        username.handle_input(event, font);
                    else if (password.active())
                        password.handle_input(event, font);
                    break;
            }
        }

        pw = password.get_text().get_text();
        pw == " " ? pw : pw.replace(pw.begin(), pw.end(), pw.length(), '*');
        pwlen.set_text(font, pw);
        surface.lock();
        surface.fill(BLACK);
        surface.put_image(background, screen);
        surface.put_image(welcome, welcome_rect);
        surface.put_image(prompt_name, prompt_name_rect);
        surface.put_image(prompt_pw, prompt_pw_rect);
        username.draw(surface);
        pwlen.draw(surface);    
        surface.unlock();
        surface.flip();
		delay(10); // yield 10 milliseconds to other programs
	}
EXIT_LOGIN_SCREEN:
    user = username.get_text().get_text();
    pw = password.get_text().get_text();
    
    return;
}


// Feedback on login.
void login_feedback(Surface & surface, Event & event, Font & font,
                    std::string & user, std::string & pw)
{
    DynamicText welcome(font);
    
    std::string message = user + ':' + pw;

    send_message(message, sock);

    message = recv_message(sock);

    std::cout << message << std::endl;

    if (message[0] != 'N')
    {
        welcome.set_color(RED);
        welcome.set_text(font, message);
        GameState = GAME_LOGIN_SCREEN;
    }
    else
    {
    	recv_player_number(message);
        message = "Logged in as player # " + to_str(player_number);
        welcome.set_color(CYAN);
        welcome.set_text(font, message);
        GameState = GAME_RUNNING;
    }
   
    welcome.set_x(W / 4);
    welcome.set_y(H / 3);
    
    // Load the fancy background image and set up camera for it.
    Image background("images/map/bg_feedback.png");
    SDL_Rect screen = {0, 0, W, H};
    
    int start = getTicks(), current = 0;
    while (1)
    {
        if (event.poll() && event.type() == QUIT)
        {
            GameState = GAME_EXIT;
            
            goto EXIT_LOGIN_FEEDBACK;
        }

        current = getTicks() - start;
        
        if (current >= 3000)           
            goto EXIT_LOGIN_FEEDBACK;
        
        surface.lock();
        surface.fill(BLACK);
        surface.put_image(background, screen);
        welcome.draw(surface);
        surface.unlock();
        surface.flip();
		delay(10); // yield 10 milliseconds to other programs
    }
EXIT_LOGIN_FEEDBACK:
    return;
}


void init(const char * hostaddr)
{
    // Buffers for communication with server
	static std::string to_server;
	static std::string from_server;

	/* initialize SDL */
    if (SDL_Init(SDL_INIT_EVERYTHING) == -1)
    {
        std::cerr << "SDL_Init ERROR: " << SDL_GetError() << std:: endl;
        
        exit(0);
    }

	/* initialize SDL_net */
	if(SDLNet_Init() == -1)
	{
		std::cerr << "SDLNet_Init ERROR: " << SDLNet_GetError() << std::endl;
        
		SDL_Quit();

        exit(1);
	}

	set = SDLNet_AllocSocketSet(1);
	if(!set)
	{
		std::cerr << "SDLNet_AllocSocketSet ERROR: "
                  << SDLNet_GetError() << std::endl;

        SDLNet_Quit();
        SDL_Quit();

        exit(2);
	}
	
	/* Resolve the argument into an IPaddress type */
	std::cout << "connecting to " << hostaddr << " port " << port << std::endl;
	if(SDLNet_ResolveHost(&ip, hostaddr, port) == -1)
	{
		std::cerr << "SDLNet_ResolveHost ERROR: "
                  << SDLNet_GetError() << std::endl;

        SDLNet_Quit();
		SDL_Quit();

        exit(3);
	}

	/* open the server socket */
	sock=SDLNet_TCP_Open(&ip);
	if(!sock)
	{
		std::cerr << "SDLNet_TCP_Open ERROR: "
                  << SDLNet_GetError() << std::endl;

        SDLNet_Quit();
		SDL_Quit();

        exit(4);
	}
	
	if(SDLNet_TCP_AddSocket(set, sock) == -1)
	{
		std::cerr << "SDLNet_TCP_AddSocket ERROR: "
                  << SDLNet_GetError() << std::endl;

		SDLNet_Quit();
		SDL_Quit();

        exit(5);
	}
}


// primary game loop.
void game(Surface & surface, Event & event, Font & font,
          const std::string &user)
{
    std::string greeting = "Hello " + user + ", welcome!";
    Image * welcome = new Image(font.render(greeting.c_str(), GREEN));
    Rect welcome_rect = welcome->getRect();
    welcome_rect.x = W / 10;
    welcome_rect.y = H / 5;

    while (1)
	{
        if (event.poll())
        {
            switch(event.type())
            {
                case QUIT:
                    GameState = GAME_EXIT;
                    goto EXIT_GREET;
                case SDL_KEYDOWN:
                    delete welcome;
                    welcome = new Image(font.render("Lolpls", BLUE));
            }
        }
        
        surface.lock();
        surface.fill(BLACK);
        surface.put_image(*welcome, welcome_rect);
        surface.unlock();
        surface.flip();
		delay(10); // yield 10 milliseconds to other programs
	}
EXIT_GREET:
    
    return;
}



/******************************************************************************
 * Point of Entry!!!
 *****************************************************************************/
int main(int argc, char* argv[])
{
    /* check our commandline */
	if(argc < 3)
	{
		std::cout << "Usage: " << argv[0] << " host_ip host_port" << std::endl;

        return 0;
	}

    /* get the port from the commandline */
	port = (Uint16)strtol(argv[2],NULL,0);
    const char * hostaddr = argv[1];
    
    init(hostaddr);

    // Load Gallery images
    std::string galpath; 
    for (int i = 1; i < 19; i++)
    {
        galpath = "images/gallery/" + to_str(i) + ".jpg";
        gallery.push_back(galpath.c_str());
    }
    
    Surface surface(W, H);	// W = 640, H = 480 are constants. This creates
                            // a drawing surface of size W-by-H

    SDL_WM_SetCaption("Space Shootout", NULL);

	Event event; // event is an Event object. It's used to process user
                 // events such as mouse clicks, key presses, etc.

    Font font("fonts/FreeSans.ttf", 32);
    Font title_font("fonts/FreeSerif.ttf", 24);

    SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);

    std::string user;
    std::string pw;

    GameState = GAME_INTRO;

    // Start the timer to maintain constant FPS.
    int start = getTicks(), end = 0, dt = 0;
    while (1)
    {
        switch (GameState)
        {
            case GAME_EXIT:
                goto EXIT_GAME;
            case GAME_INTRO:
                intro(surface, event, font);
                break;
            case GAME_TITLE:
                title(surface, event, title_font);
                delay(1000);
                break;
            case GAME_IDLE:
                idle(surface, event, font);
                break;
            case GAME_LOGIN_SCREEN:
                login_screen(surface, event, font, user, pw);
                break;
            case GAME_LOGIN_FEEDBACK:
                login_feedback(surface, event, font, user, pw);
                break;
            case GAME_RUNNING:
                game(surface, event, font, user);
                break;
        }
    }
    end = getTicks();
    dt = FPS - end + start;
    if (dt > 0) delay(dt);
EXIT_GAME:
    std::cout << "Bye! " << user << " with pw: " << pw << std::endl;
    
	return 0;	
}
