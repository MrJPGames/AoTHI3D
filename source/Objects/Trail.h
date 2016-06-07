#ifndef Trail_H
#define Trail_H

class Trail{
	public:
		Trail();
		Trail(int x, int y);
		void setPos(int var1, int var2);
		int getX();
		int getY();
		void update();
		void kill();
		void resurect();
		bool getAlive();
		float getRot();
		int getAlpha();
	private:
		int x;
		int y;
		bool active;
		int alpha;
		int rot;
};

#endif