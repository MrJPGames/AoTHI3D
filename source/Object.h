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
		int getX();
		int getY();
		float getAngle();
		bool getAlive();
		float getScale();
		void setScale(float s);
		int getType();
		int isSpecial();
	private:
		bool isAlive;
		float direction;
		float max_speed;
		float speed;
		float angle;
		float scale;
		int rot_dir;
		int type;
		float x;
		float y;
		int special;
};

#endif