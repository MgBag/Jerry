#pragma once

class VelocityVector
{
public:
	VelocityVector();
	VelocityVector(float velocity, float angle);
	~VelocityVector();

	float Velocity;
	float Angle;
};

