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

const float PI = 3.14159265;
const int MAP_WIDTH = 5000;
const int MAP_HEIGHT = 5000;

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


class Bullet 
{

public:
	Bullet();
	Bullet(float x1, float y1, float dir_x1 = 0, float dir_y1 = 0, float w1 = 2, float h1 = 2, bool a = 0)
		: x(x1), y(y1), w(w1), h(h1), is_alive(a), dir_x(dir_x1), dir_y(dir_y1)
	{
	
	}

	~Bullet();

	void draw(Surface &);

	float x, y, w, h;
	float dir_x, dir_y;
	bool is_alive;

};

void Bullet::draw(Surface & surface) 
{
	if (is_alive == true)
		surface.put_rect(x, y, w, h, 255, 0, 0);
}


class Player 
{

public:
	Player(float x1, float y1, int num = -1, int r1 = 0)
		: x(x1), y(y1), number(num), r(r1)
	{
		bullet = new Bullet(x, y);
	}

	void draw(Surface &);
	void draw_bullet(Surface &);

	float x, y;
	int number;
	int r;
	Bullet * bullet;
	//int num_bullets;
};

void Player::draw(Surface & surface) 
{
	std::string sprite_path = "images/newships/";
	sprite_path += itos(number);
	sprite_path += ".png";

	Image sprite(sprite_path.c_str());
	
	Rect display(W/2 - 25, H/2 - 25, 50, 50);
	Rect source((r/10)*50, 0,50,50);

	surface.put_image(sprite, source, display);
}

void Player::draw_bullet(Surface & surface) 
{
	bullet->draw(surface);
}

std::vector<Player> players;

void bullet_collision_with_wall(Player & player)
{
	// left wall
	if (player.bullet->x < 0) {
		player.bullet->is_alive = 0;
	}

	// right wall
	if (player.bullet->x > W) {
		player.bullet->is_alive = 0;
	}

	// top wall
	if (player.bullet->y < 0) {
		player.bullet->is_alive = 0;
	}

	// bottom wall
	if (player.bullet->y > H) {
		player.bullet->is_alive = 0;
	}
}

int main(int argc, char **argv)
{
	Surface surface(W, H);
	Event event;

	Image background_image("images/map/map-01.png");
	Rect  background_display = background_image.getRect();
	Rect  background_source;
	background_source.x = 0;
	background_source.y = 0;
	background_source.w = W;
	background_source.h = H;


	Rect mini_map(W - 50, H - 50, 50, 50);
	Rect p1_mini_map(W - 50, H - 50, 2, 2);

	Player player(W/2 - 25, H/2 - 25, 0);

	players.push_back(player);

	while(1)
	{
		float x_pos = 10 * cos((PI*(players[0].r + 90)) / 180);
		float y_pos = 10 * sin((PI*(players[0].r + 90)) / 180);

		if (players[0].bullet->is_alive == 0)
		{
			players[0].bullet->x = W/2 + 1 * cos((PI*(players[0].r + 90)) / 180);
			players[0].bullet->y = H/2 - 1 * sin((PI*(players[0].r + 90)) / 180);
		}
		else {
			players[0].bullet->x += players[0].bullet->dir_x;
			players[0].bullet->y -= players[0].bullet->dir_y;
		}

		if (event.poll() && event.type() == QUIT) break;

		KeyPressed keypressed = get_keypressed();

		if (keypressed[UPARROW]) 
		{
			players[0].x += x_pos;
			players[0].y -= y_pos;

			background_display.x += x_pos;
			background_display.y -= y_pos;

			// GLOBAL POSITION
			if (players[0].y <= 0) 
			{
				players[0].y = 0;
			}
			else if (players[0].y + 50 >= MAP_HEIGHT) 
			{
				players[0].y = MAP_HEIGHT - 50;
			}
			
			if (players[0].x <= 0) 
			{
				players[0].x = 0;
			}
			else if (players[0].x + 50 >= MAP_WIDTH) 
			{
				players[0].x = MAP_WIDTH - 50;
			}


			if (background_display.y <= 0) 
			{
				background_display.y = 0;
			}
			else if (background_display.y + H >= MAP_HEIGHT) 
			{
				background_display.y = MAP_HEIGHT - H;
			}
			
			if (background_display.x <= 0) 
			{
				background_display.x = 0;
			}
			else if (background_display.x + W >= MAP_WIDTH) 
			{
				background_display.x = MAP_WIDTH - W;
			}
		}
		else if (keypressed[LEFTARROW])
		{
			players[0].r += 10;
			if (players[0].r > 350) 
			{
				players[0].r = 0;
			}
		}
		else if (keypressed[RIGHTARROW]) 
		{
			players[0].r -= 10;
			if (players[0].r < 0)
			{
				players[0].r = 350;
			}
		}
		else if (keypressed[SPACE]) 
		{
			if (players[0].bullet->is_alive == 0) 
			{
				players[0].bullet->is_alive = 1;
				players[0].bullet->dir_x = x_pos;
				players[0].bullet->dir_y = y_pos;
			}
		}

		//-------------------------------------------------------------------------------
		// Collisions 
		//-------------------------------------------------------------------------------
		bullet_collision_with_wall(players[0]);
		// bullet_collision_with_player();
		// player_collision_with_player();


		//-------------------------------------------------------------------------------
		// Mini Map 
		//-------------------------------------------------------------------------------

		// update player position on mini map
		p1_mini_map.x = mini_map.x + (players[0].x / 100);
		p1_mini_map.y = mini_map.y + (players[0].y / 100);


		//-------------------------------------------------------------------------------
		// GET DATA FROM SERVER
		//-------------------------------------------------------------------------------
		surface.fill(RED);
		surface.put_image(background_image, background_display, background_source);
		for (int i = 0; i < players.size(); i++) 
		{
			surface.lock();			
			players[i].draw_bullet(surface);
			players[i].draw(surface);
			surface.unlock();
		}

		surface.put_rect(mini_map, WHITE);
		surface.put_rect(p1_mini_map, GREEN);

		surface.flip();


		delay(10);
	}

	SDLNet_Quit();
	SDL_Quit();

	return(0);
}
