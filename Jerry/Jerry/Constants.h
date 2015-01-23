#pragma once

#include <mutex>
#include <allegro5/allegro.h>
#include "Coordinates.h"

extern double FPS;
extern double PHYSICS_TICK;
extern int    SCREEN_W;
extern int    SCREEN_H;
extern double GRAVITY;
extern double PLAYER_SPEED;
extern double PLAYER_JUMP_SPEED;
extern double PLAYER_BOUNCE_OFFSET;
extern double PLAYER_AIR_CONTROL;
extern double PLAYER_AIR_CONTROL_BREAK; 
extern double PLAYER_SIDE_UP_BOUNCE;
extern double PLAYER_SIDE_SIDE_BOUNCE;
extern double PROJECTILE_SPEED;
extern double PROJECTILE_SIZE;
extern int    PROJECTILE_TRAIL_LENGTH;
extern double PROJECTILE_BOUNCINESS;
extern double MAX_ENTITIES;
extern double MAX_ENTITY_AGE;
extern double MAX_ENTITY_VELOCITY;
extern int    MAX_COLLISION_PREDICTION;
extern double FRICTION;
extern double FRICTION_STOP;
extern double FM_PI_2;
extern double FM_3_PI_2;
extern double FM_PI;
extern double PRECISION;
extern int    MAX_COLLISION_RECURSION;
extern bool   FLY;

extern int ActiveParticles;
extern int Particles;
extern ALLEGRO_COLOR PlayerColor;
extern ALLEGRO_COLOR JellyColor;
extern ALLEGRO_COLOR WorldColor;
extern ALLEGRO_COLOR BadWorldColor;
extern ALLEGRO_COLOR JellyWorldColor;
extern int StackOverflowProtection;
extern Coordinates Spawn;


// Evaluated from enity
enum CollPos 
{
	LX, RX, UY, DY
};

enum ItemType 
{
	PLAYER, PROJECTILE, WORLD, JELLY, BADWORLD, JELLYWORLD
};

enum KEYS
{
	RIGHT, UP, LEFT, DOWN, LCTRL
};

enum MOUSEBUTTONS
{
	LMB, RMB, MIDDLEMOUSE
};