#include "Physics.h"

void Physics::ApplyPhysics(vector<Entity*>* entities, vector<WorldBlock*>* world)
{
	for (vector<Entity*>::iterator ent = entities->begin(); ent != entities->end(); ++ent)
	{
		Collide((*ent), world);
		ApplyGravity((*ent));
	}
}


//TODO: Check if it's inside of the item
void Physics::Collide(Entity* ent, vector<WorldBlock*>* world)
{
	int worldItems = world->size();
	Coordinates* offset = VectorToOffset(ent->GetVelocityVector());

	const Coordinates* entA = ent->GetCoordinates();
	Coordinates* entB = new Coordinates(entA->X + ent->GetWidth(), entA->Y + ent->GetHeight());

	vector<float> xCol;
	vector<float> yCol;

	for (vector<WorldBlock*>::iterator wBlock = world->begin(); wBlock != world->end(); ++wBlock)
	{
		const Coordinates* worA = (*wBlock)->GetA();
		const Coordinates* worB = (*wBlock)->GetB();

		xCol.push_back(-1);
		yCol.push_back(-1);

		if (offset->Y > 0.0)
		{
			if ((entB->Y + offset->Y > worA->Y) && (entB->Y <= worA->Y) && (entB->X + offset->X > worA->X) & (entA->X + offset->X < worB->X))
			{
				if (ceil(entB->Y) == worA->Y)
				{
					yCol[yCol.size() - 1] = 0.0;
					(*wBlock)->SetColor(al_map_rgb(20, 220, 20));
				}
				else
				{
					for (float y = fmod(offset->Y, 1.0); y <= offset->Y; ++y)
					{
						if (ceil(entB->Y + y) >= worA->Y)
						{
							if (ceil(entB->Y + y) == worA->Y)
							{
								(*wBlock)->SetColor(al_map_rgb(20, 220, 20));
								yCol[yCol.size() - 1] = y;
								break;
							}
						}
					}
				}
			}
		}
		else
		{
			if ((entA->Y + offset->Y < worB->Y) && (entA->Y >= worB->Y) && (entB->X + offset->X > worA->X) && (entA->X + offset->X < worB->X))
			{
				if (ceil(entA->Y) == worB->Y)
				{
					(*wBlock)->SetColor(al_map_rgb(20, 220, 20));
					yCol[yCol.size() - 1] = 0.0;
				}
				else
				{
					for (float y = fmod(offset->Y, 1.0); y >= offset->Y; --y)
					{
						if (ceil(entA->Y + y) <= worB->Y)
						{
							if (ceil(entA->Y + y) == worB->Y)
							{
								(*wBlock)->SetColor(al_map_rgb(20, 220, 20));
								yCol[yCol.size() - 1] = y;
								break;
							}
						}
					}
				}
			}
		}

		if (offset->X >= 0.0)
		{
			if ((entB->X + offset->X > worA->X) && (entB->X <= worA->X) && (entB->Y + offset->Y > worA->Y) && (entA->Y + offset->Y < worB->Y))
			{
				if (ceil(entB->X) == worA->X)
				{
					(*wBlock)->SetColor(al_map_rgb(20, 220, 20));
					xCol[xCol.size() - 1] = 0.0;
				}
				else
				{
					for (float x = fmod(offset->X, 1.0); x <= offset->X; ++x)
					{
						if (ceil(entB->X + x) >= worA->X)
						{
							if (ceil(entB->X + x) == worA->X)
							{
								(*wBlock)->SetColor(al_map_rgb(20, 220, 20));
								xCol[xCol.size() - 1] = x;
								break;
							}
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
					(*wBlock)->SetColor(al_map_rgb(20, 220, 20));
					xCol[xCol.size() - 1] = 0.0;
				}
				else
				{
					for (float x = fmod(offset->X, 1.0); x >= offset->X; --x)
					{
						if (worB->X >= ceil(entA->X + x))
						{
							if (ceil(entA->X + x) == worB->X)
							{
								(*wBlock)->SetColor(al_map_rgb(20, 220, 20));
								xCol[xCol.size() - 1] = x;
								break;
							}
						}
					}
				}
			}
		}
	}

	int closestX = -1;
	int closestY = -1;
	float distanceX = ent->GetVelocityVector()->Velocity;
	float distanceY = ent->GetVelocityVector()->Velocity;

	for (int i = 0; i < worldItems; ++i)
	{
		if (yCol[i] != -1 && xCol[i] != -1)
		{
			//Hiero is dat problemo yo
			if (count(yCol.begin(), yCol.end(), 0.0) > 0)
			{
				distanceY = sqrt(pow(yCol[i] / offset->Y * offset->X, 2) + pow(ceil(yCol[i]), 2));
				closestY = i;
				closestX = -1;

				break;
			}
			else if (count(xCol.begin(), xCol.end(), 0.0) > 0)
			{
				distanceX = sqrt(pow(ceil(xCol[i]), 2) + pow(xCol[i] / offset->X * offset->Y, 2));
				closestX = i;
				closestY = -1;

				break;
			}

			cout << "wut, bork?";
		}

		if (yCol[i] != -1)
		{
			if (sqrt(pow(yCol[i] / offset->Y * offset->X, 2) + pow(ceil(yCol[i]), 2)) < distanceY)
			{
				distanceY = sqrt(pow(yCol[i] / offset->Y * offset->X, 2) + pow(ceil(yCol[i]), 2));
				closestY = i;
			}
		}

		if (xCol[i] != -1)
		{
			if (sqrt(pow(ceil(xCol[i]), 2) + pow(xCol[i] / offset->X * offset->Y, 2)) < distanceX)
			{
				distanceX = sqrt(pow(ceil(xCol[i]), 2) + pow(xCol[i] / offset->X * offset->Y, 2));
				closestX = i;
			}
		}
	}

	if (closestX != -1 && closestY != -1)
	{
		ent->SetCoordinates(ceil(xCol[closestX] + entA->X), ceil(yCol[closestY] + entA->Y));

		ent->SetVelocityVector(0.0, 0.0);
	}
	else if (closestY != -1)
	{
		ent->SetCoordinates(yCol[closestY] / offset->Y * offset->X + entA->X, ceil(yCol[closestY] + entA->Y));

		ent->SetVelocityVector(OffsetToVector(offset->X * FRICTION, 0.0));
	}
	else if (closestX != -1)
	{
		ent->SetCoordinates(ceil(xCol[closestX] + entA->X), xCol[closestX] / offset->X * offset->Y + entA->Y);

		ent->SetVelocityVector(OffsetToVector(0.0, offset->Y * FRICTION));
	}
	
	delete offset;
	delete entB;
}

void Physics::ApplyGravity(Entity* ent)
{
	Coordinates* offset = VectorToOffset(ent->GetVelocityVector());

	ent->MoveToOffset(offset->X, offset->Y);

	ent->SetVelocityVector(OffsetToVector(offset->X, offset->Y + GRAVITY));

	delete offset;
}

//TODO: M8, I think there is a bug in here
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
