#include <math.h>
#include "Player.h"
#define PI 3.14159265
#define WIDTH 400
#define HEIGHT 240

Player::Player(){
	direction=0;
	aim_direction=90;
	max_speed=2;
	speed=0;
	lives=3;
	x=0;
	y=0;
}

Player::Player(int var1, int var2){
	setPos(var1, var2);
}

void Player::setPos(int var1, int var2){
	x=var1;
	y=var2;
}

void Player::setSpeed(float var1){
	speed=var1*max_speed;
}

float Player::getSpeed(){
	return speed;
}

void Player::setDirection(int dir){
	direction=dir;
}

int Player::getDirection(){
	return direction;
}

void Player::setAim(int dir){
	aim_direction=dir;
}

void Player::setInvincible(int i){
	invincible_timer=i;
}

bool Player::getInvincible(){
	return invincible_timer > 0;
}

bool Player::getVisable(){
	if (invincible_timer % 20 < 10){
		return true;
	}
	return false;
}

void Player::update(){
	y-=speed*cos(direction*(PI/180));
	x+=speed*sin(direction*(PI/180));
	if (x > WIDTH){
		x=x-WIDTH;
	}else if (x < 0){
		x=WIDTH+x;
	}
	if (y>HEIGHT){
		y=y-HEIGHT;
	}else if (y<0){
		y=HEIGHT+y;
	}
	if (invincible_timer > 0){
		invincible_timer--;
	}
}

bool Player::takeDamage(){
	lives-=1;
	if (lives<=0){
		lives=3;
		return true;
	}else{
		return false;
	}
}

int Player::getX(){
	return x;
}

int Player::getY(){
	return y;
}

int Player::getAim(){
	return aim_direction;
}

int Player::getLives(){
	return lives;
}

float Player::getAimInRads(){
	return aim_direction * PI/180-1.5707963268;
}