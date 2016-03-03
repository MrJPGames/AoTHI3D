#ifndef Player_H
#define Player_H

class Player{
	public:
		Player();
		Player(int x, int y);
		void setPos(int x, int y);
		void setSpeed(float speed);
		void setDirection(int dir);
		void setAim(int dir);
		void update();
		float getX();
		float getY();
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
		int x;
		int y;
};

#endif