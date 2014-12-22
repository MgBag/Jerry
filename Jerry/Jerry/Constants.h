#pragma once

const int FPS = 60;
const int SCREEN_W = 1280;
const int SCREEN_H = 600;
const float GRAVITY = 0.150; // 10.0 / FPS;
const float FRICTION = 0.9;
const float FM_3_PI_2 = 3 * M_PI_2;
const float FM_PI = 2 * M_PI_2;
const float FM_PI_2 = M_PI_2;
const float PLAYER_SPEED = 1.0;
const float PRECISION = 0.001;
const float FRICTION_STOP = 0.1;

enum CollType {
	X, Y, XY
};

enum UDLR
{
	RIGHT, UP, LEFT, DOWN
};