#pragma once

const double PHYSICS_TICK = 60;
const double FPS = 60;
const int SCREEN_W = 1280;
const int SCREEN_H = 600;
const double GRAVITY = 9.8 / FPS;
const double FRICTION = 0.8;
const double FM_PI_2 = 3.1415926535897932384626433832795 / 2;
const double FM_3_PI_2 = 3 * FM_PI_2;
const double FM_PI = 2 * FM_PI_2;
const double PLAYER_SPEED = 1.0;
const double PRECISION = 0.0001;
const double FRICTION_STOP = 0.1;
const double PROJECTILE_SPEED = 10.0;
const double PROJECTILE_SIZE = 6.0;
const double BOUNCINESS = 0.9;
const double MAX_PARTICLES = 3000;
const double MAX_PARTICLE_AGE = 10.0 * PHYSICS_TICK;
const double AIR_CONTROL = 0.1;
const double PLAYER_JUMP_SPEED = 3.0;
const double MAX_VELOCITY = 60.0;

extern int ActiveParticles;
extern int Particles;

enum CollPos 
{
	LX, RX, UY, DY
};

enum UDLR
{
	RIGHT, UP, LEFT, DOWN
};

enum EntityType 
{
	PLAYER, PROJECTILE, WORLD, JELLY
};