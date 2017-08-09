#include <3ds.h>
#include <sf2d.h>
#include <sfil.h>
#include <sftd.h>
#include <stdio.h>
#include <stdlib.h>
#include <cstdlib> //For rand
#include <math.h>
#include <string>

#include "Objects\Player.h"
#include "Objects\Bullet.h"
#include "Objects\Object.h"
#include "Objects\Trail.h"
#include "Objects\NewEffect.h"

#include "Song.h"
#include "AudioFX.h"

#include "SaveManager.h"
#include "Online.h"

#include "OpenSans_ttf.h"
#define PI 3.14159265
#define MAX_NEW_EFFECT 350
#define MAX_OBJECT 50
#define MAX_BULLET 30
#define MAX_TRAIL 200

using namespace std;


Player player;

Bullet bullets[30]; //Never more than ~15 on screen during normal gameplay (power-ups might change that)
Object objects[50]; //Never 50 (~25 max) but HAS to be possible (easier to implement harder difficulties!)
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

int state=0;
int menustate=0;
int selectedDificulty=0;
int selectedMenuOption=0;
int selectedSubMenuOption=0;
int highScoreState=0;
int goSelected=0;

int timer=0; //Global timer

int difficulty=0;

int cur_song_id=0;

Song bgm;

AudioFX sfx_shoot;
AudioFX sfx_explosion;

string songNames[5] = {"Cut the Check by Spitfya x Desembra","Fade by Alan Walker","The Light by Tetrix Bass Feat. Veela","Spectre by Alan Walker", "OP by LOUD"};
string songFileName[5] = {"romfs:/bgm/cut_the_check.wav", "romfs:/bgm/fade.wav", "romfs:/bgm/tetrix_bass.wav", "romfs:/bgm/spectre.wav", "romfs:/bgm/op.wav"};
int SONG_AMOUNT = 5;

//Things of even greater shame
bool spawnEffectPlayer=true;

AoTSJSaveManager save;
Online online;
int curPage=0;
bool newHighScore=false;
bool newItemsDestoryedScore=false;

void restart(){
	int x;
	for (x=0;x<30;x++){
		bullets[x].kill();
	}
	for (x=0;x<50;x++){
		objects[x].kill();
	}
	for (x=0;x<MAX_NEW_EFFECT;x++){
		new_effects[x].kill();
	}
	player.setPos(200,120);
	canShoot=1;
	shootTimer=0;
	shootTimerMax=7;

	spawnTimer=20;
	if (difficulty == 0){
		spawnTimerMax=60;
		spawnTimerMin=15;
	}else if (difficulty == 1){
		spawnTimerMax=30;
		spawnTimerMin=8;
	}else{
		spawnTimerMax=10;
		spawnTimerMin=5;
	}

	//Sets how fast the spawn timer decreses (aka how quick it becomes more difficult)
	if (difficulty == 0){
		diffTimer=60*20;
		diffTimerMax=diffTimer;
		diffTimerMin=60*10;
	}else if (difficulty == 1){
		diffTimer=60*12;
		diffTimerMax=diffTimer;
		diffTimerMin=60*5;
	}else{
		diffTimer=60*20;
		diffTimerMax=diffTimer;
		diffTimerMin=60*5;
	}

	score=0;
	itemsDestroyed=0;

	int new_song_id=rand() % SONG_AMOUNT;
	if (cur_song_id != new_song_id){
		bgm.stop();
		bgm.initSong(songFileName[new_song_id].c_str());
		cur_song_id=new_song_id;
		bgm.play();
	}
}

void spawnObject(float x, float y, bool isSpecial=false, int type=-1, float speed=-1, int dir=-1){
	for (int i = 0; i<MAX_OBJECT; i++){
		if (objects[i].getAlive() == 0){
			objects[i].resurect();
			objects[i].setPos((int)x, (int)y);
			if (speed == -1)
				objects[i].setSpeed(1 + static_cast <float> (rand()) / static_cast <float> (RAND_MAX));
			else
				objects[i].setSpeed(speed);
			objects[i].setScale(0.75 + static_cast <float> (rand()) / static_cast <float> (RAND_MAX/0.5));
			if (isSpecial) objects[i].setSpecial(isSpecial);
			if (type != -1)	objects[i].setType(type);
			float xDiff = player.getX() - x;
			float yDiff = player.getY() - y;
			double angle=atan2(xDiff, -yDiff) * (180/PI);
			if (dir == -1){
				if (angle < 0){
					objects[i].setDirection(360+angle-4+rand() % 9);
				}else{
					objects[i].setDirection(angle-4+rand() % 9);
				}
			}else{
				objects[i].setDirection(dir);
			}
			break;
		}
	}
}

void spawn(int side){
	int x,y;
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
	spawnObject(x,y);
}

void spawnEffect(int x, int y, int dir, float scale){
	int i,j,max_j,newfx_x,newfx_y,newfx_dir;
	j=0;
	max_j=10+(scale-0.75)*10;
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
	for (i = 0; i<MAX_TRAIL; i++){
		if (bullet_trails[i].getAlive() == false){
			bullet_trails[i].resurect();
			bullet_trails[i].setPos(x,y);
			break;
		}
	}
}

void update(){
	int i,j;
	int x,y,x1,y1,dist;
	int b_size=3; //6x6 = radius of 3 (and yes they are 8x8 but the hitbox has to be slightly smaller because they don't fill the 8x8 area)
	int o_size;
	bool playerHasLostLifes=false;
	for (j=0;j<MAX_OBJECT;j++){ //Update object pos
		if (objects[j].getAlive()){
			objects[j].update();
			if (objects[j].isSpecial()){
				if (objects[j].getType() == 0 && rand() % 60 == 42){
					spawnObject(objects[j].getX(), objects[j].getY(), 1, 19, 3, objects[j].getAngle()*180/PI+180);
				}
			}
		}
	}
	for (j=0;j<MAX_TRAIL;j++){ //Update trail state
		if (bullet_trails[j].getAlive())
			bullet_trails[j].update();
	}
	for (j=0;j<MAX_NEW_EFFECT;j++){
		if (new_effects[j].getAlive())
			new_effects[j].update();
	}
	for (i = 0; i<MAX_BULLET; i++){ //Collision bullet-object
		if (bullets[i].getAlive()){
			bullets[i].update(); //Update bullet pos before collision checks
			spawnTrail(bullets[i].getX(), bullets[i].getY());
			x = bullets[i].getX();
			y = bullets[i].getY();
			for (j=0;j<MAX_OBJECT;j++){
				if (objects[j].getAlive()){
					x1 = objects[j].getX();
					y1 = objects[j].getY();
					o_size=16*objects[j].getScale(); //32x32 = radius of 16
					float diffX = x - x1;
					float diffY = y - y1;
					dist = sqrt((diffY * diffY) + (diffX * diffX));
					if (dist < b_size+o_size){
						if (objects[j].isSpecial())
							score+=1000;
						else
							score+=100;
						itemsDestroyed+=1;
						bullets[i].kill();
						spawnEffect(x1,y1, bullets[i].getDirection(),objects[j].getScale());
						objects[j].kill();
						//Explosion sfx
						sfx_explosion.play();
						break;
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
	if (!player.getInvincible()){
		for (j=0;j<MAX_OBJECT;j++){ //Collision player-object
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
					if (playerHasLostLifes == false){ //To avoid losing 2 or more lifes in 1 frame (rendering invincebility usless + instant GO possibility in Insane (Hard) mode)
						if (player.takeDamage()){
							//Game Over event
							newHighScore=false;
							playerHasLostLifes=true;
							newItemsDestoryedScore=false;
							if (score > save.getScore(difficulty)){
								save.setScore(difficulty, score);
								newHighScore=true;
							}
							if (itemsDestroyed > save.getItemsDestroyed(difficulty)){
								save.setItemsDestroyed(difficulty, itemsDestroyed);
								newItemsDestoryedScore=true;
							}
							if (newHighScore || newItemsDestoryedScore)
								save.storeSaveData("/3ds/AoTSJSave.xml");
							state=2;
						}else{
							player.setInvincible(120);
						}
					}
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
	for (i = 0; i<MAX_BULLET; i++){
		if (bullets[i].getAlive() == 0){
			bullets[i].resurect();
			bullets[i].setPos(player.getX(), player.getY());
			bullets[i].setSpeed(2);
			bullets[i].setDirection(player.getAim()-2+rand() % 5);
			canShoot=0;
			shootTimer=shootTimerMax;
			//Play shoot sfx
			sfx_shoot.play();
			break;
		}
	}
}

int main(int argc, char **argv)
{
	//Initialize the sf2d lib
	sf2d_init();
	romfsInit();
	//Initialize ndsp
	ndspInit();
	aptInit();

	bool isNew=false;
	APT_CheckNew3DS(&isNew);
	
	ndspSetOutputMode(NDSP_OUTPUT_STEREO);
	ndspSetOutputCount(3); // Num of buffers
	//Set the background color
	sf2d_set_clear_color(RGBA8(0x40, 0x40, 0x40, 0xFF));

	sf2d_texture *tex_player = sfil_load_PNG_file("romfs:/Player.png", SF2D_PLACE_RAM);
	sf2d_texture *tex_bullet = sfil_load_PNG_file("romfs:/Bullets.png", SF2D_PLACE_RAM);
	sf2d_texture *tex_object = sfil_load_PNG_file("romfs:/Objects.png", SF2D_PLACE_RAM);
	sf2d_texture *tex_new_effect = sfil_load_PNG_file("romfs:/new_effect_explosion.png", SF2D_PLACE_RAM);
	sf2d_texture *tex_trail = sfil_load_PNG_file("romfs:/effect_trail.png", SF2D_PLACE_RAM);
	sf2d_texture *tex_bg = sfil_load_PNG_file("romfs:/bg.png", SF2D_PLACE_RAM);
	sf2d_texture *tex_bg_bottom = sfil_load_PNG_file("romfs:/bg_bottom.png", SF2D_PLACE_RAM);
	sf2d_texture *tex_menu_mrtophat = sfil_load_PNG_file("romfs:/menu/MrTopHat.png", SF2D_PLACE_RAM);
	sf2d_texture *tex_menu_logo = sfil_load_PNG_file("romfs:/menu/Logo.png", SF2D_PLACE_RAM);
	sf2d_texture *tex_selector = sfil_load_PNG_file("romfs:/menu/Selector.png", SF2D_PLACE_RAM);
	sf2d_texture *tex_skin_selector = sfil_load_PNG_file("romfs:/menu/SkinSelector.png", SF2D_PLACE_RAM);
	sf2d_texture *tex_back = sfil_load_PNG_file("romfs:/menu/Back.png", SF2D_PLACE_RAM);
	sf2d_texture *tex_aim = sfil_load_PNG_file("romfs:/TouchAimHelp.png", SF2D_PLACE_RAM);
	sf2d_texture *tex_controls;
	if (isNew)
		tex_controls = sfil_load_PNG_file("romfs:/menu/ControlsN3DS.png", SF2D_PLACE_RAM);
	else
		tex_controls = sfil_load_PNG_file("romfs:/menu/ControlsO3DS.png", SF2D_PLACE_RAM);

	sftd_init();
	sftd_font *font = sftd_load_font_mem(OpenSans_ttf, OpenSans_ttf_size);
	sftd_draw_textf(font, 0, 0, RGBA8(255, 0, 0, 255), 50, "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz1234567890:-.'!?()\"end"); //Hack to avoid blurry text!

	int new_song_id=rand() % SONG_AMOUNT;
	if (cur_song_id != new_song_id){
		bgm.stop();
		bgm.initSong(songFileName[new_song_id].c_str());
		cur_song_id=new_song_id;
		bgm.play();
	}
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

	//Intro
		//Fade in of logo (0.2s-1s)
		//Fade in of random startup text (1s-1.5s)
		//End (2.5s)
	for (int i=12; i > 0; i--){
		sf2d_start_frame(GFX_TOP, GFX_LEFT);
			sf2d_draw_texture(tex_bg, 0, 0);
		sf2d_end_frame();
		sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
			sf2d_draw_texture(tex_bg_bottom, 0, 0);
			//sftd_draw_textf(font, 264, 5, RGBA8(255,255,255,255), 12, "1: %i", (int)(i) );
		sf2d_end_frame();
		sf2d_swapbuffers();
	}
	for (int i=48; i>0; i--){
		sf2d_start_frame(GFX_TOP, GFX_LEFT);
			sf2d_draw_texture(tex_bg, 0, 0);
			sf2d_draw_texture_scale_blend(tex_menu_logo, 28+(i*55/48), 69, 1+(0.5*(48-i)/48), 1+(0.5*(48-i)/48), RGBA8(255,255,255,(48-i)*255/48));
		sf2d_end_frame();
		sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
			sf2d_draw_texture(tex_bg_bottom, 0, 0);
			//sftd_draw_textf(font, 264, 5, RGBA8(255,255,255,255), 12, "2: %i", (int)(i) );
		sf2d_end_frame();
		sf2d_swapbuffers();
	}
	for (int i=120; i>0; i--){
		//Scan all the inputs. This should be done once for each frame
		hidScanInput();
		u32 kDown = hidKeysDown();

		sf2d_start_frame(GFX_TOP, GFX_LEFT);
			sf2d_draw_texture(tex_bg, 0, 0);
			sf2d_draw_texture_scale_blend(tex_menu_logo, 28, 69, 1.5, 1.5, RGBA8(255,255,255,255));
		sf2d_end_frame();
		sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
			sf2d_draw_texture(tex_bg_bottom, 0, 0);
			//sftd_draw_textf(font, 264, 5, RGBA8(255,255,255,255), 12, "3: %i", (int)(i) );
		sf2d_end_frame();
		sf2d_swapbuffers();
		if (kDown & KEY_A) i=0;
	}
	save.loadSaveDataFromFile("/3ds/AoTSJSave.xml");
	player.setSkin(save.getPlayerSkin());

	while (aptMainLoop()){
		//Scan all the inputs. This should be done once for each frame
		hidScanInput();
		hidCircleRead(&cpos);
		hidCstickRead(&cstick);
		hidTouchRead(&touch);
		
		u32 kDown = hidKeysDown();
		u32 kHeld = hidKeysHeld();
		u32 kUp = hidKeysUp();
		timer+=1;
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

			if ( (kHeld & KEY_R || kHeld & KEY_L || kHeld & KEY_TOUCH) && canShoot){
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
					tempDamageColChangeValue=(int)(sin(6.28*timer/120)*20)+60;
				}
				sf2d_draw_texture_part_rotate_scale_blend(tex_player, round(player.getX()), round(player.getY()), player.getAimInRads(), player.getSkin()*20, 0, 20, 20, 1, 1, RGBA8(255,255-tempDamageColChangeValue, 255-tempDamageColChangeValue, (int)player.getVisable()*255));
				sf2d_draw_texture_part_rotate_scale_blend(tex_player, round(player.getX()-400), round(player.getY()), player.getAimInRads(), player.getSkin()*20, 0, 20, 20, 1, 1, RGBA8(255,255-tempDamageColChangeValue, 255-tempDamageColChangeValue, (int)player.getVisable()*255));
				sf2d_draw_texture_part_rotate_scale_blend(tex_player, round(player.getX()+400), round(player.getY()), player.getAimInRads(), player.getSkin()*20, 0, 20, 20, 1, 1, RGBA8(255,255-tempDamageColChangeValue, 255-tempDamageColChangeValue, (int)player.getVisable()*255));
				sf2d_draw_texture_part_rotate_scale_blend(tex_player, round(player.getX()), round(player.getY()-240), player.getAimInRads(), player.getSkin()*20, 0, 20, 20, 1, 1, RGBA8(255,255-tempDamageColChangeValue, 255-tempDamageColChangeValue, (int)player.getVisable()*255));
				sf2d_draw_texture_part_rotate_scale_blend(tex_player, round(player.getX()), round(player.getY()+240), player.getAimInRads(), player.getSkin()*20, 0, 20, 20, 1, 1, RGBA8(255,255-tempDamageColChangeValue, 255-tempDamageColChangeValue, (int)player.getVisable()*255));
			sf2d_end_frame();

			sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
				sf2d_draw_texture(tex_bg_bottom, 0, 0);
				sftd_draw_textf(font, 10, 5, RGBA8(255,255,255,255), 12, "Lives: %d", player.getLives());
				sftd_draw_textf(font, 10, 17, RGBA8(255,255,255,255), 12, "Score: %d", score);
				sftd_draw_textf(font, 10, 29, RGBA8(255,255,255,255), 12, "Destroyed: %d", itemsDestroyed);
				sftd_draw_textf(font, 10, 41, RGBA8(255,255,255,255), 12, "Current song: %s", songNames[cur_song_id].c_str());
				//sftd_draw_textf(font, 10, 53, RGBA8(255,255,255,255), 12, "Free mem space: %i", linearSpaceFree());
				sftd_draw_textf(font, 264, 5, RGBA8(255,255,255,255), 12, "FPS: %i", (int)round(sf2d_get_fps()) );
				sf2d_draw_texture(tex_aim, 0, 0);
			sf2d_end_frame();
		}else if (state == 2){ //GameOver
			sf2d_start_frame(GFX_TOP, GFX_LEFT);
				sf2d_draw_texture(tex_bg, 0, 0);
				sftd_draw_text(font, 116, 5, RGBA8(255,255,255,255), 30, "Game Over!");
				sftd_draw_textf(font, 10, 70, RGBA8(255,255,255,255), 20, "Score: %d", score);
				if (newHighScore)
					sftd_draw_text(font, 250, 70, RGBA8(127+(int)(128*sin(6.28*(timer-20)/240)),127+(int)(128*sin(6.28*(timer-20)/120)),127+(int)(128*sin(6.28*(timer-60)/160)),255), 20, "New High Score!");
				sftd_draw_textf(font, 10, 90, RGBA8(255,255,255,255), 20, "Items Destroyed: %d", itemsDestroyed);
				if (newItemsDestoryedScore)
					sftd_draw_text(font, 250, 90, RGBA8(127+(int)(128*sin(6.28*(timer-11)/240)),127+(int)(128*sin(6.28*(timer-32)/240)),127+(int)(128*sin(6.28*(timer-72)/120)),255), 20, "New High Score!");
				sftd_draw_text(font, 50, 160, RGBA8(255,255,255,255), 20, "Retry");
				sftd_draw_text(font, 50, 180, RGBA8(255,255,255,255), 20, "Change Difficulty");
				sftd_draw_text(font, 50, 200, RGBA8(255,255,255,255), 20, "Menu");
				sf2d_draw_texture(tex_selector, 25, 160+20*goSelected);
			sf2d_end_frame();
				
			sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
				sf2d_draw_texture(tex_bg_bottom, 0, 0);
				sftd_draw_textf(font, 264, 5, RGBA8(255,255,255,255), 12, "FPS: %i", (int)round(sf2d_get_fps()) );
			sf2d_end_frame();
			if (kDown & KEY_DOWN){
				goSelected++;
				if (goSelected > 2)
					goSelected=0;
			}
			if (kDown & KEY_UP){
				goSelected--;
				if (goSelected < 0)
					goSelected=2;
			}
			if (kDown & KEY_A) {
				if (goSelected == 0){
					state=1;
					restart();
				}else{
					state=0;
					if (goSelected == 2)
						menustate=1;
				}
			}
		}else{
			sf2d_start_frame(GFX_TOP, GFX_LEFT);
				sf2d_draw_texture(tex_bg, 0, 0);
				if (menustate < 3){
					sf2d_draw_texture(tex_menu_mrtophat, 10, 30+(int)(20*sin(6.28*timer/240)) );
					if (menustate == 0){
						sftd_draw_text(font, 170, 200, RGBA8(255,255,255,255), 25, "Press A to start!");
						sf2d_draw_texture(tex_menu_logo, 150, 30);
					}
					if (menustate == 1){
						sf2d_draw_texture(tex_menu_logo, 150, 30);
						sf2d_draw_texture(tex_selector, 175, 110+20*selectedMenuOption);
						sftd_draw_text(font, 200, 110, RGBA8(255,255,255,255), 20, "Play");
						sftd_draw_text(font, 200, 130, RGBA8(255,255,255,255), 20, "Skins");
						sftd_draw_text(font, 200, 150, RGBA8(255,255,255,255), 20, "Scoreboard");
						sftd_draw_text(font, 200, 170, RGBA8(255,255,255,255), 20, "Online Leaderboard");
						sftd_draw_text(font, 200, 190, RGBA8(255,255,255,255), 20, "Controls");
					}
					if (menustate == 2){
						sf2d_draw_texture(tex_selector, 175, 90+20*selectedDificulty);
						sftd_draw_text(font, 150, 50, RGBA8(255,255,255,255), 25, "Difficulty Select:");
						sftd_draw_text(font, 200, 90, RGBA8(255,255,255,255), 20, "Normal");
						sftd_draw_text(font, 200, 110, RGBA8(255,255,255,255), 20, "Hard");
						sftd_draw_text(font, 200, 130, RGBA8(255,255,255,255), 20, "Insane");
					}
				}else if (menustate == 3){
					sftd_draw_text(font, 20, 20, RGBA8(255,255,255,255), 20,  "<-");
					sftd_draw_text(font, 380-sftd_get_text_width(font, 20, "->"), 20, RGBA8(255,255,255,255), 20,  "->");
					if (highScoreState == 0){
						sftd_draw_text(font, 200-sftd_get_text_width(font, 20, "High Scores")/2, 20, RGBA8(255,255,255,255), 20,  "High Scores");
						sftd_draw_textf(font, 30, 50, RGBA8(255,255,255,255), 18, "Normal: %i\nHard: %i\nInsane: %i\n", save.getScore(0), save.getScore(1), save.getScore(2));
					}
					else{
						sftd_draw_text(font, 200-sftd_get_text_width(font, 20, "Items Destoryed Scores")/2, 20, RGBA8(255,255,255,255), 20, "Items Destoryed Scores");
						sftd_draw_textf(font, 30, 50, RGBA8(255,255,255,255), 18, "Normal: %i\nHard: %i\nInsane: %i\n", save.getItemsDestroyed(0), save.getItemsDestroyed(1), save.getItemsDestroyed(2));
					}
					sf2d_draw_texture(tex_back, 4, 212);
				}else if (menustate == 4){
					if (!online.isLoaded()){
						sftd_draw_text(font, 200-sftd_get_text_width(font, 20, "Connecting...")/2, 10, RGBA8(255,255,255,255), 20, "Connecting...");
						sf2d_swapbuffers();
						if (online.getLeaderboard() != 0){
							menustate = 5;
						}
					}
					sftd_draw_text(font, 20, 10, RGBA8(255,255,255,255), 20,  "<-");
					sftd_draw_text(font, 380-sftd_get_text_width(font, 20, "->"), 10, RGBA8(255,255,255,255), 20,  "->");
					sftd_draw_text(font, 200-sftd_get_text_width(font, 20, online.getPageData(curPage,0).c_str())/2, 10, RGBA8(255,255,255,255), 20,  online.getPageData(curPage,0).c_str());
					for (int i=0; i < online.getPageLines(); i++){
						sftd_draw_textf(font, 20,35+20*i,RGBA8(255,255,255,255),20,"%s", online.getPageData(curPage,i+1).c_str());
					}
				}else if (menustate == 5){
					sftd_draw_textf(font, 200-sftd_get_text_width(font, 20, "Failed to connect! Press B to exit.")/2,0,RGBA8(255,255,255,255),20,"Failed to connect! Press B to exit.");
					sftd_draw_textf(font, 0, 50, RGBA8(255,255,255,255),20, online.getRawPageData().c_str() );
					sf2d_draw_texture(tex_back, 4, 212);
				}else if (menustate == 6){
					int xpos,ypos;
					sftd_draw_textf(font, 200-sftd_get_text_width(font, 20, "Select a player skin:")/2,0,RGBA8(255,255,255,255),20,"Select a player skin:");
					for (int i=0; i < tex_player->width/20; i++){
						if (i > 5){
							xpos = i%6;
							ypos = floor(i/6);
						}else{
							xpos = i;
							ypos=0;
						}
						sf2d_draw_texture_part_scale(tex_player, 30+xpos*60, 50+52*ypos, i*20, 0, 20, 20, 2, 2);
					}
					int i=player.getSkin();
					if (i > 5){
						xpos = i%6;
						ypos = floor(i/6);
					}else{
						xpos = i;
						ypos=0;
					}
					sf2d_draw_texture_part_scale(tex_skin_selector, 26+xpos*60, 46+52*ypos, 24, 0, 24, 24, 2, 2);
					i=selectedSubMenuOption;
					if (i > 5){
						xpos = i%6;
						ypos = floor(i/6);
					}else{
						xpos = i;
						ypos=0;
					}
					sf2d_draw_texture_part_scale(tex_skin_selector, 26+xpos*60, 46+52*ypos, 0, 0, 24, 24, 2, 2);
					sf2d_draw_texture(tex_back, 4, 212);
				}else{
					sftd_draw_textf(font, 200-sftd_get_text_width(font, 20, "Controls")/2,0,RGBA8(255,255,255,255),20,"Controls");
					sf2d_draw_texture(tex_controls, 0, 0);
					sf2d_draw_texture(tex_back, 4, 212);
				}
			sf2d_end_frame();
				
			sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
				sf2d_draw_texture(tex_bg_bottom, 0, 0);
				sftd_draw_textf(font, 264, 5, RGBA8(255,255,255,255), 12, "FPS: %i", (int)round(sf2d_get_fps()) );
				sftd_draw_textf(font, 10, 41, RGBA8(255,255,255,255), 12, "Current song: %s", songNames[cur_song_id].c_str());
			sf2d_end_frame();
			if (kDown & KEY_A) {
				if (menustate == 0){
					menustate=1;
				}else if (menustate == 1){
					switch (selectedMenuOption){
						case 0:
							menustate = 2;
							break;
						case 1:
							selectedSubMenuOption=player.getSkin();
							menustate = 6;
							break;
						case 2:
							menustate = 3;
							break;
						case 3:
							curPage=0;
							menustate = 4;
							break;
						case 4:
							menustate = 7;
							break;
					}
				}else if (menustate == 2){
					state=1;
					difficulty=selectedDificulty;
					restart();
				}else if (menustate == 6){
					player.setSkin(selectedSubMenuOption);
					save.setPlayerSkin(selectedSubMenuOption);
					save.storeSaveData("/3ds/AoTSJSave.xml");
				}
			}
			if (kDown & KEY_B){
				online.setLoaded(false);
				if (menustate == 1){
					menustate=0;
				}
				if (menustate > 1)
					menustate=1;
			}
			if (menustate == 2){
				if (kDown & KEY_DOWN){
					selectedDificulty++;
					if (selectedDificulty > 2)
						selectedDificulty=0;
				}
				if (kDown & KEY_UP){
					selectedDificulty--;
					if (selectedDificulty < 0)
						selectedDificulty=2;
				}
			}
			if (menustate == 1){
				if (kDown & KEY_DOWN){
					selectedMenuOption++;
					if (selectedMenuOption > 4)
						selectedMenuOption=0;
				}
				if (kDown & KEY_UP){
					selectedMenuOption--;
					if (selectedMenuOption < 0)
						selectedMenuOption=4;
				}
			}
		}
		if (menustate == 3){
			if (kDown & KEY_RIGHT || kDown & KEY_LEFT){
				if (highScoreState == 0)
					highScoreState=1;
				else
					highScoreState=0;
			}
		}
		if (menustate == 4){
			if (kDown & KEY_RIGHT){
				curPage++;
				if (curPage > online.getPageCount()-1){
					curPage=0;
				}
			}
			if (kDown & KEY_LEFT){
				curPage--;
				if (curPage < 0){
					curPage=online.getPageCount()-1;
				}
			}
		}
		if (menustate == 6){
			if (kDown & KEY_LEFT){
				selectedSubMenuOption--;
				if (selectedSubMenuOption < 0)
					selectedSubMenuOption=tex_player->width/20-1;
			}
			if (kDown & KEY_RIGHT){
				selectedSubMenuOption++;
				if (selectedSubMenuOption >= tex_player->width/20)
					selectedSubMenuOption=0;
			}
			if (kDown & KEY_DOWN){
				selectedSubMenuOption+=6;
				if (selectedSubMenuOption >= tex_player->width/20)
					selectedSubMenuOption=selectedSubMenuOption%6;
			}
			if (kDown & KEY_UP){
				selectedSubMenuOption-=6;
				if (selectedSubMenuOption < 0){
					selectedSubMenuOption=tex_player->width/20+selectedSubMenuOption;
				}
			}
		}
		
		if (kDown & KEY_START) break; // break in order to return to hbmenu
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
	aptExit();
	return 0;
}
