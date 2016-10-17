#ifndef NewEffect_H
#define NewEffect_H

class NewEffect{
	public:
		NewEffect();
		NewEffect(int x, int y);
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
		void setSpeed(float s);
		void setDirection(int d);
		bool isPlayerEffect();
		void setIsPlayerEffect(bool playereffect);
	private:
		int direction;
		float speed;
		int x;
		int y;
		int type;
		int frame;
		int framerate;
		int frames;
		bool active;
		float scale;
		bool player_effect=false;
};

#endif