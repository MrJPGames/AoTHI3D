#ifndef Effect_H
#define Effect_H

class Effect{
	public:
		Effect();
		Effect(int x, int y);
		void setPos(int var1, int var2);
		int getX();
		int getY();
		void setType(int var1);
		int getType();
		int getFrame();
		void update();
		void kill();
		void resurect();
		bool getAlive();
		void setScale(float s);
		float getScale();
	private:
		int x;
		int y;
		int type;
		int frame;
		int framerate;
		int frames;
		bool active;
		float scale;
};

#endif