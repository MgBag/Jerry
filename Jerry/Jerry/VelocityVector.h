#pragma once

#include "Coordinates.h"
#include "Entity.h"

class VelocityVector
{
public:
	VelocityVector();
	VelocityVector(float velocity, float angle);
	~VelocityVector();


	float Velocity;
	float Angle;
};

