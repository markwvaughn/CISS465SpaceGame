#ifndef GAMEOBJECTS_H
#define GAMEOBJECTS_H

class GameObject
{
public:
    virtual void draw(Surface & surface);
    virtual void do_damage(GameObject & GO);
    virtual bool is_there_collision(GameObject & GO);
    virtual void collision_resolution(GameObject & GO);

    int id;
    int x, y, w, h;
    int t;
    int state;
    int health;
    int power;
    int defense;
    int speed;
    Color color;
};

class Bullet : public GameObject
{
public:
	inline void draw(Surface & surface)
    {
        surface.put_rect(x, y, w, h, color);
    }

    inline void do_damage(GameObject & GO)
    {
        GO.health -= (power - GO.defense) * 100;
    }

    inline bool is_there_collision(GameObject & GO)
    {
        x 
};


void Bullet::draw(Surface & surface) 
{
	if (state == ACTIVE)
		
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

#endif
