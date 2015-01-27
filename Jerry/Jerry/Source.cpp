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
// TODO : Preformance optimizing
// TODO : Physics tick counter
// TODO : Coins or something alike
// TODO : Checkpoints
// TODO : Level end
// TODO : Load new level
// TODO : FIX CORNER IMPACT MULTIPLE HEIGHTS
// TODO : CORNER IMPACT COIN

// TODO : Portfolio : Langton's ant, BiZZdesign werk, PVB?

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
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include "Entity.h"
#include "WorldBlock.h"
#include "Physics.h"
#include "WorldEntity.h"
#include "WorldScenery.h"

using namespace std;

void Draw(list<Entity>* ent, list<WorldBlock>* world, list<WorldEntity>* worldEntities);
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
	list<WorldEntity>* worldEntities;
};

struct WorldText
{
	string text;
	int fontSize;
};


int main()
{
	ALLEGRO_DISPLAY* display = 0;
	ALLEGRO_DISPLAY_MODE disp_data;
	ALLEGRO_TIMER* frame = 0;
	list<WorldBlock>* world = new list<WorldBlock>();
	list<Entity>* entities = new list<Entity>();
	list<WorldEntity>* worldEntities = new list<WorldEntity>();
	list<WorldScenery>* worldScenery = new list<WorldScenery>();

	ALLEGRO_SAMPLE* audio_click001 = NULL;
	ALLEGRO_SAMPLE* audio_click002 = NULL;
	ALLEGRO_SAMPLE* audio_bounce = NULL;
	ALLEGRO_SAMPLE* audio_coin = NULL;
	ALLEGRO_SAMPLE* audio_land = NULL;
	ALLEGRO_SAMPLE* audio_shot_start = NULL;
	ALLEGRO_SAMPLE* audio_shot_end = NULL;
	ALLEGRO_SAMPLE* audio_drone = NULL;

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
	font = al_load_ttf_font("fonts/coolvetica.ttf", 17, 0);

	if (!font)
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

	if (!al_reserve_samples(8)){
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
	audio_drone = al_load_sample("audio/drone.wav");

	if (!(audio_click001 && audio_click002 && audio_bounce && audio_coin && audio_land && audio_shot_start && audio_shot_end && audio_drone))
	{
		printf("An audio clip did not load!\n");
		return -1;
	}

	PlayerColor = al_map_rgb(20, 20, 220);
	JellyColor = al_map_rgb(20, 220, 20);
	WorldColor = al_map_rgb(20, 20, 20);
	BadWorldColor = al_map_rgb(220, 20, 20);
	JellyWorldColor = al_map_rgb(20, 20, 220);
	CoinColor = al_map_rgb(255, 140, 0);

	entities->push_back(Entity(Spawn.X, Spawn.Y, 20, 20, 0.0, 0.0, PlayerColor, PLAYER));
	Entity* player = &(*entities->begin());

	world->push_back(WorldBlock(710, 590, 140, 10, BadWorldColor, BADWORLD));
	world->push_back(WorldBlock(290, 590, 40, 10, BadWorldColor, BADWORLD));

	world->push_back(WorldBlock(0, 0, 4000, 10, WorldColor, WORLD));
	world->push_back(WorldBlock(0, 0, 10, 600, WorldColor, WORLD));
	world->push_back(WorldBlock(3990, 0, 10, 480, WorldColor, WORLD));
	world->push_back(WorldBlock(10, 500, 280, 100, WorldColor, WORLD));
	world->push_back(WorldBlock(330, 500, 380, 100, WorldColor, WORLD));
	world->push_back(WorldBlock(420, 480, 20, 20, WorldColor, WORLD));
	world->push_back(WorldBlock(510, 440, 20, 60, WorldColor, WORLD));
	world->push_back(WorldBlock(640, 480, 20, 20, WorldColor, WORLD));
	world->push_back(WorldBlock(850, 500, 3790, 100, WorldColor, WORLD));
	world->push_back(WorldBlock(1270, 380, 190, 120, WorldColor, WORLD));
	world->push_back(WorldBlock(1160, 340, 20, 110, WorldColor, WORLD));
	world->push_back(WorldBlock(1720, 380, 130, 120, WorldColor, WORLD));
	world->push_back(WorldBlock(2380, 470, 20, 10, WorldColor, WORLD));
	world->push_back(WorldBlock(2440, 460, 20, 20, WorldColor, WORLD));
	world->push_back(WorldBlock(2500, 450, 20, 30, WorldColor, WORLD));
	world->push_back(WorldBlock(2560, 440, 20, 40, WorldColor, WORLD));
	world->push_back(WorldBlock(2620, 430, 20, 50, WorldColor, WORLD));
	world->push_back(WorldBlock(2680, 420, 20, 60, WorldColor, WORLD));
	world->push_back(WorldBlock(2740, 410, 20, 70, WorldColor, WORLD));
	world->push_back(WorldBlock(2800, 400, 20, 80, WorldColor, WORLD));
	world->push_back(WorldBlock(2860, 390, 20, 90, WorldColor, WORLD));
	world->push_back(WorldBlock(2920, 380, 20, 100, WorldColor, WORLD));
	world->push_back(WorldBlock(2980, 370, 20, 110, WorldColor, WORLD));
	world->push_back(WorldBlock(3040, 360, 20, 120, WorldColor, WORLD));
	world->push_back(WorldBlock(3100, 350, 20, 130, WorldColor, WORLD));
	world->push_back(WorldBlock(3160, 340, 20, 140, WorldColor, WORLD));
	world->push_back(WorldBlock(3220, 330, 20, 150, WorldColor, WORLD));
	world->push_back(WorldBlock(3280, 320, 20, 160, WorldColor, WORLD));
	world->push_back(WorldBlock(3340, 310, 20, 170, WorldColor, WORLD));
	world->push_back(WorldBlock(630, 490, 10, 10, WorldColor, WORLD));
	world->push_back(WorldBlock(1460, 400, 10, 100, WorldColor, WORLD));
	world->push_back(WorldBlock(1470, 420, 10, 80, WorldColor, WORLD));
	world->push_back(WorldBlock(1480, 440, 10, 60, WorldColor, WORLD));
	world->push_back(WorldBlock(1490, 460, 10, 40, WorldColor, WORLD));
	world->push_back(WorldBlock(1500, 480, 10, 20, WorldColor, WORLD));
	world->push_back(WorldBlock(760, 460, 10, 50, WorldColor, WORLD));
	world->push_back(WorldBlock(760, 320, 10, 100, WorldColor, WORLD));
	world->push_back(WorldBlock(3650, 130, 20, 320, WorldColor, WORLD));
	world->push_back(WorldBlock(3760, 130, 20, 320, WorldColor, WORLD));
	world->push_back(WorldBlock(3780, 130, 110, 20, WorldColor, WORLD));
	world->push_back(WorldBlock(3870, 80, 20, 50, WorldColor, WORLD));

	world->push_back(WorldBlock(490, 500, 20, 10, JellyWorldColor, JELLYWORLD));
	world->push_back(WorldBlock(660, 480, 10, 20, JellyWorldColor, JELLYWORLD));
	world->push_back(WorldBlock(1270, 480, 10, 20, JellyWorldColor, JELLYWORLD));
	world->push_back(WorldBlock(1170, 430, 10, 20, JellyWorldColor, JELLYWORLD));
	world->push_back(WorldBlock(1270, 380, 10, 20, JellyWorldColor, JELLYWORLD));
	world->push_back(WorldBlock(1170, 340, 10, 20, JellyWorldColor, JELLYWORLD));
	world->push_back(WorldBlock(1510, 500, 210, 10, JellyWorldColor, JELLYWORLD));
	world->push_back(WorldBlock(2350, 500, 20, 10, JellyWorldColor, JELLYWORLD));
	world->push_back(WorldBlock(2560, 440, 20, 10, JellyWorldColor, JELLYWORLD));
	world->push_back(WorldBlock(2800, 400, 20, 10, JellyWorldColor, JELLYWORLD));
	world->push_back(WorldBlock(3040, 360, 20, 10, JellyWorldColor, JELLYWORLD));
	world->push_back(WorldBlock(3340, 310, 20, 10, JellyWorldColor, JELLYWORLD));
	world->push_back(WorldBlock(2380, 470, 20, 10, JellyWorldColor, JELLYWORLD));
	world->push_back(WorldBlock(2440, 460, 20, 10, JellyWorldColor, JELLYWORLD));
	world->push_back(WorldBlock(2500, 450, 20, 10, JellyWorldColor, JELLYWORLD));
	world->push_back(WorldBlock(2620, 430, 20, 10, JellyWorldColor, JELLYWORLD));
	world->push_back(WorldBlock(2680, 420, 20, 10, JellyWorldColor, JELLYWORLD));
	world->push_back(WorldBlock(2740, 410, 20, 10, JellyWorldColor, JELLYWORLD));
	world->push_back(WorldBlock(2860, 390, 20, 10, JellyWorldColor, JELLYWORLD));
	world->push_back(WorldBlock(2920, 380, 20, 10, JellyWorldColor, JELLYWORLD));
	world->push_back(WorldBlock(2980, 370, 20, 10, JellyWorldColor, JELLYWORLD));
	world->push_back(WorldBlock(3100, 350, 20, 10, JellyWorldColor, JELLYWORLD));
	world->push_back(WorldBlock(3160, 340, 20, 10, JellyWorldColor, JELLYWORLD));
	world->push_back(WorldBlock(3220, 330, 20, 10, JellyWorldColor, JELLYWORLD));
	world->push_back(WorldBlock(3280, 320, 20, 10, JellyWorldColor, JELLYWORLD));
	world->push_back(WorldBlock(3660, 430, 10, 20, JellyWorldColor, JELLYWORLD));
	world->push_back(WorldBlock(3660, 330, 10, 20, JellyWorldColor, JELLYWORLD));
	world->push_back(WorldBlock(3660, 230, 10, 20, JellyWorldColor, JELLYWORLD));
	world->push_back(WorldBlock(3660, 130, 10, 20, JellyWorldColor, JELLYWORLD));
	world->push_back(WorldBlock(3760, 380, 10, 20, JellyWorldColor, JELLYWORLD));
	world->push_back(WorldBlock(3760, 280, 10, 20, JellyWorldColor, JELLYWORLD));
	world->push_back(WorldBlock(3760, 180, 10, 20, JellyWorldColor, JELLYWORLD));
	world->push_back(WorldBlock(3670, 500, 90, 10, JellyWorldColor, JELLYWORLD));
	world->push_back(WorldBlock(1850, 430, 10, 10, JellyWorldColor, JELLYWORLD));
	world->push_back(WorldBlock(1850, 450, 10, 10, JellyWorldColor, JELLYWORLD));
	world->push_back(WorldBlock(1850, 470, 10, 10, JellyWorldColor, JELLYWORLD));
	world->push_back(WorldBlock(1850, 490, 10, 10, JellyWorldColor, JELLYWORLD));
	world->push_back(WorldBlock(1870, 490, 10, 10, JellyWorldColor, JELLYWORLD));
	world->push_back(WorldBlock(1890, 490, 10, 10, JellyWorldColor, JELLYWORLD));
	world->push_back(WorldBlock(1910, 490, 10, 10, JellyWorldColor, JELLYWORLD));
	world->push_back(WorldBlock(4000, 0, 640, 10, JellyWorldColor, JELLYWORLD));
	world->push_back(WorldBlock(4000, 0, 10, 480, JellyWorldColor, JELLYWORLD));
	world->push_back(WorldBlock(4000, 500, 640, 10, JellyWorldColor, JELLYWORLD));
	world->push_back(WorldBlock(4630, 0, 10, 510, JellyWorldColor, JELLYWORLD));
	world->push_back(WorldBlock(4220, 250, 200, 10, JellyWorldColor, JELLYWORLD));
	world->push_back(WorldBlock(4010, 40, 60, 10, JellyWorldColor, JELLYWORLD));
	world->push_back(WorldBlock(4010, 150, 60, 10, JellyWorldColor, JELLYWORLD));
	world->push_back(WorldBlock(4010, 290, 60, 10, JellyWorldColor, JELLYWORLD));
	world->push_back(WorldBlock(4010, 380, 60, 10, JellyWorldColor, JELLYWORLD));
	world->push_back(WorldBlock(4010, 220, 60, 10, JellyWorldColor, JELLYWORLD));
	world->push_back(WorldBlock(4010, 90, 60, 10, JellyWorldColor, JELLYWORLD));
	world->push_back(WorldBlock(4560, 480, 5, 20, JellyWorldColor, JELLYWORLD));
	world->push_back(WorldBlock(4570, 480, 5, 20, JellyWorldColor, JELLYWORLD));
	world->push_back(WorldBlock(4580, 480, 5, 20, JellyWorldColor, JELLYWORLD));
	world->push_back(WorldBlock(4590, 480, 5, 20, JellyWorldColor, JELLYWORLD));
	world->push_back(WorldBlock(4600, 480, 5, 20, JellyWorldColor, JELLYWORLD));
	world->push_back(WorldBlock(4610, 480, 5, 20, JellyWorldColor, JELLYWORLD));
	world->push_back(WorldBlock(4620, 480, 5, 20, JellyWorldColor, JELLYWORLD));
	world->push_back(WorldBlock(4610, 470, 20, 5, JellyWorldColor, JELLYWORLD));
	world->push_back(WorldBlock(4610, 460, 20, 5, JellyWorldColor, JELLYWORLD));
	world->push_back(WorldBlock(4610, 450, 20, 5, JellyWorldColor, JELLYWORLD));
	world->push_back(WorldBlock(4610, 440, 20, 5, JellyWorldColor, JELLYWORLD));
	world->push_back(WorldBlock(4610, 430, 20, 5, JellyWorldColor, JELLYWORLD));

	worldEntities->push_back(WorldEntity(520, 400, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(430, 470, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(770, 440, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(780, 440, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(790, 440, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(1190, 440, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(1260, 390, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(1190, 350, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(1260, 490, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(2390, 460, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(2510, 440, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(2630, 420, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(2750, 400, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(2870, 380, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(2990, 360, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(3110, 340, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(3230, 320, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(3350, 300, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(3350, 210, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(2350, 210, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(2340, 220, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(2350, 220, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(2340, 210, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(2360, 210, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(2360, 220, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(2360, 230, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(2350, 230, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(2340, 230, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(2340, 240, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(2350, 240, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(2360, 240, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(2370, 230, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(2370, 240, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(2370, 220, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(2360, 210, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(2370, 210, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(2380, 210, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(2380, 220, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(2380, 230, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(2380, 240, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(2340, 250, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(2350, 250, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(2360, 250, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(2370, 250, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(2380, 250, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(2380, 250, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(2450, 450, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(2570, 430, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(2690, 410, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(2810, 390, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(2930, 370, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(3050, 350, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(3170, 330, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(3290, 310, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(3680, 440, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(3750, 390, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(3680, 340, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(3750, 290, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(3680, 240, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(3750, 190, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(3680, 140, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(3780, 100, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(3780, 90, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(3790, 90, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(3790, 100, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(3780, 110, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(3780, 120, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(3790, 120, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(3790, 110, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(3800, 120, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(3800, 100, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(3800, 110, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(3800, 90, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(3810, 100, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(3810, 90, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(3820, 90, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(3820, 100, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(3810, 110, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(3810, 120, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(3820, 120, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(3820, 110, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(3830, 120, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(3830, 100, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(3830, 110, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(3830, 90, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(3840, 100, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(3840, 90, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(3850, 90, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(3850, 100, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(3840, 110, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(3840, 120, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(3850, 120, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(3850, 110, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(3860, 120, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(3860, 100, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(3860, 110, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(3860, 90, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(1854, 485, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(1854, 464, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(1854, 445, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(1865, 495, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(1885, 495, 5, 5, CoinColor, COIN));
	worldEntities->push_back(WorldEntity(1905, 495, 5, 5, CoinColor, COIN));

	// User input and drawing
	al_init_user_event_source(&UserEventSource);
	al_register_event_source(EventQueue, &UserEventSource);
	al_register_event_source(EventQueue, al_get_timer_event_source(frame));
	al_register_event_source(EventQueue, al_get_display_event_source(display));
	al_register_event_source(EventQueue, al_get_keyboard_event_source());
	al_register_event_source(EventQueue, al_get_mouse_event_source());

	//void AsyncPhysics(void* void_tickQueue, void* void_entities, void* void_world)
	PhysicsVariables physVar;
	physVar.entities = entities;
	physVar.world = world;
	physVar.worldEntities = worldEntities;

	thread physThread = thread(AsyncPhysics, (void*)&physVar);
	thread consoleThread = thread(DevConsole);

	// TODO: Meak pretti fix for this
	al_grab_mouse(display);
	al_start_timer(frame);
	al_play_sample(audio_drone, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_LOOP, NULL);

	while (!quit)
	{
		ALLEGRO_EVENT e;

		al_wait_for_event(EventQueue, &e);

		if (e.type == ALLEGRO_EVENT_TIMER)
		{
			Draw(entities, world, worldEntities);
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
		else if (e.type == 555)
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

	al_rest(0.1);

	physThread.detach();
	consoleThread.detach();

	delete world, entities;

	return 0;
}

void Draw(list<Entity>* entities, list<WorldBlock>* world, list<WorldEntity>* worldEntities)
{
	if (CLEAR_DRAW) al_clear_to_color(al_map_rgb(220, 220, 220));

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
	Coordinates viewPortA = Coordinates(posA.X - (SCREEN_W / 2 - player->GetWidth() / 2), 0.0);
	Coordinates viewPortB = Coordinates(posA.X + (SCREEN_W / 2 - player->GetWidth() / 2), SCREEN_H);

	mtx.lock();
	if (DRAW_PREDICTION && CLEAR_DRAW)
	{
		list<Entity> hackhack = { Entity(originX, originY, PROJECTILE_SIZE, PROJECTILE_SIZE, shotVec->X + entOff->X, shotVec->Y + entOff->Y, al_map_rgb(180, 180, 180), PROJECTILE) };
		list<Entity>::iterator ghostShot = hackhack.begin();

		Coordinates last = Coordinates(-1, -1);

		for (int i = 0; i < MAX_COLLISION_PREDICTION; ++i)
		{
			phys.ApplyGravity(&*ghostShot);
			phys.Collide(ghostShot, world, entities, worldEntities);
			ghostShot->MoveToOffset();

			if (last.X == ghostShot->GetACoordinates()->X && last.Y == ghostShot->GetACoordinates()->Y || ghostShot->GetRemove())
			{
				break;
			}
			else
			{
				if (phys.AreColliding(&viewPortA, &viewPortB, ghostShot->GetACoordinates(), ghostShot->GetBCoordinates()))
				{
					al_draw_filled_rectangle(
						ghostShot->GetACoordinates()->X + (SCREEN_W / 2 - player->GetWidth() / 2) + posA.X * -1,
						ghostShot->GetACoordinates()->Y,
						ghostShot->GetBCoordinates()->X + (SCREEN_W / 2 - player->GetWidth() / 2) + posA.X * -1,
						ghostShot->GetBCoordinates()->Y,
						ghostShot->GetColor());
				}
				last.X = ghostShot->GetACoordinates()->X;
				last.Y = ghostShot->GetACoordinates()->Y;
			}
		}
	}
	mtx.unlock();

	for (list<WorldBlock>::iterator wBlock = world->begin(); wBlock != world->end(); ++wBlock)
	{
		if (phys.AreColliding(&viewPortA, &viewPortB, wBlock->GetA(), wBlock->GetB()))
		{
			al_draw_filled_rectangle(
				wBlock->GetA()->X + (SCREEN_W / 2 - player->GetWidth() / 2) + posA.X * -1,
				wBlock->GetA()->Y,
				wBlock->GetB()->X + (SCREEN_W / 2 - player->GetWidth() / 2) + posA.X * -1,
				wBlock->GetB()->Y,
				wBlock->GetColor());
		}
	}

	mtx.lock();
	for (list<Entity>::iterator ent = ++entities->begin(); ent != entities->end(); ++ent)
	{
		Coordinates* entA = ent->GetACoordinates();

		if (phys.AreColliding(&viewPortA, &viewPortB, entA, ent->GetBCoordinates()))
		{
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
	}

	for (list<WorldEntity>::iterator wEntity = worldEntities->begin(); wEntity != worldEntities->end(); ++wEntity)
	{
		if (phys.AreColliding(&viewPortA, &viewPortB, wEntity->GetA(), wEntity->GetB()))
		{
			al_draw_filled_rectangle(
				wEntity->GetA()->X + (SCREEN_W / 2 - player->GetWidth() / 2) + posA.X * -1,
				wEntity->GetA()->Y,
				wEntity->GetB()->X + (SCREEN_W / 2 - player->GetWidth() / 2) + posA.X * -1,
				wEntity->GetB()->Y,
				wEntity->GetColor());
		}
	}
	mtx.unlock();

	// Player
	al_draw_filled_rectangle(SCREEN_W / 2 - player->GetWidth() / 2, posA.Y, SCREEN_W / 2 - player->GetWidth() / 2 + player->GetWidth(), posA.Y + player->GetHeight(), player->GetColor());

	// Gun of player
	al_draw_line((SCREEN_W / 2), originY + PROJECTILE_SIZE / 2, gunVec->X +      SCREEN_W / 2, gunVec->Y      + originY + PROJECTILE_SIZE / 2, BadWorldColor, 5.0);
	al_draw_line((SCREEN_W / 2), originY + PROJECTILE_SIZE / 2, blankGunVec->X + SCREEN_W / 2, blankGunVec->Y + originY + PROJECTILE_SIZE / 2, al_map_rgb(100, 100, 255), 5.0);

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
	al_draw_text(font, WorldColor, 1000, 115, 0, ("Score: " + to_string(Score)).c_str());

	al_flip_display();
}

void Move(Entity* ent)
{
	Coordinates* offset = ent->GetOffset();

	if (!FLY)
	{
		if (keys[RIGHT])
		{
			if (!(ent->GetLastImpactType() == JELLY && (ent->GetLastColPos() == RX || ent->GetLastColPos() == LX)))
			{
				if (offset->X < 0.0 && ent->GetIsAirBorn())
				{
					offset->X = offset->X > -PLAYER_AIR_CONTROL_STOP ? 0.0 : offset->X * PLAYER_AIR_CONTROL_BREAK;
				}
				else
				{
					offset->X += PLAYER_SPEED * ent->GetIsAirBorn() ? PLAYER_AIR_CONTROL : 1.0;
				}
			}
		}

		if (keys[LEFT])
		{
			if (!(ent->GetLastImpactType() == JELLY && (ent->GetLastColPos() == RX || ent->GetLastColPos() == LX)))
			{
				if (offset->X > 0.0 && ent->GetIsAirBorn())
				{
					offset->X = offset->X < PLAYER_AIR_CONTROL_STOP ? 0.0 : offset->X * PLAYER_AIR_CONTROL_BREAK;
				}
				else
				{
					offset->X -= PLAYER_SPEED * ent->GetIsAirBorn() ? PLAYER_AIR_CONTROL : 1.0;
				}
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

		ALLEGRO_EVENT e;
		e.type = 555;
		e.user.data1 = PLAYER_SHOOT;
		al_emit_user_event(&UserEventSource, &e, 0);
	}
}

void AsyncPhysics(void* struc)
{
	list<WorldBlock>* world = ((PhysicsVariables*)struc)->world;
	list<Entity>* entities = ((PhysicsVariables*)struc)->entities;
	list<WorldEntity>* worldEntities = ((PhysicsVariables*)struc)->worldEntities;
	Entity* player = &(*entities->begin());

	while (!quit)
	{
		al_rest(1.0 / PHYSICS_TICK);
		
		mtx.lock();
		Move(player);
		phys.ApplyPhysics(entities, world, worldEntities);
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
				cout << "Availabe commands are: \n" << "PHYSICS_TICK\nGRAVITY\nPLAYER_SPEED\nPLAYER_JUMP_SPEED\nPLAYER_BOUNCE_OFFSET\nPLAYER_AIR_CONTROL\nPLAYER_AIR_CONTROL_BREAK;\nPLAYER_SIDE_UP_BOUNCE\nPLAYER_SIDE_SIDE_BOUNCE\nPROJECTILE_SPEED\nPROJECTILE_SIZE\nPROJECTILE_TRAIL_LENGTH\nPROJECTILE_BOUNCINESS\nMAX_ENTITIES\nMAX_ENTITY_AGE\nMAX_ENTITY_VELOCITY\nFRICTION\nFRICTION_STOP\nDRAW_PREDICTION\nCLEAR_DRAW\n";
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