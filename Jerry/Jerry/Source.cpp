// TOD? : Add noclip
// TOD? : Maek returns const so that they cannot be eddited
// DONE : Fix roation twitch
// DONE : Remove the vucking fector
// DONE : Stop in mind air on direction change
// DONE : Following trail
// DONE : Air controls
// DONE : Projectile trajectory
// DONE : Bounce
// TODO : Level
// TODO : Naming and names mang. Projectile particle etc....
// TODO : Use the recursion in Collision() so that it doesn't need recalulation later on
// TODO : In air Entity collision
// TODO : Find some crash in the drawing of the projectile trajectory
// TODO : Preformance optimizing
// TODO : Physics tick counter

// TODO : Portfolio : Langton's ant, BiZZdesign werk, PVB?

#define ALLEGRO_STATICLINK
#define _USE_MATH_DEFINES 

#include <mutex>
#include <thread>
#include <iostream>
#include <list>
#include <vector>
#include <cmath>
#include <string>
#include <mutex>
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
void move(Entity* player);
void shoot(list<Entity>* entities, ALLEGRO_EVENT e);
void AsyncPhysics(void* struc);

Physics phys;
ALLEGRO_FONT *font = 0;
bool quit = false;
bool keys[5] = { false, false, false, false, false };
string keyName[5] = { "R", "U", "L", "D", "LCTRL" };
bool mouseButtons[3] = { false, false, false };
mutex mtx;

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

	font = al_load_ttf_font("coolvetica.ttf", 17, 0);

	if (!font)
	{
		fprintf(stderr, "Failed to load font\n");
		system("pause");
		return -1;
	}

	PlayerColor = al_map_rgb(20, 20, 220);
	JellyColor = al_map_rgb(20, 220, 20);
	WorldColor = al_map_rgb(20, 20, 20);
	BadWorldColor = al_map_rgb(220, 20, 20);

	entities->push_back(Entity(Spawn.X, Spawn.Y, 20, 20, 0.0, 0.0, PlayerColor, PLAYER));
	Entity* player = &(*entities->begin());

	// world outlining
	world->push_back(WorldBlock(0, 590, 1280, 600, WorldColor, WORLD));
	world->push_back(WorldBlock(0, 0, 10, 600, WorldColor, WORLD));
	world->push_back(WorldBlock(0, 0, 1280, 10, WorldColor, WORLD));
	world->push_back(WorldBlock(1270, 0, 1280, 600, WorldColor, WORLD));

	world->push_back(WorldBlock(350, 500, 360, 600, WorldColor, WORLD));
	world->push_back(WorldBlock(100, 589, 150, 600, BadWorldColor, BADWORLD));

	////center bar
	//world->push_back(WorldBlock(300, 300, 900, 310, WorldColor, WORLD));
	//world->push_back(WorldBlock(400, 200, 500, 201, BadWorldColor, BADWORLD));
	//world->push_back(WorldBlock(1095, 400, 1100, 600, WorldColor, WORLD));
	//world->push_back(WorldBlock(900, 400, 905, 600, WorldColor, WORLD));

	////center bar stripes
	//world->push_back(WorldBlock(650, 280, 655, 330, WorldColor, WORLD));
	//world->push_back(WorldBlock(660, 280, 665, 330, WorldColor, WORLD));
	//world->push_back(WorldBlock(670, 280, 675, 330, WorldColor, WORLD));
	//world->push_back(WorldBlock(680, 280, 685, 330, WorldColor, WORLD));
	//world->push_back(WorldBlock(690, 280, 695, 330, WorldColor, WORLD));
	//world->push_back(WorldBlock(700, 280, 705, 330, WorldColor, WORLD));
	//world->push_back(WorldBlock(710, 280, 715, 330, WorldColor, WORLD));
	//world->push_back(WorldBlock(720, 280, 725, 330, WorldColor, WORLD));
	//world->push_back(WorldBlock(730, 280, 735, 330, WorldColor, WORLD));
	//world->push_back(WorldBlock(740, 280, 745, 330, WorldColor, WORLD));
	//world->push_back(WorldBlock(750, 280, 755, 330, WorldColor, WORLD));
	//world->push_back(WorldBlock(760, 280, 765, 330, WorldColor, WORLD));
	//world->push_back(WorldBlock(770, 280, 775, 330, WorldColor, WORLD));
	//world->push_back(WorldBlock(780, 280, 785, 330, WorldColor, WORLD));
	//world->push_back(WorldBlock(790, 280, 795, 330, WorldColor, WORLD));
	//world->push_back(WorldBlock(800, 280, 805, 330, WorldColor, WORLD));

	//world->push_back(WorldBlock(340, 280, 345, 330, WorldColor, WORLD));
	//world->push_back(WorldBlock(365, 280, 370, 330, WorldColor, WORLD));

	////top left stripes
	////x
	//world->push_back(WorldBlock(30, 10, 35, 30, WorldColor, WORLD));
	//world->push_back(WorldBlock(40, 10, 45, 30, WorldColor, WORLD));
	//world->push_back(WorldBlock(50, 10, 55, 30, WorldColor, WORLD));
	//world->push_back(WorldBlock(60, 10, 65, 30, WorldColor, WORLD));
	//world->push_back(WorldBlock(70, 10, 75, 30, WorldColor, WORLD));
	//world->push_back(WorldBlock(80, 10, 85, 30, WorldColor, WORLD));
	//world->push_back(WorldBlock(90, 10, 95, 30, WorldColor, WORLD));
	//world->push_back(WorldBlock(100, 10, 105, 30, WorldColor, WORLD));

	////y
	//world->push_back(WorldBlock(10, 30, 30, 35, WorldColor, WORLD));
	//world->push_back(WorldBlock(10, 40, 30, 45, WorldColor, WORLD));
	//world->push_back(WorldBlock(10, 50, 30, 55, WorldColor, WORLD));
	//world->push_back(WorldBlock(10, 60, 30, 65, WorldColor, WORLD));
	//world->push_back(WorldBlock(10, 70, 30, 75, WorldColor, WORLD));
	//world->push_back(WorldBlock(10, 80, 30, 85, WorldColor, WORLD));
	//world->push_back(WorldBlock(10, 90, 30, 95, WorldColor, WORLD));
	//world->push_back(WorldBlock(10, 100, 30, 105, WorldColor, WORLD));

	// User input and drawing
	al_register_event_source(eventQueue, al_get_timer_event_source(frame));
	al_register_event_source(eventQueue, al_get_display_event_source(display));
	al_register_event_source(eventQueue, al_get_keyboard_event_source());
	al_register_event_source(eventQueue, al_get_mouse_event_source());


	//void AsyncPhysics(void* void_tickQueue, void* void_entities, void* void_world)
	PhysicsVariables physVar;
	physVar.entities = entities;
	physVar.world = world;

	thread asyncPhys = thread(AsyncPhysics, (void*)&physVar);

	// TODO: Meak pretti fix for this
	al_grab_mouse(display);

	al_start_timer(frame);

	while (!quit)
	{
		ALLEGRO_EVENT e;

		al_wait_for_event(eventQueue, &e);

		if (e.type == ALLEGRO_EVENT_TIMER)
		{
			draw(entities, world);
		}
		else if (e.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN)
		{
			switch (e.mouse.button)
			{
			case 1:
				shoot(entities, e);
				//mouseButtons[LMB] = true;
				break;
			case 2:
				mouseButtons[RMB] = true;
				Particles = 0;
				break;
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

			case ALLEGRO_KEY_LCTRL:
				keys[LCTRL] = true;
				break;

			case ALLEGRO_KEY_ESCAPE:
				quit = true;
				break;

			case ALLEGRO_KEY_R:
				player->SetCoordinates(Spawn.X, Spawn.Y);
				break;

			case ALLEGRO_KEY_T:
				al_clear_to_color(al_map_rgb(220, 220, 220));
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

			case ALLEGRO_KEY_LCTRL:
				keys[LCTRL] = false;
				break;
			}
		}
		else if (e.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
		{
			quit = true;
		}
	}

	al_rest(0.1);

	asyncPhys.detach();

	delete world, entities;

	return 0;
}

void draw(list<Entity> *entities, list<WorldBlock> *world)
{
	al_clear_to_color(al_map_rgb(220, 220, 220));

	Entity* player = &*entities->begin();
	ALLEGRO_MOUSE_STATE mouse;
	al_get_mouse_state(&mouse);
	Coordinates posA = *player->GetACoordinates();
	double originX = posA.X + player->GetWidth() / 2 - PROJECTILE_SIZE / 2;
	double originY = posA.Y + player->GetHeight() / 2 - PROJECTILE_SIZE / 2;
	double angle = phys.OffsetToAngle((originX - (double)mouse.x + PROJECTILE_SIZE / 2) * -1, (originY - (double)mouse.y + PROJECTILE_SIZE / 2) * -1);
	Coordinates* gunVec = phys.VectorToOffset(15.0, angle);
	Coordinates* blankGunVec = phys.VectorToOffset(5.0, angle);
	Coordinates* entOff = player->GetOffset();
	Coordinates* shotVec = phys.VectorToOffset(PROJECTILE_SPEED, angle);
	list<Entity> hackhack = { Entity(originX, originY, PROJECTILE_SIZE, PROJECTILE_SIZE, shotVec->X + entOff->X, shotVec->Y + entOff->Y, al_map_rgb(180, 180, 180), PROJECTILE) };
	list<Entity>::iterator ghostShot = hackhack.begin();

	Coordinates last = Coordinates(-1, -1);

	for (int i = 0; i < MAX_COLLISION_PREDICTION; ++i)
	{
		phys.ApplyGravity(&*ghostShot);
		phys.Collide(ghostShot, world, entities);
		ghostShot->MoveToOffset();

		if (last.X == ghostShot->GetACoordinates()->X && last.Y == ghostShot->GetACoordinates()->Y || ghostShot->GetDelete())
		{
			break;
		}
		else
		{
 			al_draw_filled_rectangle(ghostShot->GetACoordinates()->X, ghostShot->GetACoordinates()->Y, ghostShot->GetBCoordinates()->X, ghostShot->GetBCoordinates()->Y, ghostShot->GetColor());
			last.X = ghostShot->GetACoordinates()->X;
			last.Y = ghostShot->GetACoordinates()->Y;
		}
	}

	mtx.lock();
	for (list<Entity>::iterator ent = ++entities->begin(); ent != entities->end(); ++ent)
	{
		Coordinates* posA = ent->GetACoordinates();
		al_draw_filled_rectangle(posA->X, posA->Y, posA->X + ent->GetWidth(), posA->Y + ent->GetHeight(), ent->GetColor());

		if (!ent->GetHit())
		{
			double lastY = 0;

			for (int i = 1; i < PROJECTILE_TRAIL_LENGTH; ++i)
			{
				al_draw_filled_rectangle(
					posA->X - ent->GetOffset()->X * i,
					posA->Y - (lastY + (ent->GetOffset()->Y - (GRAVITY * i))),
					posA->X + ent->GetWidth() - ent->GetOffset()->X * i,
					posA->Y + ent->GetHeight() - (lastY + (ent->GetOffset()->Y - (GRAVITY * i))),
					ent->GetColor());

				lastY += (ent->GetOffset()->Y - (GRAVITY * i));
			}
		}
	}
	mtx.unlock();

	for (list<WorldBlock>::iterator wBlock = world->begin(); wBlock != world->end(); ++wBlock)
	{
		al_draw_filled_rectangle(wBlock->GetA()->X, wBlock->GetA()->Y, wBlock->GetB()->X, wBlock->GetB()->Y, wBlock->GetColor());
	}

	// Player
	al_draw_filled_rectangle(posA.X, posA.Y, posA.X + player->GetWidth(), posA.Y + player->GetHeight(), player->GetColor());

	// Gun of player
	al_draw_line(originX + PROJECTILE_SIZE / 2, originY + PROJECTILE_SIZE / 2, gunVec->X + originX + PROJECTILE_SIZE / 2, gunVec->Y + originY + PROJECTILE_SIZE / 2, BadWorldColor, 5.0);
	al_draw_line(originX + PROJECTILE_SIZE / 2, originY + PROJECTILE_SIZE / 2, blankGunVec->X + originX + PROJECTILE_SIZE / 2, blankGunVec->Y + originY + PROJECTILE_SIZE / 2, PlayerColor, 5.0);

	mtx.lock();
	// Gun Charge
	struct ts { int i; list<Entity>::iterator ent; };

	for (ts t = { 0, ++entities->begin() }; t.ent != entities->end(); ++t.ent, ++t.i)
	{
		al_draw_filled_rectangle(posA.X + t.i * 7, posA.Y - 7, posA.X + t.i * 7 + 5, posA.Y - 5, JellyColor);
		al_draw_filled_rectangle(posA.X + t.i * 7, posA.Y - 7, posA.X + t.i * 7 + (t.ent->GetAge() / MAX_ENTITY_AGE * 5), posA.Y - 5, al_map_rgb(20, 20, 220));
	}
	mtx.unlock();

	string pressed = "";

	for (int i = 0; i < sizeof(keys) / sizeof(*keys); ++i)
	{
		if (keys[i])
		{
			pressed += keyName[i] + ", ";
		}
	}



	// Jelly amount
	al_draw_text(font, WorldColor, 1000, 10, 0, ("Total particles: " + to_string(Particles)).c_str());
	al_draw_text(font, WorldColor, 1000, 25, 0, ("Active particles: " + to_string(ActiveParticles)).c_str());
	al_draw_text(font, WorldColor, 1000, 40, 0, ("Player X: " + to_string(player->GetOffset()->X)).c_str());
	al_draw_text(font, WorldColor, 1000, 55, 0, ("Player Y: " + to_string(player->GetOffset()->Y)).c_str());
	al_draw_text(font, WorldColor, 1000, 70, 0, ("Pressed keys: " + pressed).c_str());


	al_flip_display();

}

void move(Entity* ent)
{
	Coordinates* offset = ent->GetOffset();

	if (keys[RIGHT])
	{
		if (offset->X < 0.0 && ent->GetIsAirBorn())
		{
			offset->X *= PLAYER_AIR_CONTROL_BREAK;
		}

		offset->X += PLAYER_SPEED * ent->GetIsAirBorn() ? PLAYER_AIR_CONTROL : 1.0;
	}

	if (keys[LEFT])
	{
		if (offset->X > 0.0 && ent->GetIsAirBorn())
		{
			offset->X *= PLAYER_AIR_CONTROL_BREAK;
		}

		offset->X -= PLAYER_SPEED * ent->GetIsAirBorn() ? PLAYER_AIR_CONTROL : 1.0;
	}

	if (keys[UP])
	{
		if (!ent->GetIsAirBorn())
		{
			offset->Y -= PLAYER_JUMP_SPEED;
		}
	}

	if (keys[LCTRL])
	{
		ent->SetIsCrouching(true);
	}
	else
	{
		ent->SetIsCrouching(false);
	}
}

// TODO : Check nececerity of convertions
// TODO : Mang, dat copy of event
void shoot(list<Entity>* entities, ALLEGRO_EVENT e)
{
	if (entities->size() -1 < MAX_ENTITIES)
	{
		Entity* player = &*entities->begin();
		Coordinates* entPos = player->GetACoordinates();
		Coordinates* entOff = player->GetOffset();
		double originX = entPos->X + player->GetWidth() / 2 - PROJECTILE_SIZE / 2;
		double originY = entPos->Y + player->GetHeight() / 2 - PROJECTILE_SIZE / 2;
		Coordinates* shotOff = phys.VectorToOffset(PROJECTILE_SPEED, phys.OffsetToAngle((originX - e.mouse.x + PROJECTILE_SIZE / 2) * -1, (originY - e.mouse.y + PROJECTILE_SIZE / 2) * -1));

		entities->insert(++entities->begin(), Entity(originX, originY, PROJECTILE_SIZE, PROJECTILE_SIZE, entOff->X + shotOff->X, entOff->Y + shotOff->Y, JellyColor, PROJECTILE));

		++Particles;

		delete shotOff;
	}
}

void AsyncPhysics(void* struc)
{
	list<WorldBlock>* world = ((PhysicsVariables*)struc)->world;
	list<Entity>* entities = ((PhysicsVariables*)struc)->entities;
	Entity* player = &(*entities->begin());

	while (!quit)
	{
		al_rest(1.0 / PHYSICS_TICK);
		
		mtx.lock();
		move(player);
		phys.ApplyPhysics(entities, world);
		mtx.unlock();

		if (mouseButtons[RMB])
		{
			mtx.lock();
			while (entities->size() > 1)
			{
				entities->pop_back();
			}
			mtx.unlock();
			mouseButtons[RMB] = false;
		}
	}
}