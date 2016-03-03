#ifndef Object_H
#define Object_H

class Object{
	public:
		Object();
		Object(int x, int y);
		void setPos(int x, int y);
		void setSpeed(float speed);
		void setDirection(int dir);
		void update();
		void kill();
		void resurect();
		float getX();
		float getY();
		float getAngle();
		bool getAlive();
		float getScale();
		void setScale(float s);
		int getType();
		int isSpecial();
	private:
		bool isAlive;
		float direction;
		int aim_direction;
		float max_speed;
		float speed;
		float angle;
		float scale;
		int rot_dir;
		int type;
		int x;
		int y;
		int special;
};

#endif