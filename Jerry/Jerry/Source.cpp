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
// TODO : Jelly into the wall

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
#include <sstream>
#include <mutex>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_font.h>
#include "Entity.h"
#include "WorldBlock.h"
#include "Physics.h"

using namespace std;

void Draw(list<Entity>* ent, list<WorldBlock>* world);
void Move(Entity* player);
void Shoot(list<Entity>* entities, ALLEGRO_EVENT e);
void AsyncPhysics(void* struc);
void DevConsole();

Physics phys;
ALLEGRO_FONT *font = 0;
bool quit = false;
bool keys[5] = { false, false, false, false, false };
string keyName[5] = { "R", "U", "L", "D", "LCTRL" };
string collPosName[4] = { "LX", "RX", "UY", "DY" };
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
	ALLEGRO_DISPLAY_MODE disp_data;
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

	//al_get_display_mode(al_get_num_display_modes() - 1, &disp_data);
	//al_get_display_mode(0, &disp_data);
	//al_set_new_display_flags(ALLEGRO_FULLSCREEN);
	//al_set_new_display_adapter(1);
	//display = al_create_display(disp_data.width, disp_data.height);
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
	JellyWorldColor = al_map_rgb(20, 20, 220);

	entities->push_back(Entity(Spawn.X, Spawn.Y, 20, 20, 0.0, 0.0, PlayerColor, PLAYER));
	Entity* player = &(*entities->begin());

	world->push_back(WorldBlock(300, 330, 240, 10, JellyWorldColor, JELLYWORLD));
	world->push_back(WorldBlock(300, 230, 240, 10, JellyWorldColor, JELLYWORLD));
	world->push_back(WorldBlock(640, 280, 30, 60, BadWorldColor, BADWORLD));
	world->push_back(WorldBlock(730, 280, 30, 60, BadWorldColor, BADWORLD));
	world->push_back(WorldBlock(820, 280, 30, 60, BadWorldColor, BADWORLD));
	world->push_back(WorldBlock(160, 590, 50, 10, BadWorldColor, BADWORLD));
	world->push_back(WorldBlock(1160, 340, 10, 90, WorldColor, WORLD));
	world->push_back(WorldBlock(0, 0, 1280, 10, WorldColor, WORLD));
	world->push_back(WorldBlock(10, 590, 150, 10, WorldColor, WORLD));
	world->push_back(WorldBlock(1270, 10, 10, 580, WorldColor, WORLD));
	world->push_back(WorldBlock(10, 340, 1150, 10, WorldColor, WORLD));
	world->push_back(WorldBlock(0, 10, 10, 590, WorldColor, WORLD));
	world->push_back(WorldBlock(210, 590, 1070, 10, WorldColor, WORLD));
	world->push_back(WorldBlock(540, 210, 730, 10, WorldColor, WORLD));
	world->push_back(WorldBlock(300, 210, 240, 20, WorldColor, WORLD));
	world->push_back(WorldBlock(250, 580, 10, 10, WorldColor, WORLD));
	world->push_back(WorldBlock(310, 570, 10, 20, WorldColor, WORLD));
	world->push_back(WorldBlock(370, 560, 10, 30, WorldColor, WORLD));
	world->push_back(WorldBlock(430, 550, 10, 40, WorldColor, WORLD));
	world->push_back(WorldBlock(490, 540, 10, 50, WorldColor, WORLD));
	world->push_back(WorldBlock(550, 530, 10, 60, WorldColor, WORLD));
	world->push_back(WorldBlock(610, 520, 10, 70, WorldColor, WORLD));
	world->push_back(WorldBlock(670, 510, 10, 80, WorldColor, WORLD));
	world->push_back(WorldBlock(730, 500, 10, 90, WorldColor, WORLD));
	world->push_back(WorldBlock(790, 490, 10, 100, WorldColor, WORLD));
	world->push_back(WorldBlock(850, 480, 10, 110, WorldColor, WORLD));
	world->push_back(WorldBlock(910, 470, 10, 120, WorldColor, WORLD));
	world->push_back(WorldBlock(1160, 470, 10, 120, WorldColor, WORLD));
	world->push_back(WorldBlock(540, 220, 10, 20, WorldColor, WORLD));

	// User input and drawing
	al_register_event_source(eventQueue, al_get_timer_event_source(frame));
	al_register_event_source(eventQueue, al_get_display_event_source(display));
	al_register_event_source(eventQueue, al_get_keyboard_event_source());
	al_register_event_source(eventQueue, al_get_mouse_event_source());


	//void AsyncPhysics(void* void_tickQueue, void* void_entities, void* void_world)
	PhysicsVariables physVar;
	physVar.entities = entities;
	physVar.world = world;

	thread physThread = thread(AsyncPhysics, (void*)&physVar);
	thread consoleThread = thread(DevConsole);

	// TODO: Meak pretti fix for this
	al_grab_mouse(display);

	al_start_timer(frame);

	while (!quit)
	{
		ALLEGRO_EVENT e;

		al_wait_for_event(eventQueue, &e);

		if (e.type == ALLEGRO_EVENT_TIMER)
		{
			Draw(entities, world);
		}
		else if (e.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN)
		{
			switch (e.mouse.button)
			{
			case 1:
				Shoot(entities, e);
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

	physThread.detach();
	consoleThread.detach();

	delete world, entities;

	return 0;
}

void Draw(list<Entity> *entities, list<WorldBlock> *world)
{
	al_clear_to_color(al_map_rgb(220, 220, 220));

	Entity* player = &*entities->begin();
	ALLEGRO_MOUSE_STATE mouse;
	al_get_mouse_state(&mouse);
	Coordinates posA = *player->GetACoordinates();
	double originX = posA.X + player->GetWidth() / 2 - PROJECTILE_SIZE / 2;
	double originY = posA.Y + player->GetHeight() / 2 - PROJECTILE_SIZE / 2;
	double angle = phys.OffsetToAngle((player->GetWidth() / 2 - PROJECTILE_SIZE / 2 - mouse.x + (SCREEN_W / 2 - player->GetWidth() / 2)) * -1, (originY - mouse.y + PROJECTILE_SIZE / 2) * -1);
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
 			al_draw_filled_rectangle(
				ghostShot->GetACoordinates()->X + (SCREEN_W / 2 - player->GetWidth() / 2) + posA.X * -1,
				ghostShot->GetACoordinates()->Y,
				ghostShot->GetBCoordinates()->X + (SCREEN_W / 2 - player->GetWidth() / 2) + posA.X * -1,
				ghostShot->GetBCoordinates()->Y,
				ghostShot->GetColor());
			last.X = ghostShot->GetACoordinates()->X;
			last.Y = ghostShot->GetACoordinates()->Y;
		}
	}

	for (list<WorldBlock>::iterator wBlock = world->begin(); wBlock != world->end(); ++wBlock)
	{
		al_draw_filled_rectangle(
			wBlock->GetA()->X + (SCREEN_W / 2 - player->GetWidth() / 2) + posA.X * -1,
			wBlock->GetA()->Y,
			wBlock->GetB()->X + (SCREEN_W / 2 - player->GetWidth() / 2) + posA.X * -1,
			wBlock->GetB()->Y,
			wBlock->GetColor());
	}

	mtx.lock();
	for (list<Entity>::iterator ent = ++entities->begin(); ent != entities->end(); ++ent)
	{
		Coordinates* entA = ent->GetACoordinates();

		al_draw_filled_rectangle(entA->X + (SCREEN_W / 2 - player->GetWidth() / 2) + posA.X * -1, entA->Y, entA->X + ent->GetWidth() + (SCREEN_W / 2 - player->GetWidth() / 2) + posA.X * -1, entA->Y + ent->GetHeight(), ent->GetColor());

		if (!ent->GetHit())
		{
			double lastY = 0;

			for (int i = 1; i < PROJECTILE_TRAIL_LENGTH; ++i)
			{
				al_draw_filled_rectangle(
					entA->X - ent->GetOffset()->X * i + (SCREEN_W / 2 - player->GetWidth() / 2) + posA.X * -1,
					entA->Y - (lastY + (ent->GetOffset()->Y - (GRAVITY * i))),
					entA->X + ent->GetWidth() - ent->GetOffset()->X * i + (SCREEN_W / 2 - player->GetWidth() / 2) + posA.X * -1,
					entA->Y + ent->GetHeight() - (lastY + (ent->GetOffset()->Y - (GRAVITY * i))),
					ent->GetColor());

				lastY += (ent->GetOffset()->Y - (GRAVITY * i));
			}
		}
	}
	mtx.unlock();

	// Player
	al_draw_filled_rectangle(SCREEN_W / 2 - player->GetWidth() / 2, posA.Y, SCREEN_W / 2 - player->GetWidth() / 2 + player->GetWidth(), posA.Y + player->GetHeight(), player->GetColor());

	// Gun of player
	al_draw_line((SCREEN_W / 2), originY + PROJECTILE_SIZE / 2, gunVec->X +      SCREEN_W / 2, gunVec->Y      + originY + PROJECTILE_SIZE / 2, BadWorldColor, 5.0);
	al_draw_line((SCREEN_W / 2), originY + PROJECTILE_SIZE / 2, blankGunVec->X + SCREEN_W / 2, blankGunVec->Y + originY + PROJECTILE_SIZE / 2, PlayerColor, 5.0);

	mtx.lock();
	// Gun Charge
	struct ts { int i; list<Entity>::iterator ent; };

	for (ts t = { 0, ++entities->begin() }; t.ent != entities->end(); ++t.ent, ++t.i)
	{
		al_draw_filled_rectangle(SCREEN_W / 2 - player->GetWidth() / 2 + t.i * 7, posA.Y - 7, SCREEN_W / 2 - player->GetWidth() / 2 + t.i * 7 + 5, posA.Y - 5, JellyColor);
		al_draw_filled_rectangle(SCREEN_W / 2 - player->GetWidth() / 2 + t.i * 7, posA.Y - 7, SCREEN_W / 2 - player->GetWidth() / 2 + t.i * 7 + (t.ent->GetAge() / MAX_ENTITY_AGE * 5), posA.Y - 5, al_map_rgb(20, 20, 220));
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
	al_draw_text(font, WorldColor, 1000, 85, 0, ("Is airborn: " + to_string(player->GetIsAirBorn())).c_str());
	al_draw_text(font, WorldColor, 1000, 100, 0, ("Last CollPos: " + collPosName[player->GetLastColPos()]).c_str());

	al_flip_display();
}

void Move(Entity* ent)
{
	Coordinates* offset = ent->GetOffset();

	if (!FLY)
	{
		if (keys[RIGHT])
		{
			if (offset->X < 0.0 && ent->GetIsAirBorn())
			{
				offset->X *= PLAYER_AIR_CONTROL_BREAK;
			}

			if (!(ent->GetLastImpactType() == JELLY && (ent->GetLastColPos() == RX || ent->GetLastColPos() == LX)))
			{
				offset->X += PLAYER_SPEED * ent->GetIsAirBorn() ? PLAYER_AIR_CONTROL : 1.0;
			}
		}

		if (keys[LEFT])
		{
			if (offset->X > 0.0 && ent->GetIsAirBorn())
			{
				offset->X *= PLAYER_AIR_CONTROL_BREAK;
			}

			if (!(ent->GetLastImpactType() == JELLY && (ent->GetLastColPos() == RX || ent->GetLastColPos() == LX)))
			{
				offset->X -= PLAYER_SPEED * ent->GetIsAirBorn() ? PLAYER_AIR_CONTROL : 1.0;
			}
		}

		if (keys[UP])
		{
			if (!ent->GetIsAirBorn())
			{
				offset->Y -= PLAYER_JUMP_SPEED;
			}
		}
	}
	else
	{
		if (keys[UP])
			--offset->Y;
		if (keys[DOWN])
			++offset->Y;
		if (keys[RIGHT])
			++offset->X;
		if (keys[LEFT])
			--offset->X;
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
void Shoot(list<Entity>* entities, ALLEGRO_EVENT e)
{
	if (entities->size() -1 < MAX_ENTITIES)
	{
		Entity* player = &*entities->begin();
		Coordinates* entPos = player->GetACoordinates();
		Coordinates* entOff = player->GetOffset();
		double originX = entPos->X + player->GetWidth() / 2 - PROJECTILE_SIZE / 2;
		double originY = entPos->Y + player->GetHeight() / 2 - PROJECTILE_SIZE / 2;
		Coordinates* shotOff = phys.VectorToOffset(PROJECTILE_SPEED, phys.OffsetToAngle((player->GetHeight() / 2 - PROJECTILE_SIZE / 2 - e.mouse.x + (SCREEN_W / 2 - player->GetWidth() / 2)) * -1, (originY - e.mouse.y + PROJECTILE_SIZE / 2) * -1));

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
		Move(player);
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

void DevConsole()
{
	string input = "";

	while (!quit)
	{ 
		cout << ">";
		
		getline(cin, input);

		int splitIndex = input.find(" ");
		string command = "";
		string value = "";

		if (splitIndex != -1)
		{
			command = input.substr(0, splitIndex);
			value = input.substr(++splitIndex, input.length() - 1);

			if (command == "GRAVITY")
			{
				GRAVITY = stod(value) / FPS;
				cout << "GRAVITY set to: " << GRAVITY << endl;
			}
			else if (command == "PHYSICS_TICK")
			{
				PHYSICS_TICK = stod(value);
				cout << "PHYSICS_TICK set to: " << PHYSICS_TICK << endl;
			}
			else if (command == "PLAYER_SPEED")
			{
				PLAYER_JUMP_SPEED = stod(value);
				cout << "PLAYER_SPEED set to: " << PLAYER_JUMP_SPEED << endl;
			}
			else if (command == "PLAYER_JUMP_SPEED")
			{
				PLAYER_JUMP_SPEED = stod(value);
				cout << "PLAYER_JUMP_SPEED set to: " << PLAYER_JUMP_SPEED << endl;
			}
			else if (command == "PLAYER_BOUNCE_OFFSET")
			{
				PLAYER_BOUNCE_OFFSET = stod(value);
				cout << "PLAYER_BOUNCE_OFFSET set to: " << PLAYER_BOUNCE_OFFSET << endl;
			}
			else if (command == "PLAYER_AIR_CONTROL")
			{
				PLAYER_AIR_CONTROL = stod(value);
				cout << "PLAYER_AIR_CONTROL set to: " << PLAYER_AIR_CONTROL << endl;
			}
			else if (command == "PLAYER_AIR_CONTROL_BREAK")
			{
				PLAYER_AIR_CONTROL_BREAK = stod(value);
				cout << "PLAYER_AIR_CONTROL_BREAK set to: " << PLAYER_AIR_CONTROL_BREAK << endl;
			}
			else if (command == "PLAYER_SIDE_UP_BOUNCE")
			{
				PLAYER_SIDE_UP_BOUNCE = stod(value);
				cout << "PLAYER_SIDE_UP_BOUNCE set to: " << PLAYER_SIDE_UP_BOUNCE << endl;
			}
			else if (command == "PLAYER_SIDE_SIDE_BOUNCE")
			{
				PLAYER_SIDE_SIDE_BOUNCE = stod(value);
				cout << "PLAYER_SIDE_SIDE_BOUNCE set to: " << PLAYER_SIDE_SIDE_BOUNCE << endl;
			}
			else if (command == "PROJECTILE_SPEED")
			{
				PROJECTILE_SPEED = stod(value);
				cout << "PROJECTILE_SPEED set to: " << PROJECTILE_SPEED << endl;
			}
			else if (command == "PROJECTILE_SIZE")
			{
				PROJECTILE_SIZE = stod(value);
				cout << "PROJECTILE_SIZE set to: " << PROJECTILE_SIZE << endl;
			}
			else if (command == "PROJECTILE_BOUNCINESS")
			{
				PROJECTILE_BOUNCINESS = stod(value);
				cout << "PROJECTILE_BOUNCINESS set to: " << PROJECTILE_BOUNCINESS << endl;
			}
			else if (command == "MAX_ENTITIES")
			{
				MAX_ENTITIES = stod(value);
				cout << "MAX_ENTITIES set to: " << MAX_ENTITIES << endl;
			}
			else if (command == "MAX_ENTITY_AGE")
			{
				MAX_ENTITY_AGE = stod(value);
				cout << "MAX_ENTITY_AGE set to: " << MAX_ENTITY_AGE << endl;
			}
			else if (command == "MAX_ENTITY_VELOCITY")
			{
				MAX_ENTITY_VELOCITY = stod(value);
				cout << "MAX_ENTITY_VELOCITY set to: " << MAX_ENTITY_VELOCITY << endl;
			}
			else if (command == "FRICTION")
			{
				FRICTION = stod(value);
				cout << "FRICTION set to: " << FRICTION << endl;
			}
			else if (command == "FRICTION_STOP")
			{
				FRICTION_STOP = stod(value);
				cout << "FRICTION_STOP set to: " << FRICTION_STOP << endl;
			}
			else
			{
				cout << "Command not found\n";
			}

		}
		else
		{
			command = input;
			
			if (command == "help")
			{
				cout << "Availabe commands are: \n" << "PHYSICS_TICK\nGRAVITY\nPLAYER_SPEED\nPLAYER_JUMP_SPEED\nPLAYER_BOUNCE_OFFSET\nPLAYER_AIR_CONTROL\nPLAYER_AIR_CONTROL_BREAK;\nPLAYER_SIDE_UP_BOUNCE\nPLAYER_SIDE_SIDE_BOUNCE\nPROJECTILE_SPEED\nPROJECTILE_SIZE\nPROJECTILE_TRAIL_LENGTH\nPROJECTILE_BOUNCINESS\nMAX_ENTITIES\nMAX_ENTITY_AGE\nMAX_ENTITY_VELOCITY\nFRICTION\nFRICTION_STOP\n\n";
			}
			else if (command == "GRAVITY")
			{
				cout << "GRAVITY is: " << GRAVITY * FPS << endl;
			}
			else if (command == "PHYSICS_TICK")
			{
				cout << "PHYSICS_TICK is: " << PHYSICS_TICK << endl;
			}
			else if (command == "PLAYER_SPEED")
			{
				cout << "PLAYER_SPEED is: " << PLAYER_SPEED << endl;
			}
			else if (command == "PLAYER_BOUNCE_OFFSET")
			{
				cout << "PLAYER_BOUNCE_OFFSET is: " << PLAYER_BOUNCE_OFFSET << endl;
			}
			else if (command == "PLAYER_JUMP_SPEED")
			{
				cout << "PLAYER_JUMP_SPEED is: " << PLAYER_JUMP_SPEED << endl;
			}
			else if (command == "PLAYER_AIR_CONTROL")
			{
				cout << "PLAYER_AIR_CONTROL is: " << PLAYER_AIR_CONTROL << endl;
			}
			else if (command == "PLAYER_AIR_CONTROL_BREAK")
			{
				cout << "PLAYER_AIR_CONTROL_BREAK is: " << PLAYER_AIR_CONTROL_BREAK << endl;
			}
			else if (command == "PLAYER_SIDE_UP_BOUNCE")
			{
				cout << "PLAYER_SIDE_UP_BOUNCE is: " << PLAYER_SIDE_UP_BOUNCE << endl;
			}
			else if (command == "PLAYER_SIDE_SIDE_BOUNCE")
			{
				cout << "PLAYER_SIDE_SIDE_BOUNCE is: " << PLAYER_SIDE_SIDE_BOUNCE << endl;
			}
			else if (command == "PROJECTILE_SPEED")
			{
				cout << "PROJECTILE_SPEED is: " << PROJECTILE_SPEED << endl;
			}
			else if (command == "PROJECTILE_SIZE")
			{
				cout << "PROJECTILE_SIZE is: " << PROJECTILE_SIZE << endl;
			}
			else if (command == "PROJECTILE_BOUNCINESS")
			{
				cout << "PROJECTILE_BOUNCINESS is: " << PROJECTILE_BOUNCINESS << endl;
			}
			else if (command == "MAX_ENTITIES")
			{
				cout << "MAX_ENTITIES is: " << MAX_ENTITIES << endl;
			}
			else if (command == "MAX_ENTITY_AGE")
			{
				cout << "MAX_ENTITY_AGE is: " << MAX_ENTITY_AGE << endl;
			}
			else if (command == "MAX_ENTITY_VELOCITY")
			{
				cout << "MAX_ENTITY_VELOCITY is: " << MAX_ENTITY_VELOCITY << endl;
			}
			else if (command == "FRICTION")
			{
				cout << "FRICTION is: " << FRICTION << endl;
			}
			else if (command == "FRICTION_STOP")
			{
				cout << "FRICTION_STOP is: " << FRICTION_STOP << endl;
			}
			else if (command == "FLY")
			{
				FLY = !FLY;

				cout << "Flying " << (FLY ? "enabled" : "disabled") << ".\n";
			}
			else
			{
				cout << "Command not found\n";
			}
		}
	}
}