// TODO : Level fixes

// Ranges: 
// ----------
// Side: 220
// Side up: 65
// UP: 123

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
#include <algorithm>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <fstream>
#include "Entity.h"
#include "WorldBlock.h"
#include "Physics.h"
#include "WorldEntity.h"
#include "WorldScenery.h"

using namespace std;

void Draw(list<Entity>* ent, list<WorldBlock>* world, list<WorldEntity>* worldEntities, list<WorldScenery>* worldScenery);
void Move(Entity* player);
void Shoot(list<Entity>* entities, ALLEGRO_EVENT* e);
//Thread function for the physics
void AsyncPhysics(void* struc);
//Thread function for the console
void DevConsole();

// Global variables
Physics Phys;
ALLEGRO_FONT* Font = 0;
ALLEGRO_FONT* FontBig = 0;
bool Quit = false;
bool Keys[5] = { false, false, false, false, false };
string KeyName[5] = { "R", "U", "L", "D", "LCTRL" };
string CollPosName[4] = { "LX", "RX", "UY", "DY" };
bool MouseButtons[3] = { false, false, false };
string HighScore = "";
mutex Mtx;
int TotalCoins;
bool Collected = false;
unsigned int TimeScore = 0;

struct PhysicsVariables
{
	list<Entity>* entities;
	list<WorldBlock>* world;
	list<WorldEntity>* worldEntities;
};

struct WorldText
{
	string text;
	ALLEGRO_FONT* font;
	ALLEGRO_COLOR color;
};


int main()
{
	ALLEGRO_DISPLAY* display = 0;
	ALLEGRO_DISPLAY_MODE disp_data;
	ALLEGRO_TIMER* frame = 0;
	// Will contain world items such as World BadWorld and JellyWorld
	list<WorldBlock>* world = new list<WorldBlock>();
	// Will contain the player and Jelly, FILO
	list<Entity>* entities = new list<Entity>();
	// For now only holds coins but is made to be rather dynamic 
	list<WorldEntity>* worldEntities = new list<WorldEntity>();
	// For now only holds text also made dynamic
	list<WorldScenery>* worldScenery = new list<WorldScenery>();

	// Audio samples
	ALLEGRO_SAMPLE* audio_click001 = NULL;
	ALLEGRO_SAMPLE* audio_click002 = NULL;
	ALLEGRO_SAMPLE* audio_bounce = NULL;
	ALLEGRO_SAMPLE* audio_coin = NULL;
	ALLEGRO_SAMPLE* audio_land = NULL;
	ALLEGRO_SAMPLE* audio_shot_start = NULL;
	ALLEGRO_SAMPLE* audio_shot_end = NULL;
	ALLEGRO_SAMPLE* audio_drone = NULL;

	// Initialze Allegro and her pluggins
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

	EventQueue = al_create_event_queue();
	if (!EventQueue)
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
		al_destroy_event_queue(EventQueue);
		system("pause");
		return -1;
	}

	frame = al_create_timer(1.0 / FPS);
	if (!frame)
	{
		cout << "Failed to initiate frame \n";
		al_destroy_display(display);
		al_destroy_event_queue(EventQueue);
		system("pause");
		return -1;
	}

	al_init_font_addon();
	if (!al_init_ttf_addon()){
		fprintf(stderr, "failed to initialize ttf addon!\n");
		return -1;
	}

	Font = al_load_ttf_font("fonts/coolvetica.ttf", 17, 0);
	FontBig = al_load_ttf_font("fonts/coolvetica.ttf", 24, 0);

	if (!(Font && FontBig))
	{
		fprintf(stderr, "Failed to load font\n");
		system("pause");
		return -1;
	}

	if (!al_install_audio()){
		fprintf(stderr, "failed to initialize audio!\n");
		return -1;
	}

	if (!al_init_acodec_addon()){
		fprintf(stderr, "failed to initialize audio codecs!\n");
		return -1;
	}

	if (!al_reserve_samples(999)){
		fprintf(stderr, "failed to reserve samples!\n");
		return -1;
	}

	audio_click001 = al_load_sample("audio/click001.wav");
	audio_click002 = al_load_sample("audio/click002.wav");
	audio_bounce = al_load_sample("audio/bounce.wav");
	audio_coin = al_load_sample("audio/coin.wav");
	audio_land = al_load_sample("audio/land.wav");
	audio_shot_start = al_load_sample("audio/shot_start.wav");
	audio_shot_end = al_load_sample("audio/shot_end.wav");
	audio_drone = al_load_sample("audio/music2.wav");

	if (!(audio_click001 && audio_click002 && audio_bounce && audio_coin && audio_land && audio_shot_start && audio_shot_end && audio_drone))
	{
		printf("An audio clip did not load!\n");
		return -1;
	}

	// Program wide set color variables
	PlayerColor = al_map_rgb(20, 20, 220);
	JellyColor = al_map_rgb(185, 44, 136);
	WorldColor = al_map_rgb(62, 52, 25);
	BadWorldColor = al_map_rgb(220, 20, 20);
	JellyWorldColor = al_map_rgb(85, 169, 81);
	CoinColor = al_map_rgb(255, 140, 0);

	// The first item in the list is always the player
	entities->push_back(Entity(Spawn.X, Spawn.Y, 20, 20, 0.0, 0.0, PlayerColor, PLAYER));
	Entity* player = &(*entities->begin());
	
	// The following will make up the map, it is made in OGMO.
	world->push_back(WorldBlock(0, 0, 5630, 10, WorldColor, WORLD));
	world->push_back(WorldBlock(660, 0, 10, 600, al_map_rgba(0,0,0,0), WORLD));
	world->push_back(WorldBlock(5630, 0, 10, 480, WorldColor, WORLD));
	world->push_back(WorldBlock(0, 500, 1930, 100, WorldColor, WORLD));
	world->push_back(WorldBlock(1970, 500, 380, 100, WorldColor, WORLD));
	world->push_back(WorldBlock(1010, 480, 20, 20, WorldColor, WORLD));
	world->push_back(WorldBlock(2290, 480, 20, 20, WorldColor, WORLD));
	world->push_back(WorldBlock(2490, 500, 3790, 100, WorldColor, WORLD));
	world->push_back(WorldBlock(2910, 380, 190, 120, WorldColor, WORLD));
	world->push_back(WorldBlock(2800, 340, 20, 110, WorldColor, WORLD));
	world->push_back(WorldBlock(3360, 380, 130, 120, WorldColor, WORLD));
	world->push_back(WorldBlock(4010, 470, 40, 10, WorldColor, WORLD));
	world->push_back(WorldBlock(4070, 460, 40, 20, WorldColor, WORLD));
	world->push_back(WorldBlock(4130, 450, 40, 30, WorldColor, WORLD));
	world->push_back(WorldBlock(4190, 440, 40, 40, WorldColor, WORLD));
	world->push_back(WorldBlock(4250, 430, 40, 50, WorldColor, WORLD));
	world->push_back(WorldBlock(4310, 420, 40, 60, WorldColor, WORLD));
	world->push_back(WorldBlock(4370, 410, 40, 70, WorldColor, WORLD));
	world->push_back(WorldBlock(4430, 400, 40, 80, WorldColor, WORLD));
	world->push_back(WorldBlock(4490, 390, 40, 90, WorldColor, WORLD));
	world->push_back(WorldBlock(4550, 380, 40, 100, WorldColor, WORLD));
	world->push_back(WorldBlock(4610, 370, 40, 110, WorldColor, WORLD));
	world->push_back(WorldBlock(4670, 360, 40, 120, WorldColor, WORLD));
	world->push_back(WorldBlock(4730, 350, 40, 130, WorldColor, WORLD));
	world->push_back(WorldBlock(4790, 340, 40, 140, WorldColor, WORLD));
	world->push_back(WorldBlock(4850, 330, 40, 150, WorldColor, WORLD));
	world->push_back(WorldBlock(4910, 320, 40, 160, WorldColor, WORLD));
	world->push_back(WorldBlock(4970, 310, 40, 170, WorldColor, WORLD));
	world->push_back(WorldBlock(2280, 490, 10, 10, WorldColor, WORLD));
	world->push_back(WorldBlock(3100, 400, 10, 100, WorldColor, WORLD));
	world->push_back(WorldBlock(3110, 420, 10, 80, WorldColor, WORLD));
	world->push_back(WorldBlock(3120, 440, 10, 60, WorldColor, WORLD));
	world->push_back(WorldBlock(3130, 460, 10, 40, WorldColor, WORLD));
	world->push_back(WorldBlock(3140, 480, 10, 20, WorldColor, WORLD));
	world->push_back(WorldBlock(2400, 460, 10, 50, WorldColor, WORLD));
	world->push_back(WorldBlock(2400, 320, 10, 100, WorldColor, WORLD));
	world->push_back(WorldBlock(5290, 130, 20, 320, WorldColor, WORLD));
	world->push_back(WorldBlock(5400, 130, 20, 320, WorldColor, WORLD));
	world->push_back(WorldBlock(5420, 130, 110, 20, WorldColor, WORLD));
	world->push_back(WorldBlock(5510, 80, 20, 50, WorldColor, WORLD));
	world->push_back(WorldBlock(1370, 410, 20, 90, WorldColor, WORLD));
	world->push_back(WorldBlock(1550, 490, 10, 10, WorldColor, WORLD));
	world->push_back(WorldBlock(1560, 480, 20, 20, WorldColor, WORLD));
	world->push_back(WorldBlock(2380, 410, 50, 10, WorldColor, WORLD));

	world->push_back(WorldBlock(2400, 460, 10, 10, BadWorldColor, BADWORLD));
	world->push_back(WorldBlock(2350, 590, 140, 10, BadWorldColor, BADWORLD));
	world->push_back(WorldBlock(1930, 590, 40, 10, BadWorldColor, BADWORLD));

	world->push_back(WorldBlock(2300, 490, 10, 10, JellyWorldColor, JELLYWORLD));
	world->push_back(WorldBlock(2910, 480, 10, 20, JellyWorldColor, JELLYWORLD));
	world->push_back(WorldBlock(2810, 430, 10, 20, JellyWorldColor, JELLYWORLD));
	world->push_back(WorldBlock(2910, 380, 10, 20, JellyWorldColor, JELLYWORLD));
	world->push_back(WorldBlock(2810, 340, 10, 20, JellyWorldColor, JELLYWORLD));
	world->push_back(WorldBlock(3150, 500, 210, 10, JellyWorldColor, JELLYWORLD));
	world->push_back(WorldBlock(3930, 500, 60, 10, JellyWorldColor, JELLYWORLD));
	world->push_back(WorldBlock(4190, 440, 40, 10, JellyWorldColor, JELLYWORLD));
	world->push_back(WorldBlock(4430, 400, 40, 10, JellyWorldColor, JELLYWORLD));
	world->push_back(WorldBlock(4670, 360, 40, 10, JellyWorldColor, JELLYWORLD));
	world->push_back(WorldBlock(4970, 310, 40, 10, JellyWorldColor, JELLYWORLD));
	world->push_back(WorldBlock(4010, 470, 40, 10, JellyWorldColor, JELLYWORLD));
	world->push_back(WorldBlock(4070, 460, 40, 10, JellyWorldColor, JELLYWORLD));
	world->push_back(WorldBlock(4130, 450, 40, 10, JellyWorldColor, JELLYWORLD));
	world->push_back(WorldBlock(4250, 430, 40, 10, JellyWorldColor, JELLYWORLD));
	world->push_back(WorldBlock(4310, 420, 40, 10, JellyWorldColor, JELLYWORLD));
	world->push_back(WorldBlock(4370, 410, 40, 10, JellyWorldColor, JELLYWORLD));
	world->push_back(WorldBlock(4490, 390, 40, 10, JellyWorldColor, JELLYWORLD));
	world->push_back(WorldBlock(4550, 380, 40, 10, JellyWorldColor, JELLYWORLD));
	world->push_back(WorldBlock(4610, 370, 40, 10, JellyWorldColor, JELLYWORLD));
	world->push_back(WorldBlock(4730, 350, 40, 10, JellyWorldColor, JELLYWORLD));
	world->push_back(WorldBlock(4790, 340, 40, 10, JellyWorldColor, JELLYWORLD));
	world->push_back(WorldBlock(4850, 330, 40, 10, JellyWorldColor, JELLYWORLD));
	world->push_back(WorldBlock(4910, 320, 40, 10, JellyWorldColor, JELLYWORLD));
	world->push_back(WorldBlock(5300, 430, 10, 20, JellyWorldColor, JELLYWORLD));
	world->push_back(WorldBlock(5300, 330, 10, 30, JellyWorldColor, JELLYWORLD));
	world->push_back(WorldBlock(5300, 230, 10, 30, JellyWorldColor, JELLYWORLD));
	world->push_back(WorldBlock(5300, 130, 10, 30, JellyWorldColor, JELLYWORLD));
	world->push_back(WorldBlock(5400, 380, 10, 30, JellyWorldColor, JELLYWORLD));
	world->push_back(WorldBlock(5400, 280, 10, 30, JellyWorldColor, JELLYWORLD));
	world->push_back(WorldBlock(5400, 180, 10, 30, JellyWorldColor, JELLYWORLD));
	world->push_back(WorldBlock(5310, 500, 90, 10, JellyWorldColor, JELLYWORLD));
	world->push_back(WorldBlock(3490, 430, 10, 10, JellyWorldColor, JELLYWORLD));
	world->push_back(WorldBlock(3490, 450, 10, 10, JellyWorldColor, JELLYWORLD));
	world->push_back(WorldBlock(3490, 470, 10, 10, JellyWorldColor, JELLYWORLD));
	world->push_back(WorldBlock(3490, 490, 10, 10, JellyWorldColor, JELLYWORLD));
	world->push_back(WorldBlock(3510, 490, 10, 10, JellyWorldColor, JELLYWORLD));
	world->push_back(WorldBlock(3530, 490, 10, 10, JellyWorldColor, JELLYWORLD));
	world->push_back(WorldBlock(3550, 490, 10, 10, JellyWorldColor, JELLYWORLD));
	world->push_back(WorldBlock(5640, 0, 640, 10, JellyWorldColor, JELLYWORLD));
	world->push_back(WorldBlock(5640, 0, 10, 480, JellyWorldColor, JELLYWORLD));
	world->push_back(WorldBlock(5640, 500, 640, 10, JellyWorldColor, JELLYWORLD));
	world->push_back(WorldBlock(6270, 0, 10, 510, JellyWorldColor, JELLYWORLD));
	world->push_back(WorldBlock(5860, 250, 200, 10, JellyWorldColor, JELLYWORLD));
	world->push_back(WorldBlock(5650, 40, 60, 10, JellyWorldColor, JELLYWORLD));
	world->push_back(WorldBlock(5650, 150, 60, 10, JellyWorldColor, JELLYWORLD));
	world->push_back(WorldBlock(5650, 290, 60, 10, JellyWorldColor, JELLYWORLD));
	world->push_back(WorldBlock(5650, 380, 60, 10, JellyWorldColor, JELLYWORLD));
	world->push_back(WorldBlock(5650, 220, 60, 10, JellyWorldColor, JELLYWORLD));
	world->push_back(WorldBlock(5650, 90, 60, 10, JellyWorldColor, JELLYWORLD));
	world->push_back(WorldBlock(1350, 500, 20, 10, JellyWorldColor, JELLYWORLD));
	world->push_back(WorldBlock(1390, 500, 20, 10, JellyWorldColor, JELLYWORLD));
	world->push_back(WorldBlock(1570, 490, 10, 10, JellyWorldColor, JELLYWORLD));
	world->push_back(WorldBlock(6200, 480, 5, 20, JellyWorldColor, JELLYWORLD));
	world->push_back(WorldBlock(6210, 480, 5, 20, JellyWorldColor, JELLYWORLD));
	world->push_back(WorldBlock(6220, 480, 5, 20, JellyWorldColor, JELLYWORLD));
	world->push_back(WorldBlock(6230, 480, 5, 20, JellyWorldColor, JELLYWORLD));
	world->push_back(WorldBlock(6240, 480, 5, 20, JellyWorldColor, JELLYWORLD));
	world->push_back(WorldBlock(6250, 480, 5, 20, JellyWorldColor, JELLYWORLD));
	world->push_back(WorldBlock(6260, 480, 5, 20, JellyWorldColor, JELLYWORLD));
	world->push_back(WorldBlock(6250, 470, 20, 5, JellyWorldColor, JELLYWORLD));
	world->push_back(WorldBlock(6250, 460, 20, 5, JellyWorldColor, JELLYWORLD));
	world->push_back(WorldBlock(6250, 450, 20, 5, JellyWorldColor, JELLYWORLD));
	world->push_back(WorldBlock(6250, 440, 20, 5, JellyWorldColor, JELLYWORLD));
	world->push_back(WorldBlock(6250, 430, 20, 5, JellyWorldColor, JELLYWORLD));

	worldEntities->push_back(WorldEntity(1020, 470, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(2410, 440, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(2420, 440, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(2430, 440, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(2830, 440, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(2900, 390, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(2830, 350, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(2900, 490, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(4030, 460, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(4150, 440, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(4270, 420, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(4390, 400, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(4510, 380, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(4630, 360, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(4750, 340, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(4870, 320, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(4990, 300, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(4990, 210, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(3950, 210, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(3940, 220, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(3950, 220, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(3940, 210, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(3960, 210, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(3960, 220, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(3960, 230, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(3950, 230, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(3940, 230, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(3940, 240, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(3950, 240, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(3960, 240, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(3970, 230, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(3970, 240, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(3970, 220, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(3960, 210, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(3970, 210, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(3980, 210, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(3980, 220, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(3980, 230, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(3980, 240, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(3940, 250, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(3950, 250, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(3960, 250, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(3970, 250, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(3980, 250, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(3980, 250, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(4090, 450, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(4210, 430, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(4330, 410, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(4450, 390, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(4570, 370, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(4690, 350, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(4810, 330, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(4930, 310, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(5320, 440, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(5394, 394, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(5316, 344, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(5394, 294, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(5316, 244, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(5394, 194, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(5316, 144, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(5420, 100, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(5420, 90, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(5430, 90, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(5430, 100, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(5420, 110, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(5420, 120, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(5430, 120, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(5430, 110, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(5440, 120, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(5440, 100, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(5440, 110, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(5440, 90, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(5450, 100, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(5450, 90, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(5460, 90, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(5460, 100, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(5450, 110, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(5450, 120, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(5460, 120, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(5460, 110, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(5470, 120, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(5470, 100, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(5470, 110, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(5470, 90, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(5480, 100, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(5480, 90, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(5490, 90, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(5490, 100, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(5480, 110, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(5480, 120, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(5490, 120, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(5490, 110, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(5500, 120, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(5500, 100, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(5500, 110, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(5500, 90, 5, 5, CoinColor, COIN));

	TotalCoins = worldEntities->size();
	
	// Scenery may have multiple forms of data
	// Here they take a void pointer to a struct which is later on dereferenced based on its ItemType
	WorldText welcome;
	welcome.font = Font;
	welcome.text = "The fastest to collect all the coins will get the highscore and a highfive!";
	welcome.color = al_map_rgb(100, 100, 100);
	worldScenery->push_back(WorldScenery(2000, 250, SCENETEXT, (void*)&welcome));

	WorldText controls1;
	controls1.font = Font;
	controls1.color = al_map_rgb(100, 100, 100);
	controls1.text = "Movement with WASD and arrow keys";
	worldScenery->push_back(WorldScenery(400, 400, SCENETEXT, (void*)&controls1));
	
	WorldText controls2;
	controls2.font = Font;
	controls2.color = al_map_rgb(100, 100, 100);
	controls2.text = "Jump with W, up or space";
	worldScenery->push_back(WorldScenery(800, 400, SCENETEXT, (void*)&controls2));
	
	WorldText controls3;
	controls3.font = Font;
	controls3.color = al_map_rgb(100, 100, 100);
	controls3.text = "Use Mouse1 to shoot and use Mouse2 to remove all Jelly";
	worldScenery->push_back(WorldScenery(1050, 375, SCENETEXT, (void*)&controls3));

	WorldText controls6;
	controls6.font = Font;
	controls6.color = al_map_rgb(100, 100, 100);
	controls6.text = "Hold shift to draw a projectile prediction";
	worldScenery->push_back(WorldScenery(1050, 400, SCENETEXT, (void*)&controls6));

	WorldText controls7;
	controls7.font = Font;
	controls7.color = al_map_rgb(100, 100, 100);
	controls7.text = "Shoot the green and see what happens";
	worldScenery->push_back(WorldScenery(1050, 425, SCENETEXT, (void*)&controls7));

	WorldText controls4;
	controls4.font = Font;
	controls4.color = al_map_rgb(100, 100, 100);
	controls4.text = "Press R to go back to spawn";
	worldScenery->push_back(WorldScenery(80, 400, SCENETEXT, (void*)&controls4));

	WorldText controls5;
	controls5.font = Font;
	controls5.color = al_map_rgb(100, 100, 100);
	controls5.text = "To stop bouncing hold CTRL";
	worldScenery->push_back(WorldScenery(1450, 400, SCENETEXT, (void*)&controls5));


	WorldText controls8;
	controls8.font = Font;
	controls8.color = al_map_rgb(100, 100, 100);
	controls8.text = "You can also hit jely from the side";
	worldScenery->push_back(WorldScenery(1700, 400, SCENETEXT, (void*)&controls8));

	WorldText credits;
	credits.font = Font;
	credits.color = al_map_rgb(100, 100, 100);
	credits.text = "All audio by Aljoscha Christenhusz";
	worldScenery->push_back(WorldScenery(6360, 220, SCENETEXT, (void*)&credits));

	WorldText credits2;
	credits2.font = Font;
	credits2.color = al_map_rgb(100, 100, 100);
	credits2.text = "Programming and other stuff by Benjamin Waanders";
	worldScenery->push_back(WorldScenery(6360, 260, SCENETEXT, (void*)&credits2));

	// Registering events
	al_init_user_event_source(&UserEventSource);
	al_register_event_source(EventQueue, &UserEventSource);
	al_register_event_source(EventQueue, al_get_timer_event_source(frame));
	al_register_event_source(EventQueue, al_get_display_event_source(display));
	al_register_event_source(EventQueue, al_get_keyboard_event_source());
	al_register_event_source(EventQueue, al_get_mouse_event_source());

	// Struct to send to the Physics thread
	PhysicsVariables physVar;
	physVar.entities = entities;
	physVar.world = world;
	physVar.worldEntities = worldEntities;

	// Starting the threads
	thread physThread = thread(AsyncPhysics, (void*)&physVar);
	thread consoleThread = thread(DevConsole);

	// Grabbing the mouse, this is broken rather easy though
	al_grab_mouse(display);

	// Start timer for FPS
	al_start_timer(frame);

	// Start soundtrack
	al_play_sample(audio_drone, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_LOOP, NULL);

	while (!Quit)
	{
		ALLEGRO_EVENT e;
		al_wait_for_event(EventQueue, &e);

		if (e.type == ALLEGRO_EVENT_TIMER)
		{
			// Score is based on time spend collection the coins
			// So the lower the score the better
			// Every frame adds on point;
			if (!Collected)
				++TimeScore;
			if (Score == TotalCoins)
			{
				// All coins have been collected
				Collected = true;

				// Get the previouse highscore
				std::ifstream scoreI;
				scoreI.open("score.txt");
				getline(scoreI, HighScore);
				scoreI.close();

				if (HighScore == "")
				{
					// If there is no previous highscore the current score will be the highscore
					ofstream scoreO;
					scoreO.open("score.txt");
					scoreO << to_string(TimeScore) << endl;
					HighScore = to_string(TimeScore);
					scoreO.close();
				}
				else if (TimeScore < stoi(HighScore))
				{
					// If the new score is better it will be the new highscore
					ofstream scoreO;
					scoreO.open("score.txt");
					scoreO << to_string(TimeScore) << endl;
					scoreO.close();
				}
			}

			Draw(entities, world, worldEntities, worldScenery);
		}
		else if (e.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN)
		{
			switch (e.mouse.button)
			{
			case 1:
				Shoot(entities, &e);
				break;
			case 2:
				// Mtx is used as a r/w lock so that only one thread can r or w at a time
				Mtx.lock();
				// Run until only the player is left
				while (entities->size() > 1)
				{
					entities->pop_back();
				}
				Mtx.unlock();
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
				// Keys is used to keep a keys state
				Keys[RIGHT] = true;
				break;

			case ALLEGRO_KEY_A:
			case ALLEGRO_KEY_LEFT:
				Keys[LEFT] = true;
				break;

			case ALLEGRO_KEY_SPACE:
			case ALLEGRO_KEY_W:
			case ALLEGRO_KEY_UP:
				Keys[UP] = true;
				break;

			case ALLEGRO_KEY_S:
			case ALLEGRO_KEY_DOWN:
				Keys[DOWN] = true;
				break;

			case ALLEGRO_KEY_LCTRL:
			case ALLEGRO_KEY_RCTRL:
				Keys[LCTRL] = true;
				break;

			case ALLEGRO_KEY_ESCAPE:
				Quit = true;
				break;

			case ALLEGRO_KEY_R:
				player->SetCoordinates(Spawn.X, Spawn.Y);
				break;

			case ALLEGRO_KEY_T:
				// To clear the screen if CLEAR_DRAW == false
				al_clear_to_color(al_map_rgb(103, 233, 246));
				break;
			
			case ALLEGRO_KEY_LSHIFT:
			case ALLEGRO_KEY_RSHIFT:
				DRAW_PREDICTION = true;
				break;

			}
		}
		else if (e.type == ALLEGRO_EVENT_KEY_UP)
		{
			switch (e.keyboard.keycode)
			{
			case ALLEGRO_KEY_D:
			case ALLEGRO_KEY_RIGHT:
				Keys[RIGHT] = false;
				break;

			case ALLEGRO_KEY_SPACE:
			case ALLEGRO_KEY_W:
			case ALLEGRO_KEY_UP:
				Keys[UP] = false;
				break;

			case ALLEGRO_KEY_A:
			case ALLEGRO_KEY_LEFT:
				Keys[LEFT] = false;
				break;

			case ALLEGRO_KEY_S:
			case ALLEGRO_KEY_DOWN:
				Keys[DOWN] = false;
				break;

			case ALLEGRO_KEY_LCTRL:
			case ALLEGRO_KEY_RCTRL:
				Keys[LCTRL] = false;
				break;

			case ALLEGRO_KEY_LSHIFT:
			case ALLEGRO_KEY_RSHIFT:
				DRAW_PREDICTION = false;
				break;
			}
		}
		else if (e.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
		{
			Quit = true;
		}
		else if (e.type == 555) // User made event for sounds in this case
		{
			switch (e.user.data1)
			{
			case PLAYER_WORLD:
				al_play_sample(audio_land, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
				break;

			case PLAYER_JELLY:
				al_play_sample(audio_bounce, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
				break;

			case PLAYER_BADWORLD:
				al_play_sample(audio_click002, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
				break;

			case JELLY_JELLY:
				al_play_sample(audio_bounce, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
				break;

			case JELLY_JELLYWORLD:
				al_play_sample(audio_shot_end, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
				break;

			case JELLY_NOTJELLYWORLD:
				al_play_sample(audio_click001, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
				break;

			case PLAYER_COIN:
				al_play_sample(audio_coin, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
				break;

			case PLAYER_SHOOT:
				al_play_sample(audio_shot_start, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
				break;
			default:
				break;
			}
		}
	}

	// Wait for other threads to finish 
	al_rest(0.1);

	physThread.detach();
	consoleThread.detach();

	delete world, entities, worldEntities, worldScenery;

	return 0;
}

void Draw(list<Entity>* entities, list<WorldBlock>* world, list<WorldEntity>* worldEntities, list<WorldScenery>* worldScenery)
{
	if (CLEAR_DRAW) al_clear_to_color(al_map_rgb(103, 233, 246));

	Entity* player = &*entities->begin();
	ALLEGRO_MOUSE_STATE mouse;
	al_get_mouse_state(&mouse);
	Coordinates plyA = *player->GetACoordinates();
	Coordinates plyB = *player->GetBCoordinates();
	// Origin to shoot from
	double originX = plyA.X + player->GetWidth() / 2 - PROJECTILE_SIZE / 2;
	double originY = plyA.Y + player->GetHeight() / 2 - PROJECTILE_SIZE / 2;
	// Angle used to draw the prediction and to draw the arm/gun thing
	// Angle gets calculated with based on the offset between the player and the mouse using triginometry
	double angle = Phys.OffsetToAngle((player->GetWidth() / 2 - PROJECTILE_SIZE / 2 - mouse.x + (SCREEN_W / 2 - player->GetWidth() / 2)) * -1, (originY - mouse.y + PROJECTILE_SIZE / 2) * -1);
	Coordinates* gunVec = Phys.VectorToOffset(15.0, angle);
	Coordinates* blankGunVec = Phys.VectorToOffset(5.0, angle);
	Coordinates* entOff = player->GetOffset();
	Coordinates* shotVec = Phys.VectorToOffset(PROJECTILE_SPEED, angle);
	// Viewport used to calculate whenever an object should be drawn
	Coordinates viewPortA = Coordinates(plyA.X - (SCREEN_W / 2 + player->GetWidth() / 2), 0.0);
	Coordinates viewPortB = Coordinates(plyA.X + (SCREEN_W / 2 + player->GetWidth() / 2), SCREEN_H);

	for (list<WorldScenery>::iterator wScene = worldScenery->begin(); wScene != worldScenery->end(); ++wScene)
	{
		if (wScene->GetType() == SCENETEXT)
		{
			Coordinates wSceneBCo = Coordinates(wScene->GetCoordinates()->X + 600, wScene->GetCoordinates()->Y + 20);

			if (Phys.AreColliding(&viewPortA, &viewPortB, wScene->GetCoordinates(), &wSceneBCo)) // Only draw if the WorldScene object is within the viewport
			{
				// Cast the void to WorldText 
				WorldText* wData = (WorldText*)wScene->GetData();
				al_draw_text(wData->font, wData->color, wScene->GetCoordinates()->X + (SCREEN_W / 2 - player->GetWidth() / 2) + plyA.X * -1, wScene->GetCoordinates()->Y, 0, (wData->text).c_str());
			}
		}
	}

	Mtx.lock();
	if (DRAW_PREDICTION && CLEAR_DRAW)
	{
		// Ghost entity for the projectile path simulation
		Entity ghostShot = Entity(originX, originY, PROJECTILE_SIZE, PROJECTILE_SIZE, shotVec->X + entOff->X, shotVec->Y + entOff->Y, al_map_rgb(180, 180, 180), PROJECTILE);

		Coordinates last = Coordinates(-1, -1);

		// Run no longer than MAX_COLLISIONS_PREDICTION this variable is in tics
		for (int i = 0; i < MAX_COLLISION_PREDICTION; ++i)
		{
			Phys.ApplyGravity(&ghostShot);
			Phys.Collide(&ghostShot, world, entities, worldEntities, false);
			ghostShot.MoveToOffset();

			// If the ghostshot doesn't move it means it hit something or of it hit something that would remove it
			if (last.X == ghostShot.GetACoordinates()->X && last.Y == ghostShot.GetACoordinates()->Y || ghostShot.GetRemove()) 
			{
				break;
			}
			else
			{
				// Only draw if it's in range of the viewport
				if (Phys.AreColliding(&viewPortA, &viewPortB, ghostShot.GetACoordinates(), ghostShot.GetBCoordinates()))
				{
					// Included the offset used for the side scrolling
					al_draw_filled_rectangle(
						// Offset everything by half of the screen minus half of the player then substract the player position
						ghostShot.GetACoordinates()->X + (SCREEN_W / 2 - player->GetWidth() / 2) - plyA.X,
						ghostShot.GetACoordinates()->Y,
						ghostShot.GetBCoordinates()->X + (SCREEN_W / 2 - player->GetWidth() / 2) - plyA.X,
						ghostShot.GetBCoordinates()->Y,
						ghostShot.GetColor());
				}
				last.X = ghostShot.GetACoordinates()->X;
				last.Y = ghostShot.GetACoordinates()->Y;
			}
		}
	}
	Mtx.unlock();

	// Draw worldblocks
	for (list<WorldBlock>::iterator wBlock = world->begin(); wBlock != world->end(); ++wBlock)
	{
		if (Phys.AreColliding(&viewPortA, &viewPortB, wBlock->GetA(), wBlock->GetB()))
		{
			al_draw_filled_rectangle(
				wBlock->GetA()->X + (SCREEN_W / 2 - player->GetWidth() / 2) - plyA.X,
				wBlock->GetA()->Y,
				wBlock->GetB()->X + (SCREEN_W / 2 - player->GetWidth() / 2) - plyA.X,
				wBlock->GetB()->Y,
				wBlock->GetColor());
		}
	}

	// Draw all entities except the player
	Mtx.lock();
	for (list<Entity>::iterator ent = ++entities->begin(); ent != entities->end(); ++ent)
	{
		Coordinates* entA = ent->GetACoordinates();
		Coordinates* entB = ent->GetBCoordinates();
		
		if (Phys.AreColliding(&viewPortA, &viewPortB, entA, entB))
		{
			al_draw_filled_rectangle(
				entA->X + (SCREEN_W / 2 - player->GetWidth() / 2) - plyA.X,
				entA->Y,
				entB->X + (SCREEN_W / 2 - player->GetWidth() / 2) - plyA.X,
				entB->Y,
				ent->GetColor());

			// Draw a trail behind the projectile as long as it's flying
			if (!ent->GetHit())
			{
				double lastY = 0;

				// Length based on PROJECTILE_TRAIL_LENGTH
				for (int i = 1; i < PROJECTILE_TRAIL_LENGTH + 1; ++i)
				{
					al_draw_filled_rectangle(
						// Set X offset per step
						entA->X - ent->GetOffset()->X * i + (SCREEN_W / 2 - player->GetWidth() / 2) + plyA.X * -1,
						// Calculate the gravity step with the offset per step
						entA->Y - (lastY + (ent->GetOffset()->Y - (GRAVITY * i))),
						entB->X - ent->GetOffset()->X * i + (SCREEN_W / 2 - player->GetWidth() / 2) + plyA.X * -1,
						entB->Y - (lastY + (ent->GetOffset()->Y - (GRAVITY * i))),
						ent->GetColor());

					// Calculate the previous height
					lastY += (ent->GetOffset()->Y - (GRAVITY * i));
				}
			}
		}
	}

	for (list<WorldEntity>::iterator wEntity = worldEntities->begin(); wEntity != worldEntities->end(); ++wEntity)
	{
		if (Phys.AreColliding(&viewPortA, &viewPortB, wEntity->GetA(), wEntity->GetB()))
		{
			al_draw_filled_rectangle(
				wEntity->GetA()->X + (SCREEN_W / 2 - player->GetWidth() / 2) + plyA.X * -1,
				wEntity->GetA()->Y,
				wEntity->GetB()->X + (SCREEN_W / 2 - player->GetWidth() / 2) + plyA.X * -1,
				wEntity->GetB()->Y,
				wEntity->GetColor());
		}
	}
	Mtx.unlock();

	// Draw the player in the center of the screen
	al_draw_filled_rectangle(
		SCREEN_W / 2 - player->GetWidth() / 2,
		plyA.Y,
		SCREEN_W / 2 - player->GetWidth() / 2 + player->GetWidth(),
		plyB.Y,
		player->GetColor());

	// Draw the gun of the player, a line based on gunVec and blankGunVec
	al_draw_line((SCREEN_W / 2), originY + PROJECTILE_SIZE / 2, gunVec->X +      SCREEN_W / 2, gunVec->Y      + originY + PROJECTILE_SIZE / 2, BadWorldColor, 5.0);
	al_draw_line((SCREEN_W / 2), originY + PROJECTILE_SIZE / 2, blankGunVec->X + SCREEN_W / 2, blankGunVec->Y + originY + PROJECTILE_SIZE / 2, al_map_rgb(100, 100, 255), 5.0);

	// Visual represntation above the player to show the current amount of Jelly and their how much lifetime they got left.
	Mtx.lock();
	struct ts { int i; list<Entity>::iterator ent; };
	for (ts t = { 0, ++entities->begin() }; t.ent != entities->end(); ++t.ent, ++t.i)
	{
		al_draw_filled_rectangle(
			SCREEN_W / 2 - player->GetWidth() / 2 + t.i * 7,
			plyA.Y - 7,
			SCREEN_W / 2 - player->GetWidth() / 2 + t.i * 7 + 5,
			plyA.Y - 5, JellyColor);

		al_draw_filled_rectangle(
			SCREEN_W / 2 - player->GetWidth() / 2 + t.i * 7,
			plyA.Y - 7,
			SCREEN_W / 2 - player->GetWidth() / 2 + t.i * 7 + (t.ent->GetAge() / MAX_ENTITY_AGE * 5),
			plyA.Y - 5, al_map_rgb(20, 20, 220));
	}
	Mtx.unlock();

	// Draw the score and time
	al_draw_text(Font, WorldColor, 1000, 10, 0, ("Coins: " + to_string(Score) + "/" + to_string(TotalCoins)).c_str());
	al_draw_text(Font, WorldColor, 1000, 25, 0, ("Time: " + to_string(TimeScore)).c_str());

	// If the player has finnished the game
	if (Collected && ScoreShow != 0)
	{
		// Every tick this gets reduced so that it has a lifetime on the screen until it's 0
		--ScoreShow;

		// Player has the highsore
		if (TimeScore == stoi(HighScore))
		{
			al_draw_text(FontBig, WorldColor, 400, 200, 0, "New Highscore!");
			al_draw_text(Font, al_map_rgb(40, 40, 40), 400, 240, 0, ("Your score: " + to_string(TimeScore)).c_str());
			al_draw_text(Font, al_map_rgb(40, 40, 40), 400, 265, 0, "Restart the game to start over or play around with the console and testing area");
		}
		else
		{
			al_draw_text(FontBig, WorldColor, 400, 200, 0, "You finished the game!");
			al_draw_text(Font, al_map_rgb(40, 40, 40), 400, 240, 0, ("Your score: " + to_string(TimeScore)).c_str());
			al_draw_text(Font, al_map_rgb(40, 40, 40), 400, 265, 0, ("Highscore: " + HighScore).c_str());
			al_draw_text(Font, al_map_rgb(40, 40, 40), 400, 290, 0, "Restart the game to start over or play around with the console and testing area");

		}
	}

	al_flip_display();
}

void Move(Entity* ent)
{
	Coordinates* offset = ent->GetOffset();

	// bool FLY can be set in console
	if (!FLY)
	{
		if (Keys[RIGHT])
		{
			// If the player boucned of a side Jelly he may not control the player until it lands on anything else then jelly sideways
			if (!(ent->GetLastImpactType() == JELLY && (ent->GetLastColPos() == RX || ent->GetLastColPos() == LX)))
			{
				// If the player is moving in the oppisite direction
				if (offset->X < 0.0 && ent->GetIsAirBorn())
				{
					// Apply break and stop of the motion gets within the range PLAYER_AIR_CONTROL_STOP
					offset->X = offset->X > -PLAYER_AIR_CONTROL_STOP ? 0.0 : offset->X * PLAYER_AIR_CONTROL_BREAK;
				}
				else
				{
					offset->X += ent->GetIsAirBorn() ? PLAYER_SPEED * PLAYER_AIR_CONTROL : PLAYER_SPEED;
				}
			}
		}

		if (Keys[LEFT]) 
		{
			if (!(ent->GetLastImpactType() == JELLY && (ent->GetLastColPos() == RX || ent->GetLastColPos() == LX)))
			{
				if (offset->X > 0.0 && ent->GetIsAirBorn())
				{
					offset->X = offset->X < PLAYER_AIR_CONTROL_STOP ? 0.0 : offset->X * PLAYER_AIR_CONTROL_BREAK;
				}
				else
				{
					offset->X -= ent->GetIsAirBorn() ? PLAYER_SPEED * PLAYER_AIR_CONTROL : PLAYER_SPEED;
				}
			}
		}

		if (Keys[UP])
		{
			// Only jump if the player is standing 
			if (!ent->GetIsAirBorn())
			{
				offset->Y -= PLAYER_JUMP_SPEED;
			}
		}
	}
	else
	{
		// Flying mode
		if (Keys[UP])
			--offset->Y;
		if (Keys[DOWN])
			++offset->Y;
		if (Keys[RIGHT])
			++offset->X;
		if (Keys[LEFT])
			--offset->X;
	}

	if (Keys[LCTRL])
	{
		// Crouching stops the bouncing
		ent->SetIsCrouching(true);
	}
	else
	{
		ent->SetIsCrouching(false);
	}
}

// TODO : Check nececerity of convertions
// TODO : Mang, dat copy of event
void Shoot(list<Entity>* entities, ALLEGRO_EVENT* e)
{
	if (entities->size() - 1 == MAX_ENTITIES)
	{
		// If the total amount of jelly is at maximum then remove the first one that was shot
		Mtx.lock();
		entities->pop_back();
		Mtx.unlock();
	}


	// Same algorithm as in Draw()
	Entity* player = &*entities->begin();
	Coordinates* plyA = player->GetACoordinates();
	Coordinates* plyOff = player->GetOffset();
	double originX = plyA->X + player->GetWidth() / 2 - PROJECTILE_SIZE / 2;
	double originY = plyA->Y + player->GetHeight() / 2 - PROJECTILE_SIZE / 2;
	Coordinates* shotOff = Phys.VectorToOffset(PROJECTILE_SPEED, Phys.OffsetToAngle((player->GetHeight() / 2 - PROJECTILE_SIZE / 2 - e->mouse.x + (SCREEN_W / 2 - player->GetWidth() / 2)) * -1, (originY - e->mouse.y + PROJECTILE_SIZE / 2) * -1));

	entities->insert(++entities->begin(), Entity(originX, originY, PROJECTILE_SIZE, PROJECTILE_SIZE, plyOff->X + shotOff->X, plyOff->Y + shotOff->Y, JellyColor, PROJECTILE));

	++Particles;

	delete shotOff;

	// Event to trigger that the player fired
	ALLEGRO_EVENT es;
	es.type = 555;
	es.user.data1 = PLAYER_SHOOT;
	al_emit_user_event(&UserEventSource, &es, 0);
}

void AsyncPhysics(void* struc)
{
	// Setting up local varibles casted from the void struct
	list<WorldBlock>* world = ((PhysicsVariables*)struc)->world;
	list<Entity>* entities = ((PhysicsVariables*)struc)->entities;
	list<WorldEntity>* worldEntities = ((PhysicsVariables*)struc)->worldEntities;
	Entity* player = &(*entities->begin());

	while (!Quit)
	{
		al_rest(1.0 / PHYSICS_TICK);
		Mtx.lock();
		// Update the playermovement bases on user input
		Move(player);
		// Run the physics such as gravity and set the players location in the next tic colliding or not
		Phys.ApplyPhysics(entities, world, worldEntities);
		Mtx.unlock();
	}
}

void DevConsole()
{
	string input = "";

	while (!Quit)
	{ 
		cout << ">";
		
		// Get user command
		getline(cin, input);

		// See if the command 
		int splitIndex = input.find(" ");
		string command = "";
		double value;

		if (splitIndex != -1) // Set Value
		{

			command = input.substr(0, splitIndex);

			// Throw not found the string after the space cannot be converted to a double
			if (splitIndex != input.size() - 1)
			{
				string valString = input.substr(++splitIndex, input.length() - 1);
				valString.erase(remove_if(valString.begin(), valString.end(), isalpha), valString.end());
				if (valString.size() > 0)
					value = stod(valString);
				else
					command = "";
			}
			else
			{
				command = "";
			}

			if (command == "GRAVITY")
			{
				GRAVITY = value / FPS;
				cout << "GRAVITY set to: " << GRAVITY * FPS << endl;
			}
			else if (command == "PHYSICS_TICK")
			{
				PHYSICS_TICK = value;
				cout << "PHYSICS_TICK set to: " << PHYSICS_TICK << endl;
			}
			else if (command == "PLAYER_SPEED")
			{
				PLAYER_SPEED = value;
				cout << "PLAYER_SPEED set to: " << PLAYER_SPEED << endl;
			}
			else if (command == "PLAYER_JUMP_SPEED")
			{
				PLAYER_JUMP_SPEED = value;
				cout << "PLAYER_JUMP_SPEED set to: " << PLAYER_JUMP_SPEED << endl;
			}
			else if (command == "PLAYER_BOUNCE_OFFSET")
			{
				PLAYER_BOUNCE_OFFSET = value;
				cout << "PLAYER_BOUNCE_OFFSET set to: " << PLAYER_BOUNCE_OFFSET << endl;
			}
			else if (command == "PLAYER_AIR_CONTROL")
			{
				PLAYER_AIR_CONTROL = value;
				cout << "PLAYER_AIR_CONTROL set to: " << PLAYER_AIR_CONTROL << endl;
			}
			else if (command == "PLAYER_AIR_CONTROL_BREAK")
			{
				PLAYER_AIR_CONTROL_BREAK = value;
				cout << "PLAYER_AIR_CONTROL_BREAK set to: " << PLAYER_AIR_CONTROL_BREAK << endl;
			}
			else if (command == "PLAYER_SIDE_UP_BOUNCE")
			{
				PLAYER_SIDE_UP_BOUNCE = value;
				cout << "PLAYER_SIDE_UP_BOUNCE set to: " << PLAYER_SIDE_UP_BOUNCE << endl;
			}
			else if (command == "PLAYER_SIDE_SIDE_BOUNCE")
			{
				PLAYER_SIDE_SIDE_BOUNCE = value;
				cout << "PLAYER_SIDE_SIDE_BOUNCE set to: " << PLAYER_SIDE_SIDE_BOUNCE << endl;
			}
			else if (command == "PROJECTILE_SPEED")
			{
				PROJECTILE_SPEED = value;
				cout << "PROJECTILE_SPEED set to: " << PROJECTILE_SPEED << endl;
			}
			else if (command == "PROJECTILE_SIZE")
			{
				PROJECTILE_SIZE = value;
				cout << "PROJECTILE_SIZE set to: " << PROJECTILE_SIZE << endl;
			}
			else if (command == "PROJECTILE_BOUNCINESS")
			{
				PROJECTILE_BOUNCINESS = value;
				cout << "PROJECTILE_BOUNCINESS set to: " << PROJECTILE_BOUNCINESS << endl;
			}
			else if (command == "MAX_ENTITIES")
			{
				MAX_ENTITIES = value;
				cout << "MAX_ENTITIES set to: " << MAX_ENTITIES << endl;
			}
			else if (command == "MAX_ENTITY_AGE")
			{
				MAX_ENTITY_AGE = value * PHYSICS_TICK;
				cout << "MAX_ENTITY_AGE set to: " << MAX_ENTITY_AGE / PHYSICS_TICK<< endl;
			}
			else if (command == "MAX_ENTITY_VELOCITY")
			{
				MAX_ENTITY_VELOCITY = value;
				cout << "MAX_ENTITY_VELOCITY set to: " << MAX_ENTITY_VELOCITY << endl;
			}
			else if (command == "FRICTION")
			{
				FRICTION = value;
				cout << "FRICTION set to: " << FRICTION << endl;
			}
			else if (command == "FRICTION_STOP")
			{
				FRICTION_STOP = value;
				cout << "FRICTION_STOP set to: " << FRICTION_STOP << endl;
			}
			else if (command == "PROJECTILE_TRAIL_LENGTH")
			{
				PROJECTILE_TRAIL_LENGTH = value;
				cout << "PROJECTILE_TRAIL_LENGTH set to: " << PROJECTILE_TRAIL_LENGTH << endl;
			}
			else
			{
				cout << "Command not found\n";
			}
		} 
		else // Get value
		{
			command = input;
			
			if (command == "help")
			{
				cout << "Availabe commands are: \n" << "PHYSICS_TICK\nGRAVITY\nPLAYER_SPEED\nPLAYER_JUMP_SPEED\nPLAYER_BOUNCE_OFFSET\nPLAYER_AIR_CONTROL\nPLAYER_AIR_CONTROL_BREAK;\nPLAYER_SIDE_UP_BOUNCE\nPLAYER_SIDE_SIDE_BOUNCE\nPROJECTILE_SPEED\nPROJECTILE_SIZE\nPROJECTILE_TRAIL_LENGTH\nPROJECTILE_BOUNCINESS\nMAX_ENTITIES\nMAX_ENTITY_AGE\nMAX_ENTITY_VELOCITY\nFRICTION\nFRICTION_STOP\nDRAW_PREDICTION\nCLEAR_DRAW\n";
			}
			else if (command == "GRAVITY")
			{
				cout << "GRAVITY is: " << GRAVITY * FPS << endl;
			}
			else if (command == "PROJECTILE_TRAIL_LENGTH")
			{
				cout << "PROJECTILE_TRAIL_LENGTH is: " << PROJECTILE_TRAIL_LENGTH << endl;
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
				cout << "MAX_ENTITY_AGE is: " << MAX_ENTITY_AGE / PHYSICS_TICK << endl;
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
			else if (command == "CLEAR_DRAW")
			{
				CLEAR_DRAW = !CLEAR_DRAW;

				cout << "CLEAR_DRAW is: " << (CLEAR_DRAW ? "enabled" : "disabled") << ".\n";
			}
			else if (command == "DRAW_PREDICTION")
			{
				DRAW_PREDICTION = !DRAW_PREDICTION;

				cout << "DRAW_PREDICTION is: " << (DRAW_PREDICTION ? "enabled" : "disabled") << ".\n";
			}
			else
			{
				cout << "Command not found\n";
			}
		}
	}
}