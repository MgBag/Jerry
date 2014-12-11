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

bool Physics::WillCollide(Entity* ent, vector<WorldBlock*> *world)
{
	Position* offset = ent->GetOffset();

	const Position* entA = ent->GetPosition();
	Position* entB = new Position(entA->X + ent->GetWidth(), entA->Y + ent->GetHeight());

	vector<int> xCol;
	vector<int> yCol;
	vector<bool> isYCol;

	for (vector<WorldBlock*>::iterator wBlock = world->begin(); wBlock != world->end(); ++wBlock)
	{
		const Position* worA = (*wBlock)->GetA();
		const Position* worB = (*wBlock)->GetB();

		xCol.push_back(-1);
		yCol.push_back(-1);
		isYCol.push_back(false);

		if (((round(entB->Y) <= worA->Y) && (entB->Y + offset->Y > worA->Y) && ((entA->X + offset->X < worB->X) || (entB->X + offset->X > worA->X))) ||
			((round(entA->Y) >= worB->Y) && (entA->Y + offset->Y < worB->Y) && ((entA->X + offset->X < worB->X) || (entB->X + offset->X > worA->X))) ||
			((round(entB->X) <= worA->X) && (entB->X + offset->X > worA->X) && ((entA->Y + offset->Y < worB->Y) || (entB->Y + offset->Y > worA->Y))) ||
			((round(entA->X) >= worB->X) && (entA->X + offset->X < worB->X) && ((entA->Y + offset->Y < worB->Y) || (entB->Y + offset->Y > worA->Y))))
		{
			if (offset->Y > 0)
			{
				if ()
				{
					if (round(entB->Y) >= worA->Y)
					{
						yCol[yCol.size() - 1] = entA->Y;

						if (round(entB->Y) == worA->Y)
						{
							isYCol[isYCol.size() - 1] = true;
						}
					}
					else
					{
						for (float y = fmod(offset->Y, 1.0); y <= offset->Y; ++y)
						{
							if (round(entB->Y + y) >= worA->Y)
							{
								yCol[yCol.size() - 1] = entA->Y + y;

								if (round(entB->Y + y) == worA->Y)
								{
									isYCol[isYCol.size() - 1] = true;
								}

								break;
							}
						}
					}
				}
			}
			else
			{

				if (worB->Y >= round(entA->Y))
				{
					yCol[yCol.size() - 1] = entA->Y;

					if (worB->Y == round(entA->Y))
					{
						isYCol[isYCol.size() - 1] = true;
					}
				}
				else
				{
					for (float y = fmod(offset->Y, 1.0); y >= offset->Y; --y)
					{
						if (worB->Y >= round(entA->Y + y))
						{
							yCol[yCol.size() - 1] = entA->Y + y;

							if (worB->Y == round(entA->Y + y))
							{
								isYCol[isYCol.size() - 1] = true;
							}

							break;
						}
					}
				}
			}

			if (offset->X > 0)
			{
				if (round(entB->Y) >= worA->X)
				{
					xCol[xCol.size() - 1] = entA->X;
				}
				else
				{
					for (float x = fmod(offset->X, 1.0); x <= offset->X; ++x)
					{
						if (round(entB->Y + x) >= worA->X)
						{
							xCol[xCol.size() - 1] = entA->X + x;

							break;
						}
					}
				}
			}
			else
			{
				if (worB->X >= round(entA->X))
				{
					xCol[xCol.size() - 1] = entA->X;
				}
				else
				{
					for (float x = fmod(offset->X, 1.0); x >= offset->X; --x)
					{
						if (worB->X >= round(entA->X + x))
						{
							xCol[xCol.size() - 1] = entA->X + x;

							break;
						}
					}
				}

			}
		}
	}

	delete offset;
	delete entB;

	for (int i = 0; i < xCol.size(); ++i)
	{
		if (xCol[i] != -1 && yCol[i] != -1)
		{
			ent->SetPosition(xCol[i], yCol[i]);

			if (isYCol[i])
				ent->SetVectorByOffset(ent->GetOffset()->X * FRICTION, 0.0);
			else
				ent->SetVectorByOffset(0.0, ent->GetOffset()->Y * FRICTION);

			return true;
		}
	}

	return false;
}

void Physics::ApplyGravity(Entity* ent)
{
	Position* offset = ent->GetOffset();

	ent->SetVectorByOffset(offset->X, offset->Y + GRAVITY);

	ent->MoveToOffset();

	delete offset;
}