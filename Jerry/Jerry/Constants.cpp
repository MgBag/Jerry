#include "Constants.h"


double	FPS = 60;
double	PHYSICS_TICK = 60;
int		SCREEN_W = 1280;
int		SCREEN_H = 600;
double	GRAVITY = 10 / FPS;
double	PLAYER_SPEED = 1.0;
double	PLAYER_JUMP_SPEED = 3.0;
double	PLAYER_BOUNCE_OFFSET = 5.7;
double	PLAYER_AIR_CONTROL = 0.1;
double	PLAYER_AIR_CONTROL_BREAK = 0.75;
double	PLAYER_AIR_CONTROL_STOP = 1.0;
double	PROJECTILE_SPEED = 10.0;
double	PROJECTILE_SIZE = 6.0;
int		PROJECTILE_TRAIL_LENGTH = 2;
double	PROJECTILE_BOUNCINESS = 1.0;
double	MAX_ENTITIES = 4;
double	MAX_ENTITY_AGE = 10.0 * PHYSICS_TICK;
double	MAX_ENTITY_VELOCITY = 60.0;
int		MAX_COLLISION_PREDICTION = 500;
double	FRICTION = 0.8;
double	FRICTION_STOP = 0.1;
double	FM_PI_2 = 3.1415926535897932384626433832795 / 2;
double	FM_3_PI_2 = 3 * FM_PI_2;
double	FM_PI = 2 * FM_PI_2;
double	PRECISION = 0.01;
int		MAX_COLLISION_RECURSION = 10;
double	PLAYER_SIDE_UP_BOUNCE = 1.5;
double	PLAYER_SIDE_SIDE_BOUNCE = 1.5;
bool	FLY = false;
bool	CLEAR_DRAW = true;
bool	DRAW_PREDICTION = false;

int ScoreShow = 25 * FPS;

int ActiveParticles = 0;
int Particles = 0;
int StackOverflowProtection = 0;
int Score = 0;


ALLEGRO_COLOR PlayerColor = al_map_rgb(20, 20, 220);
ALLEGRO_COLOR JellyColor = al_map_rgb(20, 220, 20);
ALLEGRO_COLOR WorldColor = al_map_rgb(20, 20, 20);
ALLEGRO_COLOR BadWorldColor = al_map_rgb(220, 20, 20);
ALLEGRO_COLOR JellyWorldColor = al_map_rgb(20, 20, 220);
ALLEGRO_COLOR CoinColor = al_map_rgb(255, 140, 0);
ALLEGRO_EVENT_QUEUE* EventQueue = 0;
ALLEGRO_EVENT_SOURCE UserEventSource;


Coordinates Spawn = Coordinates(40, 400);