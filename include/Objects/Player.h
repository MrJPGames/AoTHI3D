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
		void setInvincible(int i);
		bool getInvincible();
		bool getVisable();
		void update();
		void setSkin(int skinID);
		int getX();
		int getY();
		int getAim();
		int getLives();
		int getSkin();
		bool takeDamage();
		float getAimInRads();
	private:
		int direction;
		int invincible_timer=0;
		int aim_direction;
		int skin=0;
		float max_speed;
		float speed;
		int lives;
		float x;
		float y;
};

#endif