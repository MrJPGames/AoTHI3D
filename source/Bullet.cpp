#include <math.h>
#include <cstdlib> //For rand
#include "Bullet.h"
#define PI 3.14159265
#define WIDTH 400
#define HEIGHT 240

Bullet::Bullet(){
	isAlive=0;
	direction=0;
	aim_direction=90;
	max_speed=2.5;
	speed=0;
	x=0;
	y=0;
	type = rand() % 7;
}

Bullet::Bullet(int var1, int var2){
	setPos(var1, var2);
}

void Bullet::kill(){
	isAlive=0;
	type = round(rand() % 7);
}

void Bullet::resurect(){
	isAlive=1;
}

void Bullet::setPos(int var1, int var2){
	x=var1;
	y=var2;
}

void Bullet::setSpeed(float var1){
	speed=var1*max_speed;
}

void Bullet::setDirection(int dir){
	direction=dir;
}

void Bullet::update(){
	y-=round(speed*cos(direction*(PI/180)));
	x+=round(speed*sin(direction*(PI/180)));
	if (x > WIDTH+8){
		kill();
	}else if (x < -8){
		kill();
	}
	if (y > HEIGHT+8){
		kill();
	}else if (y < -8){
		kill();	
	}
}

float Bullet::getX(){
	return x;
}

float Bullet::getY(){
	return y;
}

bool Bullet::getAlive(){
	return isAlive;
}

int Bullet::getType(){
	return type;
}

float Bullet::getAngleInRads(){
	return direction * PI/180-1.5707963268;
}