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
		float getX();
		float getY();
		float getAngleInRads();
		bool getAlive();
		int getType();
	private:
		bool isAlive;
		int direction;
		int aim_direction;
		float max_speed;
		float speed;
		int x;
		int y;
		int type;
};

#endif