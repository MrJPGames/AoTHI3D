#include <3ds.h>
#include <sf2d.h>
#include <sfil.h>
#include <sftd.h>
#include <stdio.h>
#include <stdlib.h>
#include <cstdlib> //For rand
#include <math.h>

#include "Objects\Player.h"
#include "Objects\Bullet.h"
#include "Objects\Object.h"
#include "Objects\Effect.h"
#include "Objects\Trail.h"
#include "Objects\NewEffect.h"

#include "Song.h"
#include "AudioFX.h"

#include "OpenSans_ttf.h"
#define PI 3.14159265
#define MAX_NEW_EFFECT 350

using namespace std;


Player player;
Bullet bullets[30]; //Never more than ~15 on screen during normal gameplay (power-ups might change that)
Object objects[50]; //Never 50 (~25 max) but HAS to be possible (easier to implement harder difficulties!)
Effect effects[15]; //Currently no more needed, might be in the future tho!
NewEffect new_effects[MAX_NEW_EFFECT]; //Way more needed than old effect
Trail bullet_trails[200]; //Max on screen ~190
circlePosition cpos;
circlePosition cstick;
touchPosition touch;
float deadzone = 15;

int canShoot=1;
int shootTimer=0;
int shootTimerMax=7;

int spawnTimer=20;
int spawnTimerMax=60;
int spawnTimerMin=15;

int diffTimer=60*20; //60fps * 30 sec, every 30 secs = faster spawning!
int diffTimerMax=diffTimer;
int diffTimerMin=60*10;

int score=0;
int itemsDestroyed=0;

int state=1;

Song bgm;

AudioFX sfx_shoot;
AudioFX sfx_explosion;


//Things of even greater shame
bool spawnEffectPlayer=true;

void restart(){
	int x;
	for (x=0;x<30;x++){
		bullets[x].kill();
	}
	for (x=0;x<50;x++){
		objects[x].kill();
	}
	for (x=0;x<15;x++){
		effects[x].kill();
	}
	for (x=0;x<MAX_NEW_EFFECT;x++){
		new_effects[x].kill();
	}
	player.setPos(200,120);
	canShoot=1;
	shootTimer=0;
	shootTimerMax=7;

	spawnTimer=20;
	spawnTimerMax=60;
	spawnTimerMin=15;

	diffTimer=60*20;
	diffTimerMax=diffTimer;
	diffTimerMin=60*10;

	score=0;
	itemsDestroyed=0;
}

void spawn(int side){
	int x,y,i;
	switch(side){
		case 0:
			x=rand() % 400;
			y=-32;
			break;
		case 1:
			x=rand() % 400;
			y=282;
			break;
		case 2:
			x=-32;
			y=rand() % 240;
			break;
		case 3:
			x=432;
			y=rand() % 240;
			break;
		default:
			x=0;
			y=0;
			break;
	}
	for (i = 0; i<50; i++){
		if (objects[i].getAlive() == 0){
			objects[i].resurect();
			objects[i].setPos(x, y);
			objects[i].setSpeed(1 + static_cast <float> (rand()) / static_cast <float> (RAND_MAX));
			objects[i].setScale(0.75 + static_cast <float> (rand()) / static_cast <float> (RAND_MAX/0.5));
			float xDiff = player.getX() - x;
			float yDiff = player.getY() - y;
			double angle=atan2(xDiff, -yDiff) * (180/PI);
			if (angle < 0){
				objects[i].setDirection(360+angle-4+rand() % 9);
			}else{
				objects[i].setDirection(angle-4+rand() % 9);
			}
			i=50;
		}
	}
}


void spawnEffect(int x, int y, int dir, float scale){
	int i,j,max_j,newfx_x,newfx_y,newfx_dir;
	j=0;
	max_j=30+(scale-0.75)*15;
	for (i = 0; i<MAX_NEW_EFFECT; i++){
		if (new_effects[i].getAlive() == false){
			j++;
			newfx_x = x + ( rand() % (int)(20*scale) )-(10*scale);
			newfx_y = y + ( rand() % (int)(20*scale) )-(10*scale);
			if (dir != -1){
				newfx_dir = dir-30+ (rand()%60); //Relative angle of -30 to 30 degrees
			}else{
				newfx_dir=rand()%360;
			}
			new_effects[i].resurect();
			new_effects[i].setPos(newfx_x,newfx_y);
			new_effects[i].setScale(scale);
			new_effects[i].setDirection(newfx_dir);
			new_effects[i].setSpeed(1+rand()%2);
			if (j>max_j)
				break;
		}
	}	
}

void spawnPlayerEffect(int x, int y){
	int i,j,max_j,newfx_x,newfx_y;
	j=0;
	max_j=1;
	for (i = 0; i<MAX_NEW_EFFECT; i++){
		if (new_effects[i].getAlive() == false){
			j++;
			newfx_x = x + ( rand() % (int)(6) )-(3);
			newfx_y = y + ( rand() % (int)(6) )-(3);
			new_effects[i].resurect();
			new_effects[i].setIsPlayerEffect(true);
			new_effects[i].setPos(newfx_x,newfx_y);
			new_effects[i].setScale(1);
			new_effects[i].setDirection(player.getDirection()+180);
			new_effects[i].setSpeed(player.getSpeed()/8);
			if (j>max_j)
				break;
		}
	}	
}

void spawnTrail(int x, int y){
	int i;
	for (i = 0; i<200; i++){
		if (bullet_trails[i].getAlive() == false){
			bullet_trails[i].resurect();
			bullet_trails[i].setPos(x,y);
			i=200;
		}
	}
}

void update(){
	int i,j;
	int x,y,x1,y1,dist;
	int b_size=3; //6x6 = radius of 3 (and yes they are 8x8 but the hitbox has to be slightly smaller because they don't fill the 8x8 area)
	int o_size;
	for (j=0;j<50;j++){ //Update object pos
		if (objects[j].getAlive())
			objects[j].update();
	}
	for (j=0;j<15;j++){ //Update object pos
		if (effects[j].getAlive())
			effects[j].update();
	}
	for (j=0;j<200;j++){ //Update object pos
		if (bullet_trails[j].getAlive())
			bullet_trails[j].update();
		if (new_effects[j].getAlive())
			new_effects[j].update();
	}
	for (i = 0; i<30; i++){ //Collision bullet-object
		if (bullets[i].getAlive()){
			bullets[i].update(); //Update bullet pos before collision checks
			spawnTrail(bullets[i].getX(), bullets[i].getY());
			x = bullets[i].getX();
			y = bullets[i].getY();
			for (j=0;j<50;j++){
				if (objects[j].getAlive()){
					x1 = objects[j].getX();
					y1 = objects[j].getY();
					o_size=16*objects[j].getScale(); //32x32 = radius of 16
					float diffX = x - x1;
					float diffY = y - y1;
					dist = sqrt((diffY * diffY) + (diffX * diffX));
					if (dist < b_size+o_size){
						bullets[i].kill();
						spawnEffect(x1,y1, bullets[i].getDirection(),objects[j].getScale());
						objects[j].kill();
						//Explosion sfx
						sfx_explosion.play();
						score+=100;
						itemsDestroyed+=1;
						j=50; //Bullet can only hit 1 object
					}
				}
			}
		}
	}
	x=player.getX();
	y=player.getY();
	if (spawnEffectPlayer && player.getSpeed() > 0){
		spawnPlayerEffect(x,y);
		spawnEffectPlayer=false;
	}else{
		spawnEffectPlayer=true;
	}
	for (j=0;j<50;j++){ //Collision player-object
		if (objects[j].getAlive()){
			x1 = objects[j].getX();
			y1 = objects[j].getY();
			o_size=16*objects[j].getScale(); //32x32 = radius of 16
			float diffX = x - x1;
			float diffY = y - y1;
			dist = sqrt((diffY * diffY) + (diffX * diffX));
			if (dist < 10+o_size){
				spawnEffect(x1,y1, -1 ,objects[j].getScale());
				objects[j].kill();
				//Play explosion sfx
				sfx_explosion.play();
				if (player.takeDamage()){
					state=2;
				}
			}
		}
	}
	if (spawnTimer>0){
		spawnTimer-=1;
	}else{
		spawnTimer=spawnTimerMax;
		int rng=round(rand() % 4);
		spawn(rng);
	}
	diffTimer-=1;
	if (diffTimer < 0){
		diffTimerMax-=60;
		diffTimer=diffTimerMax;
		spawnTimerMax-=5;
		if (diffTimer < diffTimerMin)
			diffTimer=diffTimerMin;
		if (spawnTimerMax < spawnTimerMin)
			spawnTimerMax=spawnTimerMin;
	}
	player.update();
}

void shoot(){
	int i;
	for (i = 0; i<30; i++){
		if (bullets[i].getAlive() == 0){
			bullets[i].resurect();
			bullets[i].setPos(player.getX(), player.getY());
			bullets[i].setSpeed(2);
			bullets[i].setDirection(player.getAim()-2+rand() % 5);
			i=30;
			canShoot=0;
			shootTimer=shootTimerMax;
			//Play shoot sfx
			sfx_shoot.play();
		}
	}
}

//TODO: remove
int timer=0; //Also remove rendering and timer+=1; just above rendering code!

int main(int argc, char **argv)
{
	//Initialize the sf2d lib
	sf2d_init();
	romfsInit();
	//Initialize ndsp
	ndspInit();

	ndspSetOutputMode(NDSP_OUTPUT_STEREO);
	ndspSetOutputCount(3); // Num of buffers
	//Set the background color
	sf2d_set_clear_color(RGBA8(0x40, 0x40, 0x40, 0xFF));

	sf2d_texture *tex_player = sfil_load_PNG_file("romfs:/Player.png", SF2D_PLACE_RAM);
	sf2d_texture *tex_bullet = sfil_load_PNG_file("romfs:/Bullets.png", SF2D_PLACE_RAM);
	sf2d_texture *tex_object = sfil_load_PNG_file("romfs:/Objects.png", SF2D_PLACE_RAM);
	sf2d_texture *tex_effect = sfil_load_PNG_file("romfs:/effect_explosion.png", SF2D_PLACE_RAM);
	sf2d_texture *tex_new_effect = sfil_load_PNG_file("romfs:/new_effect_explosion.png", SF2D_PLACE_RAM);
	sf2d_texture *tex_trail = sfil_load_PNG_file("romfs:/effect_trail.png", SF2D_PLACE_RAM);
	sf2d_texture *tex_bg = sfil_load_PNG_file("romfs:/bg.png", SF2D_PLACE_RAM);
	sf2d_texture *tex_bg_bottom = sfil_load_PNG_file("romfs:/bg_bottom.png", SF2D_PLACE_RAM);

	sftd_init();
	sftd_font *font = sftd_load_font_mem(OpenSans_ttf, OpenSans_ttf_size);
	sftd_draw_textf(font, 0, 0, RGBA8(255, 0, 0, 255), 50, "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz1234567890:-.'!?()\"end"); //Hack to avoid blurry text!

	bgm.initSong("romfs:/bgm/cut_the_check.wav");  //No need to set channel as 1 is default and that's the channel we are going to use for the bgm
	sfx_shoot.setChannel(2,5);
	sfx_explosion.setChannel(8,8);
	sfx_shoot.initSfx("romfs:/sfx/shoot.wav");
	sfx_explosion.initSfx("romfs:/sfx/explosion.wav");

	// Main loop
	player.setPos(200,120);
	float speed=0;
	int dx,dy,c_dx,c_dy,t_dx,t_dy;
	bool up,down,left,right;

	bgm.play();

	while (aptMainLoop()){
		//Scan all the inputs. This should be done once for each frame
		hidScanInput();
		hidCircleRead(&cpos);
		hidCstickRead(&cstick);
		hidTouchRead(&touch);
		
		u32 kDown = hidKeysDown();
		u32 kHeld = hidKeysHeld();
		u32 kUp = hidKeysUp();

		if (state == 1){
			score+=1;
			player.setSpeed(0);
			player.setDirection(0);
			dx=cpos.dx;
			dy=cpos.dy;
			c_dx=cstick.dx;
			c_dy=cstick.dy;
			t_dx=touch.px-160;
			t_dy=touch.py-120;
			if (abs(dx) > deadzone || abs(dy) > deadzone){
				speed=sqrt((abs(dx)^2)+(abs(dy)^2))/10;
				player.setSpeed(speed);
				if (dy > 0){
					player.setDirection(atan2(dx, dy) * (180/PI));
				}else{
					player.setDirection(360 + (atan2(dx, dy) * (180/PI)));
				}
			}else{
				up=kHeld & KEY_UP || kHeld & KEY_X; //Allow for right and left handed play!
				down=kHeld & KEY_DOWN || kHeld & KEY_B;
				left=kHeld & KEY_LEFT || kHeld & KEY_Y;
				right=kHeld & KEY_RIGHT || kHeld & KEY_A;
				if (down){
					player.setSpeed(1);
					player.setDirection(180);
					if (right)
						player.setDirection(135);
					if (left)
						player.setDirection(225);
				}else if (up){
					player.setSpeed(1);
					player.setDirection(0);
					if (right)
						player.setDirection(45);
					if (left)
						player.setDirection(315);
				}else if (left){
					player.setSpeed(1);
					player.setDirection(270);
				}else if (right){
					player.setSpeed(1);
					player.setDirection(90);
				}
			}
			if (abs(c_dx) > deadzone || abs(c_dy) > deadzone){
				if (c_dy > 0){
					player.setAim(atan2(c_dx, c_dy) * (180/PI));
				}else{
					player.setAim(360 + (atan2(c_dx, c_dy) * (180/PI)));
				}
			}else if ( (abs(t_dx) > 5 || abs(t_dy) > 5) && (kHeld & KEY_TOUCH)) {
				if (t_dy > 0){
					player.setAim(atan2(t_dx, -t_dy) * (180/PI));
				}else{
					player.setAim(360 + (atan2(t_dx, -t_dy) * (180/PI)));
				}
			}

			if (kDown & KEY_START) break; // break in order to return to hbmenu
			if ( (kHeld & KEY_R || kHeld & KEY_L) && canShoot){
				shoot();
			}
			if (kUp & KEY_R || kUp & KEY_L){
				canShoot=1;
			}
			if (canShoot == 0){
				shootTimer-=1;
				if (shootTimer <= 0){
					canShoot=1;
					shootTimer=shootTimerMax;
				}
			}

			update();

			sf2d_start_frame(GFX_TOP, GFX_LEFT);
				sf2d_draw_texture(tex_bg, 0, 0);
				int x;
				int tempDamageColChangeValue=0;
				for (x=0; x < 200; x++){
					if (bullet_trails[x].getAlive())
						sf2d_draw_texture_rotate_blend(tex_trail, bullet_trails[x].getX(), bullet_trails[x].getY(), bullet_trails[x].getRot(), RGBA8(255,255,255,bullet_trails[x].getAlpha()));
				}
				for (x=0; x < 30; x++){
					if (bullets[x].getAlive())
						sf2d_draw_texture_part_rotate_scale(tex_bullet, round(bullets[x].getX()), round(bullets[x].getY()), bullets[x].getAngleInRads(), bullets[x].getType()*8, 0, 8, 8, 1, 1);
				}
				for (x=0; x < 50; x++){
					if (objects[x].getAlive())
						sf2d_draw_texture_part_rotate_scale(tex_object, round(objects[x].getX()), round(objects[x].getY()), objects[x].getAngle(), 32*objects[x].getType(), 32*objects[x].isSpecial(), 32, 32, objects[x].getScale(), objects[x].getScale());
				}
				for (x=0; x < MAX_NEW_EFFECT; x++){
					if (new_effects[x].getAlive() && !new_effects[x].isPlayerEffect())
						sf2d_draw_texture_part_scale(tex_new_effect, new_effects[x].getX()-(new_effects[x].getScale()*8/2), new_effects[x].getY()-(new_effects[x].getScale()*8/2), 8*new_effects[x].getFrame(), 0, 8, 8, new_effects[x].getScale(), new_effects[x].getScale());
					if (new_effects[x].getAlive() && new_effects[x].isPlayerEffect()){
						int ef_r,ef_g,ef_b;
						ef_r=226-(266*new_effects[x].getFrame()/8);
						ef_g=88-(88*new_effects[x].getFrame()/8);
						ef_b=34-(34*new_effects[x].getFrame()/8);
						sf2d_draw_texture_part_scale_blend(tex_new_effect, new_effects[x].getX()-(new_effects[x].getScale()*8/2), new_effects[x].getY()-(new_effects[x].getScale()*8/2), 8*new_effects[x].getFrame(), 0, 8, 8, new_effects[x].getScale(), new_effects[x].getScale(), RGBA8(ef_r,ef_g,ef_b,255));
					}
				}
				if (player.getLives() == 1){
					tempDamageColChangeValue=(int)(sin(6.28*score/120)*20)+60;
				}
				sf2d_draw_texture_rotate_blend(tex_player, round(player.getX()), round(player.getY()), player.getAimInRads(), RGBA8(255,255-tempDamageColChangeValue,255-tempDamageColChangeValue,255));
			sf2d_end_frame();

			sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
				sf2d_draw_texture(tex_bg_bottom, 0, 0);
				sftd_draw_textf(font, 10, 5, RGBA8(255,255,255,255), 12, "Lives: %d", player.getLives());
				sftd_draw_textf(font, 10, 17, RGBA8(255,255,255,255), 12, "Score: %d", score);
				sftd_draw_textf(font, 10, 29, RGBA8(255,255,255,255), 12, "Destoryed: %d", itemsDestroyed);
				sftd_draw_textf(font, 264, 5, RGBA8(255,255,255,255), 12, "FPS: %i", (int)(sf2d_get_fps()) );
				timer+=1;
			sf2d_end_frame();
		}else if (state == 2){ //GameOver
			sf2d_start_frame(GFX_TOP, GFX_LEFT);
				sf2d_draw_texture(tex_bg, 0, 0);
				sftd_draw_text(font, 116, 5, RGBA8(255,255,255,255), 30, "Game Over!");
				sftd_draw_textf(font, 10, 70, RGBA8(255,255,255,255), 20, "Score: %d", score);
				sftd_draw_textf(font, 10, 90, RGBA8(255,255,255,255), 20, "Items Destoryed: %d", itemsDestroyed);
			sf2d_end_frame();
				
			sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
				sf2d_draw_texture(tex_bg_bottom, 0, 0);
				sftd_draw_textf(font, 264, 5, RGBA8(255,255,255,255), 12, "FPS: %i", (int)(sf2d_get_fps()) );
				timer+=1;
			sf2d_end_frame();
			if (kDown & KEY_A) {
				state=1;
				restart();
			}
		}
		//Swap the buffers
		sf2d_swapbuffers();
	}
	sf2d_free_texture(tex_player);
	sf2d_free_texture(tex_bullet);
	romfsExit();
	sftd_free_font(font);
	sftd_fini();
	sf2d_fini();
	ndspExit();
	return 0;
}
