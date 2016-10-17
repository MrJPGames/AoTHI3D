#ifndef Bullet_H
#define Bullet_H

class Bullet{
	public:
		Bullet();
		Bullet(int x, int y);
		void setPos(int x, int y);
		void setSpeed(float speed);
		void setDirection(int dir);
		void update();
		void kill();
		void resurect();
		int getX();
		int getY();
		float getAngleInRads();
		bool getAlive();
		int getType();
		int getDirection();
	private:
		bool isAlive;
		int direction;
		float max_speed;
		float speed;
		float x;
		float y;
		int type;
};

#endif