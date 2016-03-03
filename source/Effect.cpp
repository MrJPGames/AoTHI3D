#include <math.h>
#include <cstdlib> //For rand
#include "Effect.h"
#define PI 3.14159265
#define WIDTH 400
#define HEIGHT 240
#define TYPES 12

Effect::Effect(){
	x=0;
	y=0;
	type=0;
	frame=0;
	framerate=2; //1 image per 2 frames!
	frames=7; //Amount of frames (in effect not how long to display!)
	active=false;
	scale=1.0;
}

Effect::Effect(int var1, int var2){
	setPos(var1, var2);
}

void Effect::setPos(int var1, int var2){
	x=var1;
	y=var2;
}

int Effect::getX(){
	return x;
}

int Effect::getY(){
	return y;
}

void Effect::setType(int var1){
	type=var1;
}

int Effect::getType(){
	return type;
}

int Effect::getFrame(){
	return frame/framerate;
}

void Effect::update(){
	if (active){
		frame+=1;
		if (floor(frame/framerate) > frames){
			kill();
		}
	}
}

void Effect::kill(){
	active=false;
}

void Effect::resurect(){
	type=0;
	frame=0;
	framerate=2;
	frames=7;
	scale=1.0;
	active=true;
}

bool Effect::getAlive(){
	return active;
}

void Effect::setScale(float s){
	scale=s;
}

float Effect::getScale(){
	return scale;
}