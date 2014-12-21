//TODO: CHECK MEMORY LEAKS

#define ALLEGRO_STATICLINK
#define _USE_MATH_DEFINES 

#include <iostream>
#include <list>
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

void draw(vector<Entity>* ent, list<WorldBlock>* world);
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
	list<WorldBlock>* world = new list<WorldBlock>();
	vector<Entity>* entities = new vector<Entity>();
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

	//al_set_new_display_adapter(1);
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



	entities->push_back(Entity(200, 200, 20, 20, al_map_rgb(220, 20, 20), 0.0, 0.0));
	Entity* player = &(*entities)[0];

	//center bar
	world->push_back(WorldBlock(300, 300, 900, 310, al_map_rgb(20, 20, 20)));

	//// world outlining
	//world->push_back(WorldBlock(0, 590, 1280, 600, al_map_rgb(20, 20, 20)));
	//world->push_back(WorldBlock(0, 0, 10, 600, al_map_rgb(20, 20, 20)));
	//world->push_back(WorldBlock(0, 0, 1280, 10, al_map_rgb(20, 20, 20)));
	//world->push_back(WorldBlock(1270, 0, 1280, 600, al_map_rgb(20, 20, 20)));

	////center bar stripes
	//world->push_back(WorldBlock(650, 280, 655, 330, al_map_rgb(20, 20, 20)));
	//world->push_back(WorldBlock(660, 280, 665, 330, al_map_rgb(20, 20, 20)));
	//world->push_back(WorldBlock(670, 280, 675, 330, al_map_rgb(20, 20, 20)));
	//world->push_back(WorldBlock(680, 280, 685, 330, al_map_rgb(20, 20, 20)));
	//world->push_back(WorldBlock(690, 280, 695, 330, al_map_rgb(20, 20, 20)));
	//world->push_back(WorldBlock(700, 280, 705, 330, al_map_rgb(20, 20, 20)));
	//world->push_back(WorldBlock(710, 280, 715, 330, al_map_rgb(20, 20, 20)));
	//world->push_back(WorldBlock(720, 280, 725, 330, al_map_rgb(20, 20, 20)));
	//world->push_back(WorldBlock(730, 280, 735, 330, al_map_rgb(20, 20, 20)));
	//world->push_back(WorldBlock(740, 280, 745, 330, al_map_rgb(20, 20, 20)));
	//world->push_back(WorldBlock(750, 280, 755, 330, al_map_rgb(20, 20, 20)));
	//world->push_back(WorldBlock(760, 280, 765, 330, al_map_rgb(20, 20, 20)));
	//world->push_back(WorldBlock(770, 280, 775, 330, al_map_rgb(20, 20, 20)));
	//world->push_back(WorldBlock(780, 280, 785, 330, al_map_rgb(20, 20, 20)));
	//world->push_back(WorldBlock(790, 280, 795, 330, al_map_rgb(20, 20, 20)));
	//world->push_back(WorldBlock(800, 280, 805, 330, al_map_rgb(20, 20, 20)));

	////top left stripes
	////x
	//world->push_back(WorldBlock(70, 10, 75, 30, al_map_rgb(20, 20, 20)));
	//world->push_back(WorldBlock(80, 10, 85, 30, al_map_rgb(20, 20, 20)));
	//world->push_back(WorldBlock(90, 10, 95, 30, al_map_rgb(20, 20, 20)));
	//world->push_back(WorldBlock(100, 10, 105, 30, al_map_rgb(20, 20, 20)));

	////y
	//world->push_back(WorldBlock(10, 70, 30, 75, al_map_rgb(20, 20, 20)));
	//world->push_back(WorldBlock(10, 80, 30, 85, al_map_rgb(20, 20, 20)));
	//world->push_back(WorldBlock(10, 90, 30, 95, al_map_rgb(20, 20, 20)));
	//world->push_back(WorldBlock(10, 100, 30, 105, al_map_rgb(20, 20, 20)));

	al_start_timer(frame);


	while (!quit)
	{
		ALLEGRO_EVENT e;

		al_wait_for_event(eventQueue, &e);

		if (e.type == ALLEGRO_EVENT_TIMER)
		{
			move(player, keys);

			phys.ApplyPhysics(entities, world);

			draw(entities, world);
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
				player->SetCoordinates(200, 200);
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

void draw(vector<Entity> *entities, list<WorldBlock> *world)
{
	al_clear_to_color(al_map_rgb(220, 220, 220));

	for (list<WorldBlock>::iterator wBlock = world->begin(); wBlock != world->end(); ++wBlock)
	{
		al_draw_filled_rectangle(wBlock->GetA()->X, wBlock->GetA()->Y, wBlock->GetB()->X, wBlock->GetB()->Y, wBlock->GetColor());

		wBlock->SetColor(al_map_rgb(20, 20, 20));
	}

	for (vector<Entity>::iterator ent = entities->begin(); ent != entities->end(); ++ent)
	{
		Coordinates* pos = ent->GetCoordinates();
		Coordinates* offset = phys.VectorToOffset(ent->GetVelocityVector());

		al_draw_filled_rectangle(pos->X, pos->Y, pos->X + ent->GetWidth(), pos->Y + ent->GetHeight(), ent->GetColor());
		al_draw_line(
			pos->X + ent->GetWidth() / 2,
			pos->Y + ent->GetHeight() / 2,
			(pos->X + ent->GetWidth() / 2) + offset->X,
			(pos->Y + ent->GetHeight() / 2) + offset->Y,
			al_map_rgb(220, 20, 20),
			1.0);
	}

	al_flip_display();
}

void move(Entity* ent, bool keys[4])
{
	if (keys[RIGHT] || keys[UP] || keys[LEFT] || keys[DOWN])
	{
		Coordinates* offset = phys.VectorToOffset(ent->GetVelocityVector());

		if (keys[RIGHT])
		{
			offset->X += PLAYER_SPEED;
		}
		if (keys[UP])
		{
			offset->Y -= PLAYER_SPEED;
		}
		if (keys[LEFT])
		{
			offset->X -= PLAYER_SPEED;
		}
		if (keys[DOWN])
		{
			offset->Y += PLAYER_SPEED;
		}

		ent->SetVelocityVector(phys.OffsetToVector(offset));

		delete offset;
	}
}