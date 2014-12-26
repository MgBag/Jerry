#include "Physics.h"

// TODO: Memory leaks mang
// TODO: Check logics mang, flows and flaws

void Physics::ApplyPhysics(list<Entity>* entities, list<WorldBlock>* world)
{
	for (list<Entity>::iterator ent = entities->begin(); ent != entities->end(); ++ent)
	{
		ApplyGravity(&(*ent));
		Collide(&(*ent), world, entities);
		MoveEntity(&(*ent));
	}
}

void Physics::ApplyGravity(Entity* ent)
{
	if (ent->getType() == PLAYER || !ent->GetHit())
	{
		Coordinates* offset = VectorToOffset(ent->GetVelocityVector());

		ent->SetVelocityVector(OffsetToVector(offset->X, offset->Y + GRAVITY));

		delete offset;
	}
}

void Physics::MoveEntity(Entity* ent)
{
	Coordinates* offset = VectorToOffset(ent->GetVelocityVector());

	ent->MoveToOffset(offset->X, offset->Y);

	delete offset;
}

void Physics::Collide(Entity* ent, list<WorldBlock>* world, list<Entity>* entities)
{
	Coordinates* entACo = ent->GetCoordinates();
	Coordinates* entBCo = new Coordinates(entACo->X + ent->GetWidth(), entACo->Y + ent->GetHeight());
	Coordinates* entOff = VectorToOffset(ent->GetVelocityVector()); // TODO: Add entity offset conlone function;
	float entVel = ent->GetVelocityVector()->Velocity;
	vector<Coordinates> collisions;
	vector<Coordinates> possitions;
	vector<int> collisionPosition;
	vector<int> collisionType;
	vector<WorldBlock*> collisionBlock;

	// To make sure that a step isn't 1.0 if the offset is equal to teh velocity
	float xStep = entOff->Y == 0 ? entOff->X / (entOff->X * (entOff->X < 0.0 ? -10.0 : 10.0)) : entOff->X / ceil(entVel);
	float yStep = entOff->X == 0 ? entOff->Y / (entOff->Y * (entOff->Y < 0.0 ? -10.0 : 10.0)) : entOff->Y / ceil(entVel);

	for (list<Entity>::iterator jel = ++entities->begin(); jel != entities->end(); ++jel)
	{
		if (jel->GetHit() && ent != &*jel)
		{
			Coordinates* jelACo = jel->GetCoordinates();
			Coordinates* jelBCo = new Coordinates(jelACo->X + jel->GetWidth(), jelACo->Y + jel->GetHeight());
			float minOffsetX;
			float minOffsetY;
			float maxOffsetX;
			float maxOffsetY;
			bool minXIsEnt = false;
			bool minYIsEnt = false;

			for (float x = 0, y = 0; (xStep < 0.0 ? x >= entOff->X : x <= entOff->X) && (yStep < 0.0 ? y >= entOff->Y : y <= entOff->Y); x += xStep, y += yStep)
			{
				// X with Offset
				if (jelACo->X < entACo->X + x)
				{
					minOffsetX = entACo->X + x;
					minXIsEnt = true;

					maxOffsetX = jelBCo->X;
				}
				else
				{
					minOffsetX = jelACo->X;

					maxOffsetX = entBCo->X + x;
				}

				//Y	with offset
				if (jelACo->Y < entACo->Y + y)
				{
					minOffsetY = entACo->Y + y;
					minYIsEnt = true;

					maxOffsetY = jelBCo->Y;
				}
				else
				{
					minOffsetY = jelACo->Y;

					maxOffsetY = entBCo->Y + y;
				}

				// Will Collide
				if (maxOffsetX > minOffsetX && maxOffsetY > minOffsetY)
				{
					if (!(maxOffsetX - (minXIsEnt ? 0.0 : (xStep < 0.0 ? xStep - PRECISION : xStep + PRECISION)) > minOffsetX - (minXIsEnt ? (xStep < 0.0 ? xStep - PRECISION : xStep + PRECISION) : 0.0)) && maxOffsetY > minOffsetY)
					{
						if (minXIsEnt)
						{
							possitions.push_back(Coordinates(jelBCo->X, entACo->Y + y));
							collisionPosition.push_back(LX);
						}
						else
						{
							possitions.push_back(Coordinates(jelACo->X - ent->GetWidth(), entACo->Y + y));
							collisionPosition.push_back(RX);
						}

					}
					else if (maxOffsetX > minOffsetX && !(maxOffsetY - (minYIsEnt ? 0.0 : (yStep < 0.0 ? yStep - PRECISION : yStep + PRECISION)) > minOffsetY - (minYIsEnt ? (yStep < 0.0 ? yStep - PRECISION : yStep + PRECISION) : 0.0)))
					{
						if (minYIsEnt)
						{
							possitions.push_back(Coordinates(entACo->X + x, jelBCo->Y));
							collisionPosition.push_back(DY);
						}
						else
						{
							possitions.push_back(Coordinates(entACo->X + x, jelACo->Y - ent->GetHeight()));
							collisionPosition.push_back(UY);
						}

					}

					// Temp work around
					collisionBlock.push_back(new WorldBlock(jelACo->X, jelACo->Y, jelBCo->X, jelBCo->Y, al_map_rgb(220, 20, 20)));
					collisions.push_back(Coordinates(x, y));
					collisionType.push_back(JELLY);

					break;
				}
			}

			delete jelBCo;
		}
	}

	for (list<WorldBlock>::iterator wor = world->begin(); wor != world->end(); ++wor)
	{
		// TODO: Add might collide
		if (true)
		{
			Coordinates* worACo = wor->GetA();
			Coordinates* worBCo = wor->GetB();
			float minOffsetX;
			float minOffsetY;
			float maxOffsetX;
			float maxOffsetY;
			bool minXIsEnt = false;
			bool minYIsEnt = false;

			

			for (float x = 0, y = 0; (xStep < 0.0 ? x >= entOff->X : x <= entOff->X) && (yStep < 0.0 ? y >= entOff->Y : y <= entOff->Y); x += xStep, y += yStep)
			{
				// X with Offset
				if (worACo->X < entACo->X + x)
				{
					minOffsetX = entACo->X + x;
					minXIsEnt = true;

					maxOffsetX = worBCo->X;
				}
				else
				{
					minOffsetX = worACo->X;

					maxOffsetX = entBCo->X + x;
				}

				//Y	with offset
				if (worACo->Y < entACo->Y + y)
				{
					minOffsetY = entACo->Y + y;
					minYIsEnt = true;

					maxOffsetY = worBCo->Y;
				}
				else
				{
					minOffsetY = worACo->Y;

					maxOffsetY = entBCo->Y + y;
				}

				// Will Collide
				if (maxOffsetX > minOffsetX && maxOffsetY > minOffsetY)
				{
					if (!(maxOffsetX - (minXIsEnt ? 0.0 : (xStep < 0.0 ? xStep - PRECISION : xStep + PRECISION)) > minOffsetX - (minXIsEnt ? (xStep < 0.0 ? xStep - PRECISION : xStep + PRECISION) : 0.0)) && maxOffsetY > minOffsetY)
					{
						if (ent->getType() == PROJECTILE)
						{
							ent->SetHeight(PROJECTILE_SIZE * 5);
						}

						if (minXIsEnt)
						{
							possitions.push_back(Coordinates(worBCo->X, entACo->Y + y));
							collisionPosition.push_back(LX);
						}
						else
						{
							possitions.push_back(Coordinates(worACo->X - ent->GetWidth(), entACo->Y + y));
							collisionPosition.push_back(RX);
						}
					}
					else if (maxOffsetX > minOffsetX && !(maxOffsetY - (minYIsEnt ? 0.0 : (yStep < 0.0 ? yStep - PRECISION : yStep + PRECISION)) > minOffsetY - (minYIsEnt ? (yStep < 0.0 ? yStep - PRECISION : yStep + PRECISION) : 0.0)))
					{
						if (ent->getType() == PROJECTILE)
						{
							ent->SetWidth(PROJECTILE_SIZE * 5);
						}

						if (minYIsEnt)
						{
							possitions.push_back(Coordinates(entACo->X + x, worBCo->Y));
							collisionPosition.push_back(DY);
						}
						else
						{
							possitions.push_back(Coordinates(entACo->X + x, worACo->Y - ent->GetHeight()));
							collisionPosition.push_back(UY);
						}
					}

					collisionBlock.push_back(&(*wor));
					collisions.push_back(Coordinates(x, y));
					collisionType.push_back(WORLD);

					break;
				}
			}
		}
	}

	if (collisions.size())
	{
		int closestX = -1;
		int closestY = -1;

		for (int i = 0; i < collisions.size(); ++i)
		{
			if (collisionPosition[i] == LX || collisionPosition[i] == RX)
			{
				if (closestX == -1 ? true : (sqrt(pow(collisions[i].X, 2) + pow(collisions[i].Y, 2)) < sqrt(pow(collisions[closestX].X, 2) + pow(collisions[closestX].Y, 2))))
				{
					closestX = i;
				}
			}
			else if (collisionPosition[i] == UY || collisionPosition[i] == DY)
			{
				if (closestY == -1 ? true : (sqrt(pow(collisions[i].X, 2) + pow(collisions[i].Y, 2)) < sqrt(pow(collisions[closestY].X, 2) + pow(collisions[closestY].Y, 2))))
				{
					closestY = i;
				}
			}
		}

		if (closestX != -1 && closestY != -1)
		{
			if (entBCo->Y == collisionBlock[closestX]->GetA()->Y)
			{
				ent->SetCoordinates(possitions[closestY].X, possitions[closestY].Y);

				if (ent->getType() == PLAYER)
				{
					// Stop the entity if the movement gets below the FRICTION_STOP
					ent->SetVelocityVector(OffsetToVector(entOff->X < FRICTION_STOP && entOff->X > FRICTION_STOP * -1 ? 0.0 : entOff->X * FRICTION, 0.0));
				}
				else
				{
					ent->SetVelocityVector(0.0, 0.0);
					ent->SetHit(true);
				}
			}
			else if (entACo->Y == collisionBlock[closestX]->GetB()->Y)
			{
				ent->SetCoordinates(possitions[closestY].X, possitions[closestY].Y);

				if (ent->getType() == PLAYER)
				{
					ent->SetVelocityVector(OffsetToVector(entOff->X < FRICTION_STOP && entOff->X > FRICTION_STOP * -1 ? 0.0 : entOff->X * FRICTION, 0.0));
				}
				else
				{
					ent->SetVelocityVector(0.0, 0.0);
					ent->SetHit(true);
				}
			}
			else
			{
				ent->SetCoordinates(possitions[closestX].X, possitions[closestY].Y);

				ent->SetVelocityVector(0.0, 0.0);
				
				if (ent->getType() == PROJECTILE)
				{
					ent->SetHit(true);
				}
			}
		}
		else if (closestY != -1)
		{
			ent->SetCoordinates(possitions[closestY].X, possitions[closestY].Y);

			if (collisionType[closestY] == JELLY)
			{
				ent->SetVelocityVector(OffsetToVector(entOff->X, entOff->Y *-1));
			}
			else if (ent->getType() == PLAYER)
			{
				ent->SetVelocityVector(OffsetToVector(entOff->X < FRICTION_STOP && entOff->X > FRICTION_STOP * -1 ? 0.0 : entOff->X * FRICTION, 0.0));
			}
			else
			{
				ent->SetVelocityVector(0.0, 0.0);
				ent->SetHit(true);
			}
		}
		else if (closestX != -1)
		{
			ent->SetCoordinates(possitions[closestX].X, possitions[closestX].Y);

			if (collisionType[closestX] == JELLY)
			{
				ent->SetVelocityVector(OffsetToVector(entOff->X *-1, entOff->Y));
			}
			else if (ent->getType() == PLAYER)
			{
				ent->SetVelocityVector(OffsetToVector(0.0, entOff->Y < FRICTION_STOP && entOff->Y > FRICTION_STOP * -1 ? 0.0 : entOff->Y * FRICTION));
			}
			else
			{
				ent->SetVelocityVector(0.0, 0.0);
				ent->SetHit(true);
			}
		}

		// Add entities will collide
		if (WillCollide(ent, world))
		{
			Collide(ent, world, entities);
		}
	}

	delete entBCo;
	delete entOff;
}

bool Physics::WillCollide(Entity* entity, WorldBlock* block)
{
	float minY; 
	float maxY; 
	float minX; 
	float maxX; 

	Coordinates* entACo = entity->GetCoordinates();
	Coordinates* entBCo = new Coordinates(entACo->X + entity->GetWidth(), entACo->Y + entity->GetHeight());
	Coordinates* worACo = block->GetA();
	Coordinates* worBCo = block->GetB();
	Coordinates* entOff = VectorToOffset(entity->GetVelocityVector());

	// To make sure that a step isn't 1.0 if the offset is equal to teh velocity
	float xStep = entOff->Y == 0 ? entOff->X / (entOff->X * (entOff->X < 0.0 ? -10.0 : 10.0)) : entOff->X / ceil(entity->GetVelocityVector()->Velocity);
	float yStep = entOff->X == 0 ? entOff->Y / (entOff->Y * (entOff->Y < 0.0 ? -10.0 : 10.0)) : entOff->Y / ceil(entity->GetVelocityVector()->Velocity);

	for (float x = 0, y = 0; (xStep < 0.0 ? x >= entOff->X : x <= entOff->X) && (yStep < 0.0 ? y >= entOff->Y : y <= entOff->Y); x += xStep, y += yStep)
	{
		// X with Offset
		if (worACo->X < entACo->X + x)
		{
			minX = entACo->X + x;

			maxX = worBCo->X;
		}
		else
		{
			minX = worACo->X;

			maxX = entBCo->X + x;
		}

		//Y	with offset
		if (worACo->Y < entACo->Y + y)
		{
			minY = entACo->Y + y;

			maxY = worBCo->Y;
		}
		else
		{
			minY = worACo->Y;

			maxY = entBCo->Y + y;
		}
	
		if (maxX > minX && maxY > minY)
		{
			delete entBCo;
			delete entOff;
			return true;
		}
	}

	delete entBCo;
	delete entOff;
	return false;
}

bool Physics::WillCollide(Entity* entity, list<WorldBlock>* world)
{
	for (list<WorldBlock>::iterator wor = world->begin(); wor != world->end(); ++wor)
	{
		if (WillCollide(entity, &(*wor)))
		{
			return true;
		}
	}

	return false;
}

float Physics::OffsetToAngle(float x, float y)
{
	if (x == 0.0 && y > 0.0)
	{
		return FM_PI;
	}
	else if (x == 0.0 && y < 0.0)
	{
		return 0.0;
	}
	else if (x > 0.0 && y == 0.0)
	{
		return FM_3_PI_2;
	}
	else if (x < 0.0 && y == 0.0)
	{
		return FM_PI_2;
	}
	else if (x < 0.0 && y < 0.0)
	{
		return atan((x * -1.0) / (y * -1.0));
	}
	else if (x < 0.0 && y > 0.0)
	{
		return atan(y / (x * -1.0)) + FM_PI_2;
	}
	else if (x > 0.0 && y > 0.0)
	{
		return atan(x / y) + FM_PI;
	}
	else if (x > 0.0 && y < 0.0)
	{
		return atan((y * -1.0) / x) + FM_3_PI_2;
	}
	else
	{
		return 0.0;
	}
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
	else
	{
		vec->Angle = 0.0;
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
	else
	{
		vec->Angle = 0.0;
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
	else if (angle == FM_3_PI_2)
	{
		offset->X = velocity;
		offset->Y = 0.0;
	}
	else if (angle == FM_PI)
	{
		offset->X = 0.0;
		offset->Y = velocity * -1;
	}
	else if (angle == FM_PI_2)
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
	else if (angle < FM_PI_2 && angle > 0.0)
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
	else if (angle == FM_3_PI_2)
	{
		offset->X = velocity;
		offset->Y = 0.0;
	}
	else if (angle == FM_PI)
	{
		offset->X = 0.0;
		offset->Y = velocity;
	}
	else if (angle == FM_PI_2)
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
	else if (angle < FM_PI_2 && angle > 0.0)
	{
		offset->X = sin(angle) * velocity * -1;
		offset->Y = cos(angle) * velocity * -1;
	}

	return offset;
}