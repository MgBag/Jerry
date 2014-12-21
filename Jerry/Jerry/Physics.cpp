#include "Physics.h"

void Physics::ApplyPhysics(vector<Entity>* entities, list<WorldBlock>* world)
{
	for (vector<Entity>::iterator ent = entities->begin(); ent != entities->end(); ++ent)
	{
		ApplyGravity(&(*ent));
		Collide(&(*ent), world);
		MoveEntity(&(*ent));
	}
}


void Physics::ApplyGravity(Entity* ent)
{
	Coordinates* offset = VectorToOffset(ent->GetVelocityVector());

	ent->SetVelocityVector(OffsetToVector(offset->X, offset->Y + GRAVITY));

	delete offset;
}

void Physics::MoveEntity(Entity* ent)
{
	Coordinates* offset = VectorToOffset(ent->GetVelocityVector());

	ent->MoveToOffset(offset->X, offset->Y);

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

void Physics::Collide(Entity* ent, list<WorldBlock>* world)
{
	// TODO: Only run for those in range
	Coordinates* entACo = ent->GetCoordinates();
	Coordinates* entBCo = new Coordinates(entACo->X + ent->GetWidth(), entACo->Y + ent->GetHeight());
	Coordinates* entOff = VectorToOffset(ent->GetVelocityVector()); // TODO: Add entity offset conlone function;
	float entVel = ent->GetVelocityVector()->Velocity;
	vector<Coordinates> collisions;
	vector<Coordinates> possitions;
	vector<bool> isXCollision;
	
	for (list<WorldBlock>::iterator wor = world->begin(); wor != world->end(); ++wor)
	{
		// TODO: Add only if in range statement
		Coordinates* worACo = wor->GetA();
		Coordinates* worBCo = wor->GetB();

		
		// TODO: Straigt line shit testing
		if (entVel)
		{
			float minOffsetX;
			float minOffsetY;
			float maxOffsetX;
			float maxOffsetY;
			bool minXIsEnt = false;
			bool minYIsEnt = false;
			//bool maxXIsEnt = false;
			//bool maxYIsEnt = false;

			// TODO: What to do if the step is 0?
			float xStep = entOff->X / entVel;
			float yStep = entOff->Y / entVel;

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
					wor->SetColor(al_map_rgb(20, 220, 20));

					if (!(maxOffsetX - (minXIsEnt ? 0.0 : (xStep < 0.0 ? xStep - PRECISION : xStep + PRECISION)) > minOffsetX - (minXIsEnt ? (xStep < 0.0 ? xStep - PRECISION : xStep + PRECISION) : 0.0)))
					{
						if (minXIsEnt)
						{
							possitions.push_back(Coordinates(worBCo->X, minYIsEnt ? minOffsetY : maxOffsetY));
						}
						else
						{
							possitions.push_back(Coordinates(worACo->X - ent->GetWidth(), minYIsEnt ? minOffsetY : maxOffsetY));
						}
						
						isXCollision.push_back(true);
					}
					else if (!(maxOffsetY - (minYIsEnt ? 0.0 : (yStep < 0.0 ? yStep - PRECISION : yStep + PRECISION)) > minOffsetY - (minYIsEnt ? (yStep < 0.0 ? yStep - PRECISION : yStep + PRECISION) : 0.0)))
					{
						if (minYIsEnt)
						{
							possitions.push_back(Coordinates(minXIsEnt ? minOffsetX : maxOffsetX, worBCo->Y));
						}
						else
						{
							possitions.push_back(Coordinates(minXIsEnt ? minOffsetX : maxOffsetX, worACo->Y - ent->GetHeight()));
						}

						isXCollision.push_back(false);
					}
					else
					{
						possitions.push_back(Coordinates(entACo->X, entACo->Y));
					}

					collisions.push_back(Coordinates(x, y));

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
			if (isXCollision[i])
			{
				if (closestX == -1 ? true : (sqrt(pow(collisions[i].X, 2) + pow(collisions[i].Y, 2)) < sqrt(pow(collisions[closestX].X, 2) + pow(collisions[closestX].Y, 2))))
				{
					closestX = i;
				}
			}
			else if (closestY == -1 ? true : (sqrt(pow(collisions[i].X, 2) + pow(collisions[i].Y, 2)) < sqrt(pow(collisions[closestY].X, 2) + pow(collisions[closestY].Y, 2))))
			{
				closestY = i;
			}
		}


		if (closestX != -1 && closestY != -1)
		{
			ent->SetCoordinates(possitions[closestX].X, possitions[closestY].Y);

			ent->SetVelocityVector(0.0, 0.0);
		}
		else if (closestY != -1)
		{
			ent->SetCoordinates(possitions[closestY].X, possitions[closestY].Y);

			ent->SetVelocityVector(0.0, 0.0);
		}
		else if (closestX != -1)
		{
			ent->SetCoordinates(possitions[closestX].X, possitions[closestX].Y);

			ent->SetVelocityVector(0.0, 0.0);
		}
	}
}

//void Physics::setMinMax(Coordinates* min, Coordinates* max, Coordinates* minOffset, Coordinates* maxOffset, Coordinates* entACo, Coordinates* entBCo, Coordinates* entOff, Coordinates* worACo, Coordinates* worBCo)
//{
//	setMinMax(min, max, minOffset, maxOffset, entACo, entBCo, entOff, worACo, worBCo);
//
//	// X
//	if (worACo->X < entACo->X)
//	{
//		min->X = entACo->X;
//		max->X = worBCo->X;
//	}
//	else
//	{
//		min->X = worACo->X;
//		max->X = entBCo->X;
//	}
//
//	//Y	
//	if (worACo->Y < entACo->Y)
//	{
//		min->Y = entACo->Y;
//		max->Y = worBCo->Y;
//	}
//	else
//	{
//		min->Y = worACo->Y;
//		max->Y = entBCo->Y;
//	}
//
//	// X with Offset
//	if (worACo->X < entACo->X + entOff->X)
//	{
//		min->X = entACo->X + entOff->X;
//		max->X = worBCo->X;
//	}
//	else
//	{
//		min->X = worACo->X;
//		max->X = entBCo->X + entOff->X;
//	}
//
//	//Y	with offset
//	if (worACo->Y < entACo->Y + entOff->Y)
//	{
//		min->Y = entACo->Y + entOff->Y;
//		max->Y = worBCo->Y;
//	}
//	else
//	{
//		min->Y = worACo->Y;
//		max->Y = entBCo->Y + entOff->Y;
//	}
//}


//
////TODO: Check if it's inside of the item
////TODO: This part is doing the evil. It looks like it is. 
//void Physics::Collide(Entity* ent, vector<WorldBlock*>* world)
//{
//	int worldItems = world->size();
//	Coordinates* offset = VectorToOffset(ent->GetVelocityVector());
//
//	const Coordinates* entA = ent->GetCoordinates();
//	Coordinates* entB = new Coordinates(entA->X + ent->GetWidth(), entA->Y + ent->GetHeight());
//
//	vector<float> xCol;
//	vector<float> yCol;
//
//	for (vector<WorldBlock*>::iterator wBlock = world->begin(); wBlock != world->end(); ++wBlock)
//	{
//		const Coordinates* worA = (*wBlock)->GetA();
//		const Coordinates* worB = (*wBlock)->GetB();
//
//		xCol.push_back(-1);
//		yCol.push_back(-1);
//
//		if (offset->Y > 0.0)
//		{
//			if ((entB->Y + offset->Y > worA->Y) && (entB->Y <= worA->Y) && (entB->X + offset->X > worA->X) & (entA->X + offset->X < worB->X))
//			{
//				if (ceil(entB->Y) == worA->Y)
//				{
//					yCol[yCol.size() - 1] = 0.0;
//					(*wBlock)->SetColor(al_map_rgb(20, 220, 20));
//				}
//				else
//				{
//					for (float y = fmod(offset->Y, 1.0); y <= offset->Y; ++y)
//					{
//						if (ceil(entB->Y + y) >= worA->Y)
//						{
//							if (ceil(entB->Y + y) == worA->Y)
//							{
//								(*wBlock)->SetColor(al_map_rgb(20, 220, 20));
//								yCol[yCol.size() - 1] = y;
//								break;
//							}
//						}
//					}
//				}
//			}
//		}
//		else
//		{
//			if ((entA->Y + offset->Y < worB->Y) && (entA->Y >= worB->Y) && (entB->X + offset->X > worA->X) && (entA->X + offset->X < worB->X))
//			{
//				if (ceil(entA->Y) == worB->Y)
//				{
//					(*wBlock)->SetColor(al_map_rgb(20, 220, 20));
//					yCol[yCol.size() - 1] = 0.0;
//				}
//				else
//				{
//					for (float y = fmod(offset->Y, 1.0); y >= offset->Y; --y)
//					{
//						if (ceil(entA->Y + y) <= worB->Y)
//						{
//							if (ceil(entA->Y + y) == worB->Y)
//							{
//								(*wBlock)->SetColor(al_map_rgb(20, 220, 20));
//								yCol[yCol.size() - 1] = y;
//								break;
//							}
//						}
//					}
//				}
//			}
//		}
//
//		if (offset->X >= 0.0)
//		{
//			if ((entB->X + offset->X > worA->X) && (entB->X <= worA->X) && (entB->Y + offset->Y > worA->Y) && (entA->Y + offset->Y < worB->Y))
//			{
//				if (ceil(entB->X) == worA->X)
//				{
//					(*wBlock)->SetColor(al_map_rgb(20, 220, 20));
//					xCol[xCol.size() - 1] = 0.0;
//				}
//				else
//				{
//					for (float x = fmod(offset->X, 1.0); x <= offset->X; ++x)
//					{
//						if (ceil(entB->X + x) >= worA->X)
//						{
//							if (ceil(entB->X + x) == worA->X)
//							{
//								(*wBlock)->SetColor(al_map_rgb(20, 220, 20));
//								xCol[xCol.size() - 1] = x;
//								break;
//							}
//						}
//					}
//				}
//			}
//		}
//		else
//		{
//			if ((entA->X + offset->X < worB->X) && (entA->X >= worB->X) && (entB->Y + offset->Y > worA->Y) && (entA->Y + offset->Y < worB->Y))
//			{
//				if (ceil(entA->X) == worB->X)
//				{
//					(*wBlock)->SetColor(al_map_rgb(20, 220, 20));
//					xCol[xCol.size() - 1] = 0.0;
//				}
//				else
//				{
//					for (float x = fmod(offset->X, 1.0); x >= offset->X; --x)
//					{
//						if (worB->X >= ceil(entA->X + x))
//						{
//							if (ceil(entA->X + x) == worB->X)
//							{
//								(*wBlock)->SetColor(al_map_rgb(20, 220, 20));
//								xCol[xCol.size() - 1] = x;
//								break;
//							}
//						}
//					}
//				}
//			}
//		}
//	}
//
//	int closestX = -1;
//	int closestY = -1;
//	float distanceX = ent->GetVelocityVector()->Velocity;
//	float distanceY = ent->GetVelocityVector()->Velocity;
//
//	for (int i = 0; i < worldItems; ++i)
//	{
//		if (yCol[i] != -1 && xCol[i] != -1)
//		{
//			////Hiero is dat problemo yo
//			//// Fixi?
//			//// Het is de volgorde waarin dit word getriggerd. 
//
//
//			//if (count(yCol.begin(), yCol.end(), 0.0) > 0)
//			//{
//			//	distanceY = sqrt(pow(yCol[i] / offset->Y * offset->X, 2) + pow(ceil(yCol[i]), 2));
//			//	closestY = i;
//			//	closestX = -1;
//
//			//}
//			//
//			//if (count(xCol.begin(), xCol.end(), 0.0) > 0)
//			//{
//			//	distanceX = sqrt(pow(ceil(xCol[i]), 2) + pow(xCol[i] / offset->X * offset->Y, 2));
//			//	closestX = i;
//			//	closestY = -1;
//
//			//}
//
//			//// Gekke collide is ook zonder trigger van dit
//			cout << "wut, bork?";
//		}
//		else if (yCol[i] != -1)
//		{
//			if (sqrt(pow(yCol[i] / offset->Y * offset->X, 2) + pow(ceil(yCol[i]), 2)) < distanceY)
//			{
//				distanceY = sqrt(pow(yCol[i] / offset->Y * offset->X, 2) + pow(ceil(yCol[i]), 2));
//				closestY = i;
//			}
//		}
//		else if (xCol[i] != -1)
//		{
//			if (sqrt(pow(ceil(xCol[i]), 2) + pow(xCol[i] / offset->X * offset->Y, 2)) < distanceX)
//			{
//				distanceX = sqrt(pow(ceil(xCol[i]), 2) + pow(xCol[i] / offset->X * offset->Y, 2));
//				closestX = i;
//			}
//		}
//	}
//
//	if (closestX != -1 && closestY != -1)
//	{
//		ent->SetCoordinates(ceil(xCol[closestX] + entA->X), ceil(yCol[closestY] + entA->Y));
//
//		ent->SetVelocityVector(0.0, 0.0);
//	}
//	else if (closestY != -1)
//	{
//		ent->SetCoordinates(yCol[closestY] / offset->Y * offset->X + entA->X, ceil(yCol[closestY] + entA->Y));
//
//		ent->SetVelocityVector(OffsetToVector(offset->X * FRICTION, 0.0));
//	}
//	else if (closestX != -1)
//	{
//		ent->SetCoordinates(ceil(xCol[closestX] + entA->X), xCol[closestX] / offset->X * offset->Y + entA->Y);
//
//		ent->SetVelocityVector(OffsetToVector(0.0, offset->Y * FRICTION));
//	}
//
//	delete offset;
//	delete entB;
//}

//Coordinates* min = new Coordinates();
//Coordinates* max = new Coordinates();

//// X
//if (worACo->X < entACo->X)
//{
//	min->X = entACo->X;
//	max->X = worBCo->X;
//}
//else
//{
//	min->X = worACo->X;
//	max->X = entBCo->X;
//}

////Y	
//if (worACo->Y < entACo->Y)
//{
//	min->Y = entACo->Y;
//	max->Y = worBCo->Y;
//}
//else
//{
//	min->Y = worACo->Y;
//	max->Y = entBCo->Y;
//}

//// Already colliding
//if (max->X > min->X && max->Y > min->Y)
//{
//	wor->SetColor(al_map_rgb(20, 220, 20));

//	collisions.push_back(Coordinates(0.0, 0.0));
//}
