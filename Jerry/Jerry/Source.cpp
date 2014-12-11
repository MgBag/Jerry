#define ALLEGRO_STATICLINK
#define _USE_MATH_DEFINES 

#include <iostream>
#include <vector>
#include <cmath>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include "Entity.h"
#include "WorldBlock.h"
#include "Physics.h"
#include "Constants.h"

using namespace std;

void draw(vector<Entity*> *ent, vector<WorldBlock*> *world);

enum UDLR
{
	UP, DOWN, LEFT, RIGHT
};

int main()
{
	ALLEGRO_DISPLAY* display = 0;
	ALLEGRO_EVENT_QUEUE* eventQueue = 0;
	ALLEGRO_TIMER* frame = 0;
	vector<WorldBlock*>* world = new vector<WorldBlock*>();
	vector<Entity*>* entities = new vector<Entity*>();
	Physics phys;

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

	world->push_back(new WorldBlock(0, 850, 1400, 900));
	//world->push_back(new WorldBlock(0, 0, 1400, 50));
	//world->push_back(new WorldBlock(0, 0, 50, 900));

	Entity* player = new Entity(700, 650, 20, 20, al_map_rgb(220, 20, 20), 10, M_PI_2);

	entities->push_back(player);

	al_start_timer(frame);


	while (!quit)
	{
		ALLEGRO_EVENT e;

		al_wait_for_event(eventQueue, &e);

		if (e.type == ALLEGRO_EVENT_TIMER)
		{
			phys.ApplyPhysics(entities, world);

			draw(entities, world);
		}
		else if (e.type == ALLEGRO_EVENT_KEY_DOWN)
		{

			player->SetVectorByOffset(player->GetOffset()->X, player->GetOffset()->Y - 10);
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
		al_draw_filled_rectangle((*wBlock)->GetA()->X, (*wBlock)->GetA()->Y, (*wBlock)->GetB()->X, (*wBlock)->GetB()->Y, al_map_rgb(20, 20, 20));
	}

	for (vector<Entity*>::iterator ent = entities->begin(); ent != entities->end(); ++ent)
	{
		Position* pos = (*ent)->GetPosition();

		al_draw_filled_rectangle(pos->X, pos->Y, pos->X + (*ent)->GetWidth(), pos->Y + (*ent)->GetHeight(), (*ent)->GetColor());
	}

	al_flip_display();
}


