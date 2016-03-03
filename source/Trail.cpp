#include <math.h>
#include <cstdlib> //For rand
#include "Trail.h"
#define PI 3.14159265
#define WIDTH 400
#define HEIGHT 240

Trail::Trail(){
	x=0;
	y=0;
	active=false;
	rot=0; //Rotation
	alpha=255;
}

Trail::Trail(int var1, int var2){
	setPos(var1, var2);
}

float Trail::getRot(){
	return rot * PI/180;
}

int Trail::getAlpha(){
	return alpha;
}

void Trail::setPos(int var1, int var2){
	x=var1;
	y=var2;
}

int Trail::getX(){
	return x;
}

int Trail::getY(){
	return y;
}

void Trail::update(){
	if (active){
		rot+=5;
		if (rot > 360)
			rot=0;
		alpha-=20;
		if (alpha < 0)
			kill();
	}
}

void Trail::kill(){
	active=false;
}

void Trail::resurect(){
	active=true;
	rot=0;
	alpha=255;
}

bool Trail::getAlive(){
	return active;
}