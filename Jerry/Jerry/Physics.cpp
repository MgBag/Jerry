#include "Physics.h"

void Physics::ApplyPhysics(vector<Entity*>* entities, vector<WorldBlock*>* world)
{
	for (vector<Entity*>::iterator ent = entities->begin(); ent != entities->end(); ++ent)
	{
		if (!WillCollide((*ent), world))
		{
			ApplyGravity((*ent));
		}
	}
}

// TODO: Rounding?
// TODO: Maybe undo rounding? It might be causing the stutter
// TODO: Remove the rounding, it needs not to be included in the actual move, only for the calculations Allegro will make it pixel perfect.
// TODO: Theoretical collision glitch due to array order. Needs more work and testing, movement first though
bool Physics::WillCollide(Entity* ent, vector<WorldBlock*>* world)
{
	Coordinates* offset = VectorToOffset(ent->GetVelocityVector());

	const Coordinates* entA = ent->GetCoordinates();
	Coordinates* entB = new Coordinates(entA->X + ent->GetWidth(), entA->Y + ent->GetHeight());

	vector<float> xCol;
	vector<float> yCol;
	vector<bool> isXCol;
	vector<bool> isYCol;

	for (vector<WorldBlock*>::iterator wBlock = world->begin(); wBlock != world->end(); ++wBlock)
	{
		const Coordinates* worA = (*wBlock)->GetA();
		const Coordinates* worB = (*wBlock)->GetB();

		xCol.push_back(-1);
		yCol.push_back(-1);
		isYCol.push_back(false);
		isXCol.push_back(false);

		if (offset->Y > 0.0)
		{
			if ((entB->Y + offset->Y > worA->Y) && (entB->Y <= worA->Y) && (entB->X + offset->X > worA->X) & (entA->X + offset->X < worB->X))
			{
				if (ceil(entB->Y) >= worA->Y)
				{
					yCol[yCol.size() - 1] = 0.0;

					if (ceil(entB->Y) == worA->Y)
					{
						isYCol[isYCol.size() - 1] = true;
					}
				}
				else
				{
					for (float y = fmod(offset->Y, 1.0); y <= offset->Y; ++y)
					{
						if (ceil(entB->Y + y) >= worA->Y)
						{
							yCol[yCol.size() - 1] = y;

							if (ceil(entB->Y + y) == worA->Y)
							{
								isYCol[isYCol.size() - 1] = true;
							}

							break;
						}
					}
				}
			}
		}
		else if (offset->Y < 0.0)
		{
			if ((entA->Y + offset->Y < worB->Y) && (entA->Y >= worB->Y) && (entB->X + offset->X > worA->X) && (entA->X + offset->X < worB->X))
			{
				if (ceil(entA->Y) <= worB->Y)
				{
					yCol[yCol.size() - 1] = 0.0;

					if (ceil(entA->Y) == worB->Y)
					{
						isYCol[isYCol.size() - 1] = true;
					}
				}
				else
				{
					for (float y = fmod(offset->Y, 1.0); y >= offset->Y; --y)
					{
						if (ceil(entA->Y + y) <= worB->Y)
						{
							yCol[yCol.size() - 1] = y;

							if (ceil(entA->Y + y) == worB->Y)
							{
								isYCol[isYCol.size() - 1] = true;
							}

							break;
						}
					}
				}
			}
		}

		if (!isYCol[isYCol.size() - 1])
		{
			if (offset->X >= 0.0)
			{
				if ((entB->X + offset->X > worA->X) && (entB->X <= worA->X) && (entB->Y + offset->Y > worA->Y) && (entA->Y + offset->Y < worB->Y))
				{
					if (ceil(entB->X) == worA->X)
					{
						xCol[xCol.size() - 1] = 0.0;

						if (ceil(entB->X) == worA->X)
						{
							isXCol[isXCol.size() - 1] = true;
						}
					}
					else
					{
						for (float x = fmod(offset->X, 1.0); x <= offset->X; ++x)
						{
							if (ceil(entB->X + x) >= worA->X)
							{
								xCol[xCol.size() - 1] = x;
								
								if (ceil(entB->X + x) == worA->X)
								{
									isXCol[isXCol.size() - 1] = true;
								}

								break;
							}
						}
					}
				}
			}
			else
			{
				if ((entA->X + offset->X < worB->X) && (entA->X >= worB->X) && (entB->Y + offset->Y > worA->Y) && (entA->Y + offset->Y < worB->Y))
				{
					if (ceil(entA->X) == worB->X)
					{
						xCol[xCol.size() - 1] = 0.0;

						if (ceil(entA->X) == worB->X)
						{
							isXCol[isXCol.size() - 1] = true;
						}
					}
					else
					{
						for (float x = fmod(offset->X, 1.0); x >= offset->X; --x)
						{
							if (worB->X >= ceil(entA->X + x))
							{
								xCol[xCol.size() - 1] = x;

								if (ceil(entA->X + x) == worB->X)
								{
									isXCol[isXCol.size() - 1] = true;
								}

								break;
							}
						}
					}
				}
			}
		}
	}

	int closest = -1;
	float distance = ent->GetVelocityVector()->Velocity;

	for (int i = 0; i < xCol.size(); ++i)
	{
		if (isYCol[i])
		{
			if (sqrt(pow(yCol[i] / offset->Y * offset->X, 2) + pow(ceil(yCol[i]), 2)) < distance)
			{
				distance = sqrt(pow(yCol[i] / offset->Y * offset->X, 2) + pow(ceil(yCol[i]), 2));
				closest = i;
			}
		}
		
		if (isXCol[i])
		{
			if (sqrt(pow(ceil(xCol[i]), 2) + pow(xCol[i] / offset->X * offset->Y, 2)) < distance)
			{
				distance = sqrt(pow(ceil(xCol[i]), 2) + pow(xCol[i] / offset->X * offset->Y, 2));
				closest = i;
			}
		}
	}

	if (closest != -1)
	{
		if (isYCol[closest] || isXCol[closest])
		{
			if (isYCol[closest])
			{
				ent->SetCoordinates(yCol[closest] / offset->Y * offset->X + entA->X, ceil(yCol[closest] + entA->Y));

				ent->SetVelocityVector(OffsetToVector(offset->X * FRICTION, 0.0));
			}
			else if (isXCol[closest])
			{
				ent->SetCoordinates(ceil(xCol[closest] + entA->X), xCol[closest] / offset->X * offset->Y + entA->Y);

				ent->SetVelocityVector(OffsetToVector(0.0, offset->Y * FRICTION));
			}

			delete offset;
			delete entB;

			return true;
		}
	}

	delete offset;
	delete entB;

	return false;
}

void Physics::ApplyGravity(Entity* ent)
{
	Coordinates* offset = VectorToOffset(ent->GetVelocityVector());

	ent->MoveToOffset(offset->X, offset->Y);

	ent->SetVelocityVector(OffsetToVector(offset->X, offset->Y + GRAVITY));

	delete offset;
}

VelocityVector* Physics::OffsetToVector(float x, float y)
{
	VelocityVector* vec = new VelocityVector();

	vec->Velocity = sqrt(x * x + y * y);

	if (x == 0.0 && y > 0.0)
	{
		vec->Angle = FM_PI;
	}
	else if (x == 0.0 && y < 0.0)
	{
		vec->Angle = 0.0;
	}
	else if (x > 0.0 && y == 0.0)
	{
		vec->Angle = FM_3_PI_2;
	}
	else if (x < 0.0 && y == 0.0)
	{
		vec->Angle = FM_PI_2;
	}
	else if (x < 0.0 && y < 0.0)
	{
		vec->Angle = atan((x * -1.0) / (y * -1.0));
	}
	else if (x < 0.0 && y > 0.0)
	{
		vec->Angle = atan(y / (x * -1.0)) + FM_PI_2;
	}
	else if (x > 0.0 && y > 0.0)
	{
		vec->Angle = atan(x / y) + FM_PI;
	}
	else if (x > 0.0 && y < 0.0)
	{
		vec->Angle = atan((y * -1.0) / x) + FM_3_PI_2;
	}

	return vec;
}

VelocityVector* Physics::OffsetToVector(Coordinates* coordinates)
{
	float x = coordinates->X;
	float y = coordinates->Y;

	VelocityVector* vec = new VelocityVector();

	vec->Velocity = sqrt(x * x + y * y);

	if (x == 0.0 && y > 0.0)
	{
		vec->Angle = FM_PI;
	}
	else if (x == 0.0 && y < 0.0)
	{
		vec->Angle = 0.0;
	}
	else if (x > 0.0 && y == 0.0)
	{
		vec->Angle = FM_3_PI_2;
	}
	else if (x < 0.0 && y == 0.0)
	{
		vec->Angle = FM_PI_2;
	}
	else if (x < 0.0 && y < 0.0)
	{
		vec->Angle = atan((x * -1.0) / (y * -1.0));
	}
	else if (x < 0.0 && y > 0.0)
	{
		vec->Angle = atan(y / (x * -1.0)) + FM_PI_2;
	}
	else if (x > 0.0 && y > 0.0)
	{
		vec->Angle = atan(x / y) + FM_PI;
	}
	else if (x > 0.0 && y < 0.0)
	{
		vec->Angle = atan((y * -1.0) / x) + FM_3_PI_2;
	}

	return vec;
}

Coordinates* Physics::VectorToOffset(float velocity, float angle)
{
	Coordinates *offset = new Coordinates();

	if (angle == 0.0)
	{
		offset->X = 0.0;
		offset->Y = velocity * -1;
	}
	else if (fmod(angle, FM_3_PI_2) == 0.0)
	{
		offset->X = velocity;
		offset->Y = 0.0;
	}
	else if (fmod(angle, FM_PI) == 0.0)
	{
		offset->X = 0.0;
		offset->Y = velocity;
	}
	else if (fmod(angle, FM_PI_2) == 0.0)
	{
		offset->X = velocity * -1;
		offset->Y = 0.0;
	}
	else if (angle > FM_3_PI_2)
	{
		offset->X = cos(angle - FM_3_PI_2) * velocity;
		offset->Y = sin(angle - FM_3_PI_2) * velocity * -1;
	}
	else if (angle > FM_PI)
	{
		offset->X = sin(angle - FM_PI) * velocity;
		offset->Y = cos(angle - FM_PI) * velocity;
	}
	else if (angle > FM_PI_2)
	{
		offset->X = cos(angle - FM_PI_2) * velocity * -1;
		offset->Y = sin(angle - FM_PI_2) * velocity;
	}
	else
	{
		offset->X = sin(angle) * velocity * -1;
		offset->Y = cos(angle) * velocity * -1;
	}

	return offset;
}

Coordinates* Physics::VectorToOffset(VelocityVector* vec)
{
	Coordinates *offset = new Coordinates();

	float velocity = vec->Velocity;
	float angle = vec->Angle;

	if (angle == 0.0)
	{
		offset->X = 0.0;
		offset->Y = velocity * -1;
	}
	else if (fmod(angle, FM_3_PI_2) == 0.0)
	{
		offset->X = velocity;
		offset->Y = 0.0;
	}
	else if (fmod(angle, FM_PI) == 0.0)
	{
		offset->X = 0.0;
		offset->Y = velocity;
	}
	else if (fmod(angle, FM_PI_2) == 0.0)
	{
		offset->X = velocity * -1;
		offset->Y = 0.0;
	}
	else if (angle > FM_3_PI_2)
	{
		offset->X = cos(angle - FM_3_PI_2) * velocity;
		offset->Y = sin(angle - FM_3_PI_2) * velocity * -1;
	}
	else if (angle > FM_PI)
	{
		offset->X = sin(angle - FM_PI) * velocity;
		offset->Y = cos(angle - FM_PI) * velocity;
	}
	else if (angle > FM_PI_2)
	{
		offset->X = cos(angle - FM_PI_2) * velocity * -1;
		offset->Y = sin(angle - FM_PI_2) * velocity;
	}
	else
	{
		offset->X = sin(angle) * velocity * -1;
		offset->Y = cos(angle) * velocity * -1;
	}

	return offset;
}
