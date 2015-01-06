// TODO : Fix roation twitch
// TODO : Remove the vucking fector

#define ALLEGRO_STATICLINK
#define _USE_MATH_DEFINES 

#include <process.h>
#include <iostream>
#include <list>
#include <vector>
#include <cmath>
#include <string>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_font.h>
#include "Entity.h"
#include "WorldBlock.h"
#include "Physics.h"
#include "Constants.h"

using namespace std;

void draw(list<Entity>* ent, list<WorldBlock>* world);
void move(Entity* player, bool keys[4]);
void shoot(list<Entity>* entities, ALLEGRO_EVENT e, int burstID);
void AsyncPhysics(void* struc);

Physics phys;
ALLEGRO_FONT *font = 0;
bool quit = false;

struct PhysicsVariables
{
	list<Entity>* entities;
	list<WorldBlock>* world;
};

int main()
{
	ALLEGRO_DISPLAY* display = 0;
	ALLEGRO_EVENT_QUEUE* eventQueue = 0;
	ALLEGRO_TIMER* frame = 0;
	list<WorldBlock>* world = new list<WorldBlock>();
	list<Entity>* entities = new list<Entity>();
	bool keys[4] = { false, false, false, false };

	if (!al_init())
	{
		cout << "Allegro failed to initiate\n";
		system("pause");
		return -1;
	}

	if (!al_install_keyboard())
	{
		cout << "Allegro install keyboard\n";
		system("pause");
		return -1;
	}

	if (!al_install_mouse())
	{
		cout << "Allegro install mouse\n";
		system("pause");
		return -1;
	}

	if (!al_init_primitives_addon())
	{
		cout << "Failed to initiate primitives addon\n";
		system("pause");
		return -1;
	}

	eventQueue = al_create_event_queue();
	if (!eventQueue)
	{
		cout << "Failed to initiate event queue\n";
		system("pause");
		return -1;
	}


	//al_set_new_display_adapter(1);
	display = al_create_display(SCREEN_W, SCREEN_H);
	if (!display)
	{
		cout << "Failed to initiate display \n";
		al_destroy_event_queue(eventQueue);
		system("pause");
		return -1;
	}

	frame = al_create_timer(1.0 / FPS);
	if (!frame)
	{
		cout << "Failed to initiate frame \n";
		al_destroy_display(display);
		al_destroy_event_queue(eventQueue);
		system("pause");
		return -1;
	}


	// TODO: Do these
	al_init_font_addon();
	al_init_ttf_addon();

	font = al_load_ttf_font("arial.ttf", 16, 0);

	if (!font)
	{
		fprintf(stderr, "Failed to load font\n");
	}

	entities->push_back(Entity(20, 400, 20, 20, 0.0, 0.0, al_map_rgb(220, 20, 20), PLAYER, 0));
	Entity* player = &(*entities->begin());

	//center bar
	world->push_back(WorldBlock(300, 300, 900, 310, al_map_rgb(20, 20, 20)));

	// world outlining
	world->push_back(WorldBlock(0, 590, 1280, 600, al_map_rgb(20, 20, 20)));
	world->push_back(WorldBlock(0, 0, 10, 600, al_map_rgb(20, 20, 20)));
	world->push_back(WorldBlock(0, 0, 1280, 10, al_map_rgb(20, 20, 20)));
	world->push_back(WorldBlock(1270, 0, 1280, 600, al_map_rgb(20, 20, 20)));

	//center bar stripes
	world->push_back(WorldBlock(650, 280, 655, 330, al_map_rgb(20, 20, 20)));
	world->push_back(WorldBlock(660, 280, 665, 330, al_map_rgb(20, 20, 20)));
	world->push_back(WorldBlock(670, 280, 675, 330, al_map_rgb(20, 20, 20)));
	world->push_back(WorldBlock(680, 280, 685, 330, al_map_rgb(20, 20, 20)));
	world->push_back(WorldBlock(690, 280, 695, 330, al_map_rgb(20, 20, 20)));
	world->push_back(WorldBlock(700, 280, 705, 330, al_map_rgb(20, 20, 20)));
	world->push_back(WorldBlock(710, 280, 715, 330, al_map_rgb(20, 20, 20)));
	world->push_back(WorldBlock(720, 280, 725, 330, al_map_rgb(20, 20, 20)));
	world->push_back(WorldBlock(730, 280, 735, 330, al_map_rgb(20, 20, 20)));
	world->push_back(WorldBlock(740, 280, 745, 330, al_map_rgb(20, 20, 20)));
	world->push_back(WorldBlock(750, 280, 755, 330, al_map_rgb(20, 20, 20)));
	world->push_back(WorldBlock(760, 280, 765, 330, al_map_rgb(20, 20, 20)));
	world->push_back(WorldBlock(770, 280, 775, 330, al_map_rgb(20, 20, 20)));
	world->push_back(WorldBlock(780, 280, 785, 330, al_map_rgb(20, 20, 20)));
	world->push_back(WorldBlock(790, 280, 795, 330, al_map_rgb(20, 20, 20)));
	world->push_back(WorldBlock(800, 280, 805, 330, al_map_rgb(20, 20, 20)));

	world->push_back(WorldBlock(340, 280, 345, 330, al_map_rgb(20, 20, 20)));
	world->push_back(WorldBlock(365, 280, 370, 330, al_map_rgb(20, 20, 20)));


	//top left stripes
	//x
	world->push_back(WorldBlock(30, 10, 35, 30, al_map_rgb(20, 20, 20)));
	world->push_back(WorldBlock(40, 10, 45, 30, al_map_rgb(20, 20, 20)));
	world->push_back(WorldBlock(50, 10, 55, 30, al_map_rgb(20, 20, 20)));
	world->push_back(WorldBlock(60, 10, 65, 30, al_map_rgb(20, 20, 20)));
	world->push_back(WorldBlock(70, 10, 75, 30, al_map_rgb(20, 20, 20)));
	world->push_back(WorldBlock(80, 10, 85, 30, al_map_rgb(20, 20, 20)));
	world->push_back(WorldBlock(90, 10, 95, 30, al_map_rgb(20, 20, 20)));
	world->push_back(WorldBlock(100, 10, 105, 30, al_map_rgb(20, 20, 20)));

	//y
	world->push_back(WorldBlock(10, 30, 30, 35, al_map_rgb(20, 20, 20)));
	world->push_back(WorldBlock(10, 40, 30, 45, al_map_rgb(20, 20, 20)));
	world->push_back(WorldBlock(10, 50, 30, 55, al_map_rgb(20, 20, 20)));
	world->push_back(WorldBlock(10, 60, 30, 65, al_map_rgb(20, 20, 20)));
	world->push_back(WorldBlock(10, 70, 30, 75, al_map_rgb(20, 20, 20)));
	world->push_back(WorldBlock(10, 80, 30, 85, al_map_rgb(20, 20, 20)));
	world->push_back(WorldBlock(10, 90, 30, 95, al_map_rgb(20, 20, 20)));
	world->push_back(WorldBlock(10, 100, 30, 105, al_map_rgb(20, 20, 20)));

	// User input and drawing
	al_register_event_source(eventQueue, al_get_timer_event_source(frame));
	al_register_event_source(eventQueue, al_get_display_event_source(display));
	al_register_event_source(eventQueue, al_get_keyboard_event_source());
	al_register_event_source(eventQueue, al_get_mouse_event_source());


	//void AsyncPhysics(void* void_tickQueue, void* void_entities, void* void_world)
	PhysicsVariables physVar;
	physVar.entities = entities;
	physVar.world = world;

	_beginthread(AsyncPhysics, 0, (void*)&physVar);

	// TODO: Meak pretti fix for this
	al_grab_mouse(display);

	al_start_timer(frame);

	int tempShoot = 0;
	ALLEGRO_EVENT mouse;
	// TODO: Make dynamic
	int burstID = 1;

	while (!quit)
	{
		ALLEGRO_EVENT e;

		al_wait_for_event(eventQueue, &e);

		if (e.type == ALLEGRO_EVENT_TIMER)
		{
			if (tempShoot)
			{
				shoot(entities, mouse, burstID);
				--tempShoot;
			}

			move(player, keys);

			//phys.ApplyPhysics(entities, world);

			draw(entities, world);
		}
		else if (e.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN)
		{
			if (e.mouse.button == 1 && !tempShoot)
			{
				++burstID;
				tempShoot = 3;
				mouse = e;
			}
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

			case ALLEGRO_KEY_SPACE:
			case ALLEGRO_KEY_W:
			case ALLEGRO_KEY_UP:
				keys[UP] = true;
				break;

			case ALLEGRO_KEY_S:
			case ALLEGRO_KEY_DOWN:
				keys[DOWN] = true;
				break;

			case ALLEGRO_KEY_R:
				player->SetCoordinates(105, 10);
				break;

			case ALLEGRO_KEY_ESCAPE:
				quit = true;
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

	al_rest(0.1);

	delete world, entities;

	return 0;
}

void draw(list<Entity> *entities, list<WorldBlock> *world)
{
	al_clear_to_color(al_map_rgb(220, 220, 220));

	for (list<WorldBlock>::iterator wBlock = world->begin(); wBlock != world->end(); ++wBlock)
	{
		al_draw_filled_rectangle(wBlock->GetA()->X, wBlock->GetA()->Y, wBlock->GetB()->X, wBlock->GetB()->Y, wBlock->GetColor());
	}


	// TODO: Particle age in color
	for (list<Entity>::iterator ent = entities->begin(); ent != entities->end(); ++ent)
	{
		Coordinates* pos = ent->GetACoordinates();
		Coordinates* offset = ent->GetOffset();

		al_draw_filled_rectangle(pos->X, pos->Y, pos->X + ent->GetWidth(), pos->Y + ent->GetHeight(), ent->GetColor());
		al_draw_line(
			pos->X + ent->GetWidth() / 2,
			pos->Y + ent->GetHeight() / 2,
			(pos->X + ent->GetWidth() / 2) + offset->X,
			(pos->Y + ent->GetHeight() / 2) + offset->Y,
			al_map_rgb(220, 20, 20),
			1.0);
	}

	ALLEGRO_MOUSE_STATE mouse;
	al_get_mouse_state(&mouse);

	al_draw_pixel(mouse.x, mouse.y, al_map_rgb(20, 20, 220));

	Entity* player = &*entities->begin();
	Coordinates* entPos = player->GetACoordinates();
	float originX = entPos->X + player->GetWidth() / 2;
	float originY = entPos->Y + player->GetHeight() / 2;
	Coordinates* gunVec = phys.VectorToOffset(15.0, phys.OffsetToAngle((originX - (float)mouse.x) * -1, (originY - (float)mouse.y) * -1));
	Coordinates* blankGunVec = phys.VectorToOffset(5.0, phys.OffsetToAngle((originX - (float)mouse.x) * -1, (originY - (float)mouse.y) * -1));

	al_draw_line(originX, originY, gunVec->X + originX, gunVec->Y + originY, al_map_rgb(20, 20, 220), 5.0);
	al_draw_line(originX, originY, blankGunVec->X + originX, blankGunVec->Y + originY, al_map_rgb(220, 20, 20), 5.0);

	al_draw_text(font, al_map_rgb(255, 10, 10), 5, 5, 0, std::to_string(12).c_str());

	al_flip_display();
}

void move(Entity* ent, bool keys[4])
{
	if (keys[RIGHT] || keys[UP] || keys[LEFT] || keys[DOWN])
	{
		Coordinates* offset = ent->GetOffset();

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
	}
}

// TODO: Check nececerity of convertions
void shoot(list<Entity>* entities, ALLEGRO_EVENT e, int burstID)
{
	if (entities->size() > PARTICLE_MAX)
	{
		//entities->erase(++entities->begin());
		//entities->erase(++entities->begin());
		//entities->erase(++entities->begin());
		//entities->erase(++entities->begin());
		//entities->erase(++entities->begin());
		//entities->erase(++entities->begin());
		//entities->erase(++entities->begin());
		//entities->erase(++entities->begin());
		//entities->erase(++entities->begin());
		//entities->erase(++entities->begin());
		//entities->erase(++entities->begin());
		//entities->erase(++entities->begin());
		//entities->erase(++entities->begin());
		//entities->erase(++entities->begin());
		//entities->erase(++entities->begin());
		//entities->erase(++entities->begin());
	}

	for (list<Entity>::iterator ent = ++entities->begin(); ent != entities->end(); ++ent)
	{
		unsigned char r, g, b;
		al_unmap_rgb(ent->GetColor(), &r, &g, &b);

		g -= 220.0 / PARTICLE_MAX;

		ent->SetColor(al_map_rgb(r, g, b));
	}

	Entity* player = &*entities->begin();
	Coordinates* entPos = player->GetACoordinates();
	Coordinates* entOff = player->GetOffset();
	float originX = entPos->X + player->GetWidth() / 2 - PROJECTILE_SIZE / 2;
	float originY = entPos->Y + player->GetHeight() / 2 - PROJECTILE_SIZE / 2;
	//Coordinates* shotOff = phys.VectorToOffset(PROJECTILE_SPEED, phys.OffsetToAngle((originX - e.mouse.x + PROJECTILE_SIZE / 2) * -1, (originY - e.mouse.y + PROJECTILE_SIZE / 2) * -1));
	Coordinates* shotOff1 = phys.VectorToOffset(PROJECTILE_SPEED, FM_PI_2 / 4 * 1);
	Coordinates* shotOff2 = phys.VectorToOffset(PROJECTILE_SPEED, FM_PI_2 / 4 * 2);
	Coordinates* shotOff3 = phys.VectorToOffset(PROJECTILE_SPEED, FM_PI_2 / 4 * 3);
	Coordinates* shotOff4 = phys.VectorToOffset(PROJECTILE_SPEED, FM_PI_2 / 4 * 4);
	Coordinates* shotOff5 = phys.VectorToOffset(PROJECTILE_SPEED, FM_PI_2 / 4 * 5);
	Coordinates* shotOff6 = phys.VectorToOffset(PROJECTILE_SPEED, FM_PI_2 / 4 * 6);
	Coordinates* shotOff7 = phys.VectorToOffset(PROJECTILE_SPEED, FM_PI_2 / 4 * 7);
	Coordinates* shotOff8 = phys.VectorToOffset(PROJECTILE_SPEED, FM_PI_2 / 4 * 8);
	Coordinates* shotOff9 = phys.VectorToOffset(PROJECTILE_SPEED, FM_PI_2 / 4 * 9);
	Coordinates* shotOff10 = phys.VectorToOffset(PROJECTILE_SPEED, FM_PI_2 / 4 * 10);
	Coordinates* shotOff11 = phys.VectorToOffset(PROJECTILE_SPEED, FM_PI_2 / 4 * 11);
	Coordinates* shotOff12 = phys.VectorToOffset(PROJECTILE_SPEED, FM_PI_2 / 4 * 12);
	Coordinates* shotOff13 = phys.VectorToOffset(PROJECTILE_SPEED, FM_PI_2 / 4 * 13);
	Coordinates* shotOff14 = phys.VectorToOffset(PROJECTILE_SPEED, FM_PI_2 / 4 * 14);
	Coordinates* shotOff15 = phys.VectorToOffset(PROJECTILE_SPEED, FM_PI_2 / 4 * 15);
	Coordinates* shotOff16 = phys.VectorToOffset(PROJECTILE_SPEED, FM_PI_2 / 4 * 16);


	//Coordinates shotDelta = Coordinates(entOff->X + shotOff->X, entOff->Y + shotOff->Y);

	//entities->push_back(Entity(originX, originY, PROJECTILE_SIZE, PROJECTILE_SIZE, shotDelta.X, shotDelta.Y, al_map_rgb(20, 220, 20), PROJECTILE, burstID));
	entities->push_back(Entity(originX, originY, PROJECTILE_SIZE, PROJECTILE_SIZE, entOff->X + shotOff1->X, entOff->Y + shotOff1->Y, al_map_rgb(20, 220, 20), PROJECTILE, burstID));
	entities->push_back(Entity(originX, originY, PROJECTILE_SIZE, PROJECTILE_SIZE, entOff->X + shotOff2->X, entOff->Y + shotOff2->Y, al_map_rgb(20, 220, 20), PROJECTILE, burstID));
	entities->push_back(Entity(originX, originY, PROJECTILE_SIZE, PROJECTILE_SIZE, entOff->X + shotOff3->X, entOff->Y + shotOff3->Y, al_map_rgb(20, 220, 20), PROJECTILE, burstID));
	entities->push_back(Entity(originX, originY, PROJECTILE_SIZE, PROJECTILE_SIZE, entOff->X + shotOff4->X, entOff->Y + shotOff4->Y, al_map_rgb(20, 220, 20), PROJECTILE, burstID));
	entities->push_back(Entity(originX, originY, PROJECTILE_SIZE, PROJECTILE_SIZE, entOff->X + shotOff5->X, entOff->Y + shotOff5->Y, al_map_rgb(20, 220, 20), PROJECTILE, burstID));
	entities->push_back(Entity(originX, originY, PROJECTILE_SIZE, PROJECTILE_SIZE, entOff->X + shotOff6->X, entOff->Y + shotOff6->Y, al_map_rgb(20, 220, 20), PROJECTILE, burstID));
	entities->push_back(Entity(originX, originY, PROJECTILE_SIZE, PROJECTILE_SIZE, entOff->X + shotOff7->X, entOff->Y + shotOff7->Y, al_map_rgb(20, 220, 20), PROJECTILE, burstID));
	entities->push_back(Entity(originX, originY, PROJECTILE_SIZE, PROJECTILE_SIZE, entOff->X + shotOff8->X, entOff->Y + shotOff8->Y, al_map_rgb(20, 220, 20), PROJECTILE, burstID));
	entities->push_back(Entity(originX, originY, PROJECTILE_SIZE, PROJECTILE_SIZE, entOff->X + shotOff9->X, entOff->Y + shotOff9->Y, al_map_rgb(20, 220, 20), PROJECTILE, burstID));
	entities->push_back(Entity(originX, originY, PROJECTILE_SIZE, PROJECTILE_SIZE, entOff->X + shotOff10->X, entOff->Y + shotOff10->Y, al_map_rgb(20, 220, 20), PROJECTILE, burstID));
	entities->push_back(Entity(originX, originY, PROJECTILE_SIZE, PROJECTILE_SIZE, entOff->X + shotOff11->X, entOff->Y + shotOff11->Y, al_map_rgb(20, 220, 20), PROJECTILE, burstID));
	entities->push_back(Entity(originX, originY, PROJECTILE_SIZE, PROJECTILE_SIZE, entOff->X + shotOff12->X, entOff->Y + shotOff12->Y, al_map_rgb(20, 220, 20), PROJECTILE, burstID));
	entities->push_back(Entity(originX, originY, PROJECTILE_SIZE, PROJECTILE_SIZE, entOff->X + shotOff13->X, entOff->Y + shotOff13->Y, al_map_rgb(20, 220, 20), PROJECTILE, burstID));
	entities->push_back(Entity(originX, originY, PROJECTILE_SIZE, PROJECTILE_SIZE, entOff->X + shotOff14->X, entOff->Y + shotOff14->Y, al_map_rgb(20, 220, 20), PROJECTILE, burstID));
	entities->push_back(Entity(originX, originY, PROJECTILE_SIZE, PROJECTILE_SIZE, entOff->X + shotOff15->X, entOff->Y + shotOff15->Y, al_map_rgb(20, 220, 20), PROJECTILE, burstID));
	entities->push_back(Entity(originX, originY, PROJECTILE_SIZE, PROJECTILE_SIZE, entOff->X + shotOff16->X, entOff->Y + shotOff16->Y, al_map_rgb(20, 220, 20), PROJECTILE, burstID));

	delete shotOff1,
		shotOff2,
		shotOff3,
		shotOff4,
		shotOff5,
		shotOff6,
		shotOff7,
		shotOff8,
		shotOff9,
		shotOff10,
		shotOff11,
		shotOff12,
		shotOff13,
		shotOff14,
		shotOff15,
		shotOff16;
}

void AsyncPhysics(void* struc)
{
	list<WorldBlock>* world = ((PhysicsVariables*)struc)->world;
	list<Entity>* entities = ((PhysicsVariables*)struc)->entities;


	while (!quit)
	{
		al_rest(1.0 / PHYSICS_TICK);
		phys.ApplyPhysics(entities, world);
	}
	

	_endthread();
}