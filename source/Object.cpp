#include <math.h>
#include <cstdlib> //For rand
#include "Object.h"
#define PI 3.14159265
#define WIDTH 400
#define HEIGHT 240
#define TYPES 12
#define SPECIALTYPES 1

Object::Object(){
	isAlive=0;
	direction=0;
	aim_direction=90;
	speed=0;
	x=0;
	y=0;
	angle=0;
	scale=1;
	rot_dir=1;
	special=0;
	type = rand() % TYPES;
	if ((rand() % 200) == 42){ //Special item!
		type = rand() % SPECIALTYPES;
		special=1;
	}
}

Object::Object(int var1, int var2){
	setPos(var1, var2);
}

void Object::kill(){
	isAlive=0;
	rot_dir=-rot_dir;
	special=0;
	type = rand() % TYPES;
	if ((rand() % 200) == 42){ //Special item!
		type = 0;
		special=1;
	}
}

void Object::resurect(){
	isAlive=1;
}

void Object::setPos(int var1, int var2){
	x=var1;
	y=var2;
}

void Object::setSpeed(float var1){
	speed=var1;
}

void Object::setDirection(int dir){
	direction=dir;
}

void Object::update(){
	y-=round(speed*cos(direction*(PI/180)));
	x+=round(speed*sin(direction*(PI/180)));
	angle+=2*PI/180;
	if (x > WIDTH+32){
		kill();
	}else if (x < -32){
		kill();
	}
	if (y > HEIGHT+32){
		kill();
	}else if (y < -32){
		kill();	
	}
}

float Object::getX(){
	return x;
}

float Object::getY(){
	return y;
}

bool Object::getAlive(){
	return isAlive;
}

float Object::getAngle(){
	return angle;
}

float Object::getScale(){
	return scale;
}

void Object::setScale(float s){
	scale=s;
}

int Object::getType(){
	return type;
}

int Object::isSpecial(){
	return special;
}