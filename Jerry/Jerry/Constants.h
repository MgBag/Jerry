#pragma once

const int FPS = 60;
const int SCREEN_W = 1280;
const int SCREEN_H = 600;
const float GRAVITY = 9.8 / FPS;
const float FRICTION = 0.8;
const float FM_3_PI_2 = 3 * M_PI_2;
const float FM_PI = 2 * M_PI_2;
const float FM_PI_2 = M_PI_2;
const float PLAYER_SPEED = 1.0;
const float PRECISION = 0.01;
const float FRICTION_STOP = 0.1;
const float PROJECTILE_SPEED = 10.0;
const float PROJECTILE_SIZE = 5.0;
const float PARTICLE_MAX = 900;
const float BOUNCINESS = 0.9;
 
enum CollPos {
	LX, RX, UY, DY
};

enum UDLR
{
	RIGHT, UP, LEFT, DOWN
};

enum EntityType {
	PLAYER, PROJECTILE, WORLD, JELLY
};