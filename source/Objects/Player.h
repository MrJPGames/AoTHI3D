#ifndef Player_H
#define Player_H

class Player{
	public:
		Player();
		Player(int x, int y);
		void setPos(int x, int y);
		void setSpeed(float speed);
		float getSpeed();
		void setDirection(int dir);
		int getDirection();
		void setAim(int dir);
		void update();
		int getX();
		int getY();
		int getAim();
		int getLives();
		bool takeDamage();
		float getAimInRads();
	private:
		int direction;
		int aim_direction;
		float max_speed;
		float speed;
		int lives;
		float x;
		float y;
};

#endif