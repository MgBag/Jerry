#pragma once
#include <allegro5/allegro.h>

const double FPS = 60;
const double PHYSICS_TICK = 60;
const int	 SCREEN_W = 1280;
const int	 SCREEN_H = 600;
const double GRAVITY = 9.8 / FPS;
const double PLAYER_SPEED = 1.0;
const double PLAYER_JUMP_SPEED = 3.0;
const double PLAYER_BOUNCE_OFFSET = 6.0;
const double PLAYER_AIR_CONTROL = 0.1;
const double PLAYER_AIR_CONTROL_BREAK = 0.9;
const double PROJECTILE_SPEED = 10.0;
const double PROJECTILE_SIZE = 6.0;
const int	 PROJECTILE_TRAIL_LENGTH = 2;
const double PROJECTILE_BOUNCINESS = 1.0;
const double MAX_ENTITIES = 500;
const double MAX_ENTITY_AGE = 10.0 * PHYSICS_TICK;
const double MAX_ENTITY_VELOCITY = 60.0;
const int	 MAX_COLLISION_PREDICTION = 500;
const double FRICTION = 0.8;
const double FRICTION_STOP = 0.1;
const double FM_PI_2 = 3.1415926535897932384626433832795 / 2;
const double FM_3_PI_2 = 3 * FM_PI_2;
const double FM_PI = 2 * FM_PI_2;
const double PRECISION = 0.0001;

extern int ActiveParticles;
extern int Particles;
extern ALLEGRO_COLOR PlayerColor;
extern ALLEGRO_COLOR JellyColor;
extern ALLEGRO_COLOR WorldColor;
extern ALLEGRO_COLOR BadWorldColor;

enum CollPos 
{
	LX, RX, UY, DY
};

enum ItemType 
{
	PLAYER, PROJECTILE, WORLD, JELLY, BADWORLD
};

enum KEYS
{
	RIGHT, UP, LEFT, DOWN, LCTRL
};

enum MOUSEBUTTONS
{
	LMB, RMB, MIDDLEMOUSE
};