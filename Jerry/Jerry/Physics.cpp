#include "Physics.h"

void Physics::ApplyPhysics(vector<Entity*> *entities, vector<WorldBlock*> *world)
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
bool Physics::WillCollide(Entity* ent, vector<WorldBlock*> *world)
{
	Position* offset = ent->GetOffset();

	const Position* entA = ent->GetPosition();
	Position* entB = new Position(entA->X + ent->GetWidth(), entA->Y + ent->GetHeight());

	vector<float> xCol;
	vector<float> yCol;
	vector<bool> isXCol;
	vector<bool> isYCol;

	for (vector<WorldBlock*>::iterator wBlock = world->begin(); wBlock != world->end(); ++wBlock)
	{
		const Position* worA = (*wBlock)->GetA();
		const Position* worB = (*wBlock)->GetB();

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
					yCol[yCol.size() - 1] = entA->Y;

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
							yCol[yCol.size() - 1] = entA->Y + y;

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
					yCol[yCol.size() - 1] = entA->Y;

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
							yCol[yCol.size() - 1] = entA->Y + y;

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
						xCol[xCol.size() - 1] = entA->X;

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
								xCol[xCol.size() - 1] = entA->X + x;
								
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
						xCol[xCol.size() - 1] = entA->X;

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
								xCol[xCol.size() - 1] = entA->X + x;

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

	for (int i = 0; i < xCol.size(); ++i) //move this to the inside 
	{
		if ((yCol[i] != -1 && isYCol[i]) || (xCol[i] != -1 && isXCol[i]))
		{
			if (isYCol[i])
			{
				ent->SetPosition(entA->X + ((yCol[i] - entA->Y) / offset->Y * offset->X), ceil(yCol[i]));

				ent->SetVectorByOffset(offset->X * FRICTION, GRAVITY * 2 *-1);
			}
			else if (isXCol[i])
			{
				ent->SetPosition(ceil(xCol[i]), entA->Y + ((xCol[i] - entA->X) / offset->X * offset->Y));

				ent->SetVectorByOffset(0.0, offset->Y * FRICTION);
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
	Position* offset = ent->GetOffset();

	ent->MoveToOffset();

	ent->SetVectorByOffset(offset->X, offset->Y + GRAVITY);

	delete offset;
}