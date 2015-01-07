#pragma once

class VelocityVector
{
public:
	VelocityVector();
	VelocityVector(double velocity, double angle);
	~VelocityVector();

	double Velocity;
	double Angle;
};

