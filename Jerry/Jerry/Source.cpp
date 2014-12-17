//TODO: CHECK MEMORY LEAKS

#define ALLEGRO_STATICLINK
#define _USE_MATH_DEFINES 

#include <iostream>
#include <vector>
#include <cmath>
#include <string>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include "Entity.h"
#include "WorldBlock.h"
#include "Physics.h"
#include "Constants.h"

using namespace std;

void draw(vector<Entity*> *ent, vector<WorldBlock*> *world);
void move(Entity* player, bool keys[4]);

Physics phys;

enum UDLR
{
	RIGHT, UP, LEFT, DOWN
};

int main()
{
	ALLEGRO_DISPLAY* display = 0;
	ALLEGRO_EVENT_QUEUE* eventQueue = 0;
	ALLEGRO_TIMER* frame = 0;
	vector<WorldBlock*>* world = new vector<WorldBlock*>();
	vector<Entity*>* entities = new vector<Entity*>();
	bool keys[4] = { false, false, false, false };

	bool quit = false;

	if (!al_init())
	{
		cout << "Allegro failed to initiate\n";
		return -1;
	}

	if (!al_install_keyboard())
	{
		cout << "Allegro install keyboard\n";
		return -1;
	}

	if (!al_init_primitives_addon())
	{
		cout << "Failed to initiate primitives addon\n";
		return -1;
	}

	eventQueue = al_create_event_queue();
	if (!eventQueue)
	{
		cout << "Failed to initiate event queue\n";
		return -1;
	}

	al_set_new_display_adapter(1);
	display = al_create_display(SCREEN_W, SCREEN_H);
	if (!display)
	{
		cout << "Failed to initiate display \n";
		al_destroy_event_queue(eventQueue);
		return -1;
	}

	frame = al_create_timer(1.0 / FPS);
	if (!frame)
	{
		cout << "Failed to initiate display \n";
		al_destroy_display(display);
		al_destroy_event_queue(eventQueue);
		return -1;
	}

	al_register_event_source(eventQueue, al_get_timer_event_source(frame));
	al_register_event_source(eventQueue, al_get_display_event_source(display));
	al_register_event_source(eventQueue, al_get_keyboard_event_source());

	Entity* player = new Entity(700, 650, 6, 6, al_map_rgb(220, 20, 20), 0.0, 0.0);

	entities->push_back(player);

	// world outlining
	world->push_back(new WorldBlock(0, 850, 1400, 900, al_map_rgb(20, 20, 20)));
	world->push_back(new WorldBlock(0, 0, 50, 900, al_map_rgb(20, 20, 20)));
	world->push_back(new WorldBlock(0, 0, 1400, 50, al_map_rgb(20, 20, 20)));
	world->push_back(new WorldBlock(1350, 0, 1400, 900, al_map_rgb(20, 20, 20)));
	
	//center bar
	world->push_back(new WorldBlock(400, 400, 1000, 450, al_map_rgb(20, 20, 20)));

	//center bar stripes
	world->push_back(new WorldBlock(650, 350, 655, 500, al_map_rgb(20, 20, 20)));
	world->push_back(new WorldBlock(660, 350, 665, 500, al_map_rgb(20, 20, 20)));
	world->push_back(new WorldBlock(670, 350, 675, 500, al_map_rgb(20, 20, 20)));
	world->push_back(new WorldBlock(680, 350, 685, 500, al_map_rgb(20, 20, 20)));
	world->push_back(new WorldBlock(690, 350, 695, 500, al_map_rgb(20, 20, 20)));
	world->push_back(new WorldBlock(700, 350, 705, 500, al_map_rgb(20, 20, 20)));
	world->push_back(new WorldBlock(710, 350, 715, 500, al_map_rgb(20, 20, 20)));
	world->push_back(new WorldBlock(720, 350, 725, 500, al_map_rgb(20, 20, 20)));
	world->push_back(new WorldBlock(730, 350, 735, 500, al_map_rgb(20, 20, 20)));
	world->push_back(new WorldBlock(740, 350, 745, 500, al_map_rgb(20, 20, 20)));
	world->push_back(new WorldBlock(750, 350, 755, 500, al_map_rgb(20, 20, 20)));
	world->push_back(new WorldBlock(760, 350, 765, 500, al_map_rgb(20, 20, 20)));
	world->push_back(new WorldBlock(770, 350, 775, 500, al_map_rgb(20, 20, 20)));
	world->push_back(new WorldBlock(780, 350, 785, 500, al_map_rgb(20, 20, 20)));
	world->push_back(new WorldBlock(790, 350, 795, 500, al_map_rgb(20, 20, 20)));
	world->push_back(new WorldBlock(800, 350, 805, 500, al_map_rgb(20, 20, 20)));

	//top left stripes
	//x
	world->push_back(new WorldBlock(70, 50, 75, 70, al_map_rgb(20, 20, 20)));
	world->push_back(new WorldBlock(80, 50, 85, 70, al_map_rgb(20, 20, 20)));
	world->push_back(new WorldBlock(90, 50, 95, 70, al_map_rgb(20, 20, 20)));
	world->push_back(new WorldBlock(100, 50, 105, 70, al_map_rgb(20, 20, 20)));

	//y
	world->push_back(new WorldBlock(50, 70, 70, 75, al_map_rgb(20, 20, 20)));
	world->push_back(new WorldBlock(50, 80, 70, 85, al_map_rgb(20, 20, 20)));
	world->push_back(new WorldBlock(50, 90, 70, 95, al_map_rgb(20, 20, 20)));
	world->push_back(new WorldBlock(50, 100, 70, 105, al_map_rgb(20, 20, 20)));

	al_start_timer(frame);


	while (!quit)
	{
		ALLEGRO_EVENT e;

		al_wait_for_event(eventQueue, &e);

		if (e.type == ALLEGRO_EVENT_TIMER)
		{
			phys.ApplyPhysics(entities, world);

			draw(entities, world);

			move(player, keys);
		}
		else if (e.type == ALLEGRO_EVENT_KEY_DOWN)
		{
			switch (e.keyboard.keycode)
			{
			case ALLEGRO_KEY_D:
			case ALLEGRO_KEY_RIGHT:
				keys[RIGHT] = true;
				break;

			case ALLEGRO_KEY_A:
			case ALLEGRO_KEY_LEFT:
				keys[LEFT] = true;
				break;

			case ALLEGRO_KEY_W:
			case ALLEGRO_KEY_UP:
				keys[UP] = true;
				break;

			case ALLEGRO_KEY_S:
			case ALLEGRO_KEY_DOWN:
				keys[DOWN] = true;
				break;

			case ALLEGRO_KEY_R:
				player->SetCoordinates(60, 80);
				break;
			}
		}
		else if (e.type == ALLEGRO_EVENT_KEY_UP)
		{
			switch (e.keyboard.keycode)
			{
			case ALLEGRO_KEY_D:
			case ALLEGRO_KEY_RIGHT:
				keys[RIGHT] = false;
				break;

			case ALLEGRO_KEY_SPACE:
			case ALLEGRO_KEY_W:
			case ALLEGRO_KEY_UP:
				keys[UP] = false;
				break;

			case ALLEGRO_KEY_A:
			case ALLEGRO_KEY_LEFT:
				keys[LEFT] = false;
				break;

			case ALLEGRO_KEY_S:
			case ALLEGRO_KEY_DOWN:
				keys[DOWN] = false;
				break;
			}
		}
		else if (e.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
		{
			quit = true;
		}
	}

	return 0;
}

void draw(vector<Entity*> *entities, vector<WorldBlock*> *world)
{
	al_clear_to_color(al_map_rgb(220, 220, 220));

	for (vector<WorldBlock*>::iterator wBlock = world->begin(); wBlock != world->end(); ++wBlock)
	{
		al_draw_filled_rectangle((*wBlock)->GetA()->X, (*wBlock)->GetA()->Y, (*wBlock)->GetB()->X, (*wBlock)->GetB()->Y, (*wBlock)->GetColor());

		(*wBlock)->SetColor(al_map_rgb(20, 20, 20));
	}

	for (vector<Entity*>::iterator ent = entities->begin(); ent != entities->end(); ++ent)
	{
		Coordinates* pos = (*ent)->GetCoordinates();

		al_draw_filled_rectangle(pos->X, pos->Y, pos->X + (*ent)->GetWidth(), pos->Y + (*ent)->GetHeight(), (*ent)->GetColor());
	}

	al_flip_display();
}

void move(Entity* ent, bool keys[4])
{
	Coordinates* offset = phys.VectorToOffset(ent->GetVelocityVector());

	if (keys[RIGHT])
	{
		offset->X += 0.05;
	}
	if (keys[UP])
	{
		offset->Y -= 0.05;
	}
	if (keys[LEFT])
	{
		offset->X -= 0.05;
	}
	if (keys[DOWN])
	{
		offset->Y += 0.05;
	}

	ent->SetVelocityVector(phys.OffsetToVector(offset));

	delete offset;
}

