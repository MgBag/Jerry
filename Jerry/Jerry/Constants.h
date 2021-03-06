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
// Air break multiplier used to decrease the speed 
extern double PLAYER_AIR_CONTROL_BREAK;
// Precision to stop the player movement in air
extern double PLAYER_AIR_CONTROL_STOP;
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
// World friction
extern double FRICTION;
// Varibale used to stop if the speed is within range
extern double FRICTION_STOP;
extern double FM_PI_2;
extern double FM_3_PI_2;
extern double FM_PI;
extern double PRECISION;
extern int    MAX_COLLISION_RECURSION;
extern bool   FLY;
extern bool   CLEAR_DRAW;
extern bool   DRAW_PREDICTION;

extern int ScoreShow;
extern int Score;
extern int ActiveParticles;
extern int Particles;
extern ALLEGRO_COLOR PlayerColor;
extern ALLEGRO_COLOR JellyColor;
extern ALLEGRO_COLOR WorldColor;
extern ALLEGRO_COLOR BadWorldColor;
extern ALLEGRO_COLOR JellyWorldColor;
extern ALLEGRO_COLOR CoinColor;
extern int StackOverflowProtection;
extern Coordinates Spawn;
extern ALLEGRO_EVENT_QUEUE* EventQueue;
extern ALLEGRO_EVENT_SOURCE UserEventSource;

// Evaluated from enity
enum CollPos 
{
	LX, RX, UY, DY
};

enum ItemType 
{
	PLAYER, PROJECTILE, WORLD, JELLY, BADWORLD, JELLYWORLD, COIN, SCENETEXT
};

enum KEYS
{
	RIGHT, UP, LEFT, DOWN, LCTRL
};

enum MOUSEBUTTONS
{
	LMB, RMB, MIDDLEMOUSE
};

enum AUDIOTYPES
{
	PLAYER_WORLD, PLAYER_JELLY, PLAYER_BADWORLD, JELLY_JELLY, JELLY_JELLYWORLD, JELLY_NOTJELLYWORLD, PLAYER_COIN, PLAYER_SHOOT
};
