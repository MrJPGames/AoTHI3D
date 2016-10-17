#include <math.h>
#include <cstdlib> //For rand
#include "NewEffect.h"
#define PI 3.14159265
#define WIDTH 400
#define HEIGHT 240
#define TYPES 12

NewEffect::NewEffect(){
	x=0;
	y=0;
	type=0;
	frame=0;
	framerate=2; //1 image per 3 frames!
	frames=7; //Amount of frames (in effect not how long to display!)
	active=false;
	scale=1.0;
}

NewEffect::NewEffect(int var1, int var2){
	setPos(var1, var2);
}

void NewEffect::setPos(int var1, int var2){
	x=var1;
	y=var2;
}


void NewEffect::setSpeed(float var1){
	speed=var1;
}

void NewEffect::setDirection(int dir){
	direction=dir;
}


int NewEffect::getX(){
	return x;
}

int NewEffect::getY(){
	return y;
}

void NewEffect::setType(int var1){
	type=var1;
}

int NewEffect::getType(){
	return type;
}

int NewEffect::getFrame(){
	return (int)(frame/framerate);
}

void NewEffect::update(){
	if (active){
		frame+=1;
		y-=speed*cos(direction*(PI/180));
		x+=speed*sin(direction*(PI/180));
		if (frame > frames*framerate){
			kill();
		}
	}
}

void NewEffect::kill(){
	active=false;
}

void NewEffect::resurect(){
	type=0;
	frame=0;
	scale=1.0;
	active=true;
	player_effect=false;
}

bool NewEffect::getAlive(){
	return active;
}

void NewEffect::setScale(float s){
	scale=s;
}

float NewEffect::getScale(){
	return scale;
}

bool NewEffect::isPlayerEffect(){
	return player_effect;
}

void NewEffect::setIsPlayerEffect(bool playereffect){
	player_effect=playereffect;
}