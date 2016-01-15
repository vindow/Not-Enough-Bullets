#define SDL_MAIN_HANDLED
#include<SDL.h>
#include<GL/glew.h>
#include<stdio.h>
#include<stdlib.h>
#include<vector>
#include<algorithm>
#include<cmath>
#include<iostream>
#include "DrawUtils.h"

struct AABB {
	float x;
	float y;
	int h;
	int w;
	std::vector<long long int>* bitmap;
};
struct EnemyData {
	GLuint sprite;
	AABB* box;
	int cooldown;
	float movespeed;
};
struct PlayerData {
	GLuint sprite;
	AABB* box;
};
struct Laser {
	GLuint sprite;
	AABB* box;
	int alliance;
};

enum StateType {
	STATE_SPLASH_SCREEN,
	STATE_MAIN_MENU,
	STATE_PLAYING,
	STATE_GAME_OVER
};
enum EventType {
	ET_PLAYER_LASER_COLLISION,
	ET_ENEMY_LASER_COLLISION,
	ET_ENEMY_PLAYER_COLLISION,
	ET_VICTORY,
	ET_GAME_START
};
struct Event {
	EventType ev;
	int laserindex;
	int enemyindex;
};

std::vector<Laser> plasers;
std::vector<Laser> elasers;
std::vector<EnemyData> enemies;
struct AABB* playerbox = new struct AABB;
struct PlayerData* player = new struct PlayerData;
AABB* camera = new struct AABB;
bool gameover = false;
bool gamestart = false;
bool victory = false;
int playerw;
int playerh;
int enemy1w;
int enemy1h;
int enemy2w;
int enemy2h;
StateType gameState;
GLuint playersprite;
GLuint enemy1sprite;
GLuint enemy2sprite;


void HandleEvent(Event* event) {
	switch (event->ev) {
	case 0:
		//spawn debris, remove player, remove laser, remove life,maybe gameover

		gameover = true;
		elasers[event->laserindex] = elasers[elasers.size() - 1];
		elasers.pop_back();
		player->box->x = 150;
		player->box->y = camera->y + camera->h;
		break;
	case 1:
		//remove enemy, remove laser
		/*plasers[event->laserindex] = plasers[plasers.size() - 1];
		plasers.pop_back();*/
		enemies[event->enemyindex] = enemies[enemies.size() - 1];
		enemies.pop_back();
		break;
	case 2:
		//remove enemy and player, gameover
		gameover = true;
		enemies[event->enemyindex] = enemies[enemies.size() - 1];
		enemies.pop_back();
		player->box->x = 150;
		player->box->y = camera->y + camera->h;
		break;
	case 3:
		//set victory flag to yes, running to no
		gamestart = false;
		gameover = false;
		victory = true;
		break;
	case 4:
		//set gamestart to true, initialize enemies
		gamestart = true;
		gameover = false;
		victory = false;

		
		for (int i = 0; i<100; i++)
		{

			int random = rand() % 100 + 1;
			struct AABB* ebox1 = new struct AABB;
			ebox1->h = enemy1h;
			ebox1->w = enemy1w;
			if (random <= 25)
			{
				ebox1->x = 25;
			}
			else if (random >25 && random <= 50)
			{
				ebox1->x = 300;
			}
			else if (random >50 && random <= 75)
			{
				ebox1->x = 200;
			}
			else if (random > 75 && random <= 100)
			{
				ebox1->x = 100;
			}

			ebox1->y = 56500 - (535 * i);

			struct EnemyData* enemy1 = new struct EnemyData;
			enemy1->sprite = enemy1sprite;
			enemy1->box = ebox1;
			enemy1->cooldown = 0;
			enemy1->movespeed = 2;
			enemies.push_back(*enemy1);

		}

		for (int j = 0; j<50; j++)
		{
			int random = rand() % 100 + 1;
			struct AABB* ebox2 = new struct AABB;
			ebox2->h = enemy2h;
			ebox2->w = enemy2w;
			if (random <= 25)
			{
				ebox2->x = 50;
			}
			else if (random >25 && random <= 50)
			{
				ebox2->x = 250;
			}
			else if (random >50 && random <= 75)
			{
				ebox2->x = 350;
			}
			else if (random > 75 && random <= 100)
			{
				ebox2->x = 75;
			}
			ebox2->y = 56000 - (j * 530);

			struct EnemyData* enemy2 = new struct EnemyData;
			enemy2->sprite = enemy2sprite;
			enemy2->box = ebox2;
			enemy2->cooldown = 0;
			enemy2->movespeed = 2;
			enemies.push_back(*enemy2);
		}
		playerbox->h = playerh;
		playerbox->w = playerw;
		playerbox->x = 150;
		playerbox->y = 60600;

		player->box = playerbox;

		camera->x = 0;
		camera->y = 60000;
		camera->h = 600;
		camera->w = 400;
		break;
	default:
		break;
	}
}

std::vector<Event> eventQueue;

void initEventQueue(void) {
	eventQueue.reserve(16);
}

void QueueEvent(Event* ev)
{
	eventQueue.push_back(*ev);
}

void UpdateEventQueue(void)
{
	int i;
	for (i = 0; i < eventQueue.size(); ++i) {
		HandleEvent(&eventQueue[i]);
	}
	eventQueue.clear(); //< note: keeps memory around
}


bool AABBIntersect(const AABB* box1, const AABB* box2)
{
	// box1 to the right
	if (box1->x > box2->x + box2->w) {
		return false;
	}
	// box1 to the left
	if (box1->x + box1->w < box2->x) {
		return false;
	}
	// box1 below
	if (box1->y > box2->y + box2->h) {
		return false;
	}
	// box1 above
	if (box1->y + box1->h < box2->y) {
		return false;
	}
	return true;
}

bool checkPixCollision(AABB* b1, AABB* b2, AABB intersect, bool left, bool up) {
	int i;
	long long int temp = 0;
	std::vector<long long int> bitmap1 = *(b1->bitmap);
	std::vector<long long int> bitmap2 = *(b2->bitmap);
	if (up) {
		for (i = 0; i < intersect.h; i++) {
			if (left) {
				temp = bitmap1[bitmap1.size() - intersect.h + i - 1] << (b1->w - intersect.w) & bitmap2[i];
			}
			else {
				temp = bitmap1[bitmap1.size() - intersect.h + i - 1] & bitmap2[i] << (b2->w - intersect.w);
			}
			if (temp != 0) {
				return true;
			}
		}
	}
	else {
		for (i = 0; i < intersect.h; i++) {
			if (left) {
				temp = bitmap2[bitmap2.size() - intersect.h + i - 1] & bitmap2[i] << (b1->w - intersect.w);
			}
			else {
				temp = bitmap2[bitmap2.size() - intersect.h + i - 1] << (b2->w - intersect.w) & bitmap2[i];
			}
			if (temp != 0) {
				return true;
			}
		}
	}
	return false;
}

int main(void)
{
	gameState = STATE_SPLASH_SCREEN;
	bool shouldExit = false;
	// Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		return 1;
	}
	// Create the window, OpenGL context
	SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_Window* window = SDL_CreateWindow(
		"TestSDL",
		SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		400, 600,
		SDL_WINDOW_OPENGL);
	if (!window) {
		SDL_Quit();
		return 1;
	}
	SDL_GL_CreateContext(window);
	// Make sure we have a recent version of OpenGL
	GLenum glewError = glewInit();
	if (glewError != GLEW_OK) {
		SDL_Quit();
		return 1;
	}
	if (!GLEW_VERSION_1_5) {
		SDL_Quit();
		return 1;
	}

	Uint8 prevKeys[SDL_NUM_SCANCODES] = { 0 };
	const Uint8* currentKeys = SDL_GetKeyboardState(NULL);

	initEventQueue();

	playersprite = glTexImageTGAFile("playerShip3_green.tga", &playerw, &playerh);
	
	enemy1sprite = glTexImageTGAFile("enemyBlack1.tga", &enemy1w, &enemy1h);

	enemy2sprite = glTexImageTGAFile("enemyBlack2.tga", &enemy2w, &enemy2h);

	int playerlaserw = 0;
	int playerlaserh = 0;
	std::vector<long long int> playerlaserbitmap;
	GLuint playerlasersprite = glTexImageTGAFileBitmap("laserGreen10.tga", &playerlaserw, &playerlaserh, playerlaserbitmap);
	
	int enemylaser8w = 0;
	int enemylaser8h = 0;
	std::vector<long long int> enemylaser8bitmap;
	GLuint enemylasersprite8 = glTexImageTGAFileBitmap("laserRed08.tga", &enemylaser8w, &enemylaser8h, enemylaser8bitmap);

	int titlew = 0;
	int titleh = 0;
	GLuint titlesprite = glTexImageTGAFile("TitleScreen.tga", &titlew, &titleh);

	int victoryw = 0;
	int victoryh = 0;
	GLuint victorysprite = glTexImageTGAFile("YouWinScreen.tga", &victoryw, &victoryh);

	int gameoverw = 0;
	int gameoverh = 0;
	GLuint gameoversprite = glTexImageTGAFile("GameOver.tga", &gameoverw, &gameoverh);

	camera->x = 0;
	camera->y = 60000;
	camera->h = 600;
	camera->w = 400;
	
	int width = 40;
	int height = 40;
	glViewport(0, 0, 400, 600);
	glMatrixMode(GL_PROJECTION);
	glOrtho(0, 400, 600, 0, 0, 1);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER, 0.04);
	GLuint spaceTex = glTexImageTGAFile("blue.tga", &width, &height);


	GLuint background[2][240];
	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < 240; j++) {
			background[i][j] = spaceTex;
		}
	}
	player->sprite = playersprite;

	int ticksPerFrame = 1000 / 60; //< or whatever other framerate you want as a max
	int prevTick = SDL_GetTicks();
	int ticksPerPhysics = 1000 / 100;
	int prevPhysicsTick = prevTick;

	int firecooldown = 0;

	

	while (!shouldExit) {
		switch (gameState) {
		case 0:
			break;
		case 1:
			break;
		case 2:
			break;
		case 3:
			break;
		}
		// Handle OS message pump
		SDL_Event event;
		memcpy(prevKeys, currentKeys, sizeof(prevKeys));

		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_QUIT:
				return 0;
			}
		}
		if (gameover) {
			glDrawSprite(gameoversprite, 0, 0, gameoverw, gameoverh);
			currentKeys = SDL_GetKeyboardState(NULL);
			if (currentKeys[SDL_SCANCODE_SPACE]) {
				
				struct Event* newEvent = new struct Event;
				newEvent->ev = ET_GAME_START;
				QueueEvent(newEvent);
			}
			UpdateEventQueue();
		}
		else if (victory) {
			glDrawSprite(victorysprite, 0, 0, victoryw, victoryh);
			currentKeys = SDL_GetKeyboardState(NULL);
			if (currentKeys[SDL_SCANCODE_SPACE]) {
				struct Event* newEvent = new struct Event;
				newEvent->ev = ET_GAME_START;
				QueueEvent(newEvent);
			}
			UpdateEventQueue();
		}
		else if (!gamestart && !victory && !gameover) {
			glDrawSprite(titlesprite, 0, 0, titlew, titleh);
			currentKeys = SDL_GetKeyboardState(NULL);
			if (currentKeys[SDL_SCANCODE_SPACE]) {
				struct Event* newEvent = new struct Event;
				newEvent->ev = ET_GAME_START;
				QueueEvent(newEvent);
			}
			UpdateEventQueue();
		}
		else {

			//Cap framerate
			int tick = SDL_GetTicks();
			do {
				SDL_Delay(std::max(0, ticksPerFrame - (tick - prevTick)));
				tick = SDL_GetTicks();
			} while (ticksPerFrame - (tick - prevTick) > 0);
			prevTick = tick;

			//update input, move player
			currentKeys = SDL_GetKeyboardState(NULL);
			if (currentKeys[SDL_SCANCODE_LEFT]) {
				player->box->x -= 3;
			}
			if (currentKeys[SDL_SCANCODE_UP]) {
				player->box->y -= 3;
			}
			if (currentKeys[SDL_SCANCODE_RIGHT]) {
				player->box->x += 3;
			}
			if (currentKeys[SDL_SCANCODE_DOWN]) {
				player->box->y += 3;
			}
			if (currentKeys[SDL_SCANCODE_SPACE] && firecooldown == 0) {
				struct AABB* plaserbox = new struct AABB;
				plaserbox->h = playerlaserh;
				plaserbox->w = playerlaserw;
				plaserbox->x = player->box->x + player->box->w / 2 - playerlaserw / 2;
				plaserbox->y = player->box->y - playerlaserh + 10;
				plaserbox->bitmap = &playerlaserbitmap;

				struct Laser* plaser = new struct Laser;
				plaser->box = plaserbox;
				plaser->sprite = playerlasersprite;
				plaser->alliance = 0;

				plasers.push_back(*plaser);
				firecooldown = 20;
			}

			//autoscroll camera
			camera->y -= 5;
			player->box->y -= 5;
			//Draw the tiles
			for (int i = 0; i < 2; i++) {
				for (int j = floor(camera->y / height); j < floor(camera->y / height) + 4; j++) {
					glDrawSprite(background[i][j], i * width - camera->x, j * height - camera->y, height, width);
				}
			}

			//draw the player's lasers, kill offscreen ones, move live ones
			int i;
			for (i = 0; i < plasers.size(); i++) {
				plasers[i].box->y -= 20;
				if (plasers[i].box->y < camera->y) {
					plasers[i] = plasers[plasers.size() - 1];
					plasers.pop_back();
				}
				glDrawSprite(plasers[i].sprite, plasers[i].box->x - camera->x, plasers[i].box->y - camera->y, plasers[i].box->w, plasers[i].box->h);
			}
			//draw enemy lasers, kill offscreen enemy lasers
			for (i = 0; i < elasers.size(); i++) {
				glDrawSprite(elasers[i].sprite, elasers[i].box->x - camera->x, elasers[i].box->y - camera->y, elasers[i].box->w, elasers[i].box->h);
				if (elasers[i].box->y > camera->h + camera->y) {
					elasers[i] = elasers[elasers.size() - 1];
					elasers.pop_back();
				}
			}
			//draw the player
			glDrawSprite(player->sprite, player->box->x - camera->x, player->box->y - camera->y, player->box->w, player->box->h);

			//draw and move the drawn the NPCs, depsawn the exited ones
			for (i = 0; i < enemies.size(); i++) {
				if (AABBIntersect(camera, enemies[i].box)) {
					glDrawSprite(enemies[i].sprite, enemies[i].box->x - camera->x, enemies[i].box->y - camera->y, enemies[i].box->w, enemies[i].box->h);
					if (enemies[i].cooldown == 0) {
						enemies[i].cooldown = 50;
						struct AABB* elaserbox = new struct AABB;
						elaserbox->h = enemylaser8h;
						elaserbox->w = enemylaser8w;
						elaserbox->x = enemies[i].box->x + enemies[i].box->w / 2;
						elaserbox->y = enemies[i].box->y + enemies[i].box->h;

						struct Laser* elaser = new struct Laser;
						elaser->alliance = 1;
						elaser->box = elaserbox;
						elaser->sprite = enemylasersprite8;

						elasers.push_back(*elaser);
					}
					enemies[i].box->y -= enemies[i].movespeed;
				}
				if (enemies[i].box->y > camera->h + camera->y) {
					enemies[i] = enemies[enemies.size() - 1];
					enemies.pop_back();
				}

			}
			if (enemies.size() == 0) {
				struct Event* newEvent = new struct Event;
				newEvent->ev = ET_VICTORY;
				QueueEvent(newEvent);
			}
			if (firecooldown != 0) {
				firecooldown--;
			}
			// Update physics
			while (tick > prevPhysicsTick + ticksPerPhysics) {
				if (player->box->x < 0) {
					player->box->x = 0;
				}
				if (player->box->x > 400 - player->box->w) {
					player->box->x = 400 - player->box->w;
				}
				if (player->box->y > camera->y + camera->h - player->box->h) {
					player->box->y = camera->y + camera->h - player->box->h;
				}
				if (player->box->y < camera->y) {
					player->box->y = camera->y;
				}
				int i, j, intersecth, intersectw;
				int enemiessize = enemies.size();
				int plaserssize = plasers.size();
				int elaserssize = elasers.size();
				for (i = 0; i < enemiessize; i++) {
					for (j = 0; j < plaserssize; j++) {
						if (AABBIntersect(enemies[i].box, plasers[j].box)) {
							struct Event* newEvent = new struct Event;
							newEvent->ev = ET_ENEMY_LASER_COLLISION;
							newEvent->enemyindex = i;
							newEvent->laserindex = j;
							QueueEvent(newEvent);

						}
					}
				}
				for (i = 0; i < enemiessize; i++) {
					if (AABBIntersect(enemies[i].box, player->box)) {
						struct Event* newEvent = new struct Event;
						newEvent->ev = ET_ENEMY_PLAYER_COLLISION;
						newEvent->enemyindex = i;
						QueueEvent(newEvent);

					}
				}
				for (i = 0; i < elaserssize; i++) {
					if (AABBIntersect(player->box, elasers[i].box)) {
						struct Event* newEvent = new struct Event;
						newEvent->ev = ET_PLAYER_LASER_COLLISION;
						newEvent->laserindex = i;
						QueueEvent(newEvent);
					}
				}
				prevPhysicsTick += ticksPerPhysics;
				UpdateEventQueue();
			}
		}
		


		SDL_GL_SwapWindow(window);
	}
	SDL_Quit();
	return 0;
}