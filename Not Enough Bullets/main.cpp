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
	int lives;
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
	STATE_GAME_OVER,
	STATE_VICTORY,
	STATE_PLAYING
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
int tilew;
int tileh;
int lifespritew;
int lifespriteh;
StateType gameState = STATE_SPLASH_SCREEN;
GLuint playersprite;
GLuint enemy1sprite;
GLuint enemy2sprite;
GLuint background[4][240];
GLuint lifesprite;
std::vector<long long int> playerbitmap;
std::vector<long long int> enemy1bitmap;
std::vector<long long int> enemy2bitmap;

void init() {
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
		ebox1->bitmap = &enemy1bitmap;

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
		ebox2->bitmap = &enemy2bitmap;

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
	playerbox->bitmap = &playerbitmap;

	player->box = playerbox;
	player->lives = 3;

	camera->x = 0;
	camera->y = 60000;
	camera->h = 600;
	camera->w = 400;
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


void drawAll()
{
	int i;
	//draw the background that is onscreen
	for (int i = 0; i < 4; i++) {
		for (int j = floor(camera->y / tileh); j < floor(camera->y / tileh) + 4; j++) {
			glDrawSprite(background[i][j], i * tilew - camera->x, j * tileh - camera->y, tileh, tilew);
		}
	}
	//draw the player's lasers, kill offscreen ones, move live ones
	for (i = 0; i < plasers.size(); i++) {
		glDrawSprite(plasers[i].sprite, plasers[i].box->x - camera->x, plasers[i].box->y - camera->y, plasers[i].box->w, plasers[i].box->h);
	}
	//draw enemy lasers, kill offscreen enemy lasers
	for (i = 0; i < elasers.size(); i++) {
		glDrawSprite(elasers[i].sprite, elasers[i].box->x - camera->x, elasers[i].box->y - camera->y, elasers[i].box->w, elasers[i].box->h);
	}
	//draw enemies that are onscreen
	for (i = 0; i < enemies.size(); i++) {
		if (AABBIntersect(camera, enemies[i].box))
			glDrawSprite(enemies[i].sprite, enemies[i].box->x - camera->x, enemies[i].box->y - camera->y, enemies[i].box->w, enemies[i].box->h);
	}
	//draw the player
	glDrawSprite(player->sprite, player->box->x - camera->x, player->box->y - camera->y, player->box->w, player->box->h);
	for (i = 0; i < player->lives; i++) {
		glDrawSprite(lifesprite, 790 - ((lifespritew + 5) * (i + 1)), 10, lifespritew, lifespriteh);
	}
}

bool checkPixCollision(AABB* b1, AABB* b2) {
	//Get the intersect of the AABB collision
	struct AABB intersect;
	if (b1->x < b2->x) {
		intersect.x = b2->x;
	}
	else {
		intersect.x = b1->x;
	}
	if (b1->x + b1->w < b2->x + b2->w) {
		intersect.w = b1->x + b1->w - intersect.x;
	}
	else {
		intersect.w = b2->x + b2->w - intersect.x;
	}
	if (b1->y < b2->y) {
		intersect.y = b2->y;
	}
	else {
		intersect.y = b1->y;
	}
	if (b1->y + b1->h < b2->y + b2->h) {
		intersect.h = b1->y + b1->h - intersect.y;
	}
	else {
		intersect.h = b2->y + b2->h - intersect.y;
	}

	//check pixel perfect collision
	int i;
	long long int temp = 0;
	std::vector<long long int> bitmap1 = *(b1->bitmap);
	std::vector<long long int> bitmap2 = *(b2->bitmap);
	if (b1->y + b1->h < b2->y + b2->h) { //box1 above box2 or box1 inside box2 on y-axis
		for (i = 0; i < intersect.h; i++) {
			if (b1->x + b1->w < b2->x + b2->w) { //box1 left of box2 or inside box2 on x-axis
				temp = (bitmap1[bitmap1.size() - intersect.h + i] << (int)(b2->x + b2->w - b1->x - b1->w)) & bitmap2[i];
			}
			else { //box1 right of box2 or encompasses box2 on x-axis
				temp = (bitmap2[bitmap2.size() - intersect.h + i] << (int)(b1->x + b1->w - b2->x - b2->w)) & bitmap1[i];
			}
			if (temp != 0) {
				return true;
			}
		}
	}
	else { //box1 below box2 or box 1 encompasses box2 on y-axis (box 2 above box 1)
		for (i = 0; i < intersect.h; i++) {
			if (b2->x + b2->w < b1->x + b1->w) { //box2 left of box1 or inside box1 on x-axis
				temp = (bitmap2[bitmap2.size() - intersect.h + i] << (int)(b1->x + b1->w - b2->x - b2->w)) & bitmap1[i];
			}
			else { //box2 right of box1 or encompasses box1 on x-axis
				temp = (bitmap1[bitmap1.size() - intersect.h + i] << (int)(b2->x + b2->w - b1->x - b1->w)) & bitmap2[i];
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
	bool shouldExit = false;
	// ---------------Initialize SDL and OpenGL stuff---------------
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		return 1;
	}
	// Create the window, OpenGL context
	SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_Window* window = SDL_CreateWindow(
		"TestSDL",
		SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		800, 600,
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

	//Setup keyboard input
	Uint8 prevKeys[SDL_NUM_SCANCODES] = { 0 };
	const Uint8* currentKeys = SDL_GetKeyboardState(NULL);

	playersprite = glTexImageTGAFileBitmap("Assets/Sprites/playerShip3_green.tga", &playerw, &playerh, playerbitmap);
	
	enemy1sprite = glTexImageTGAFileBitmap("Assets/Sprites/enemyBlack1.tga", &enemy1w, &enemy1h, enemy1bitmap);
	
	enemy2sprite = glTexImageTGAFileBitmap("Assets/Sprites/enemyBlack2.tga", &enemy2w, &enemy2h, enemy2bitmap);

	int playerlaserw = 0;
	int playerlaserh = 0;
	std::vector<long long int> playerlaserbitmap;
	GLuint playerlasersprite = glTexImageTGAFileBitmap("Assets/Sprites/laserGreen10.tga", &playerlaserw, &playerlaserh, playerlaserbitmap);
	
	int enemylaser8w = 0;
	int enemylaser8h = 0;
	std::vector<long long int> enemylaser8bitmap;
	GLuint enemylasersprite8 = glTexImageTGAFileBitmap("Assets/Sprites/laserRed08.tga", &enemylaser8w, &enemylaser8h, enemylaser8bitmap);

	int splashscreenw = 0, splashscreenh = 0;
	GLuint splashScreen = glTexImageTGAFile("Assets/Sprites/SplashScreen.tga", &splashscreenw, &splashscreenh);

	int titlew = 0, titleh = 0;
	GLuint titlesprite = glTexImageTGAFile("Assets/Sprites/TitleScreen.tga", &titlew, &titleh);

	int victoryw = 0;
	int victoryh = 0;
	GLuint victorysprite = glTexImageTGAFile("Assets/Sprites/YouWinScreen.tga", &victoryw, &victoryh);

	int gameoverw = 0;
	int gameoverh = 0;
	GLuint gameoversprite = glTexImageTGAFile("Assets/Sprites/GameOver.tga", &gameoverw, &gameoverh);

	lifesprite = glTexImageTGAFile("Assets/Sprites/playerLife3_green.tga", &lifespritew, &lifespriteh);

	camera->x = 0;
	camera->y = 60000;
	camera->h = 600;
	camera->w = 800;
	
	
	glViewport(0, 0, 800, 600);
	glMatrixMode(GL_PROJECTION);
	glOrtho(0, 800, 600, 0, 0, 1);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER, 0.04);
	GLuint spaceTex = glTexImageTGAFile("Assets/Sprites/blue.tga", &tilew, &tileh);


	
	for (int i = 0; i < 4; i++) {
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
		// Handle OS message pump
		SDL_Event event;
		memcpy(prevKeys, currentKeys, sizeof(prevKeys));

		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_QUIT:
				return 0;
			}
		}
		switch (gameState) {
		case 0: //Splash Screen
			glDrawSprite(splashScreen, 0, 0, splashscreenw, splashscreenh);
			currentKeys = SDL_GetKeyboardState(NULL);
			if (currentKeys[SDL_SCANCODE_SPACE] && !prevKeys[SDL_SCANCODE_SPACE]) {
				gameState = STATE_MAIN_MENU;
			}
			break;
		case 1: //Main Menu
			glDrawSprite(titlesprite, 0, 0, titlew, titleh);
			currentKeys = SDL_GetKeyboardState(NULL);
			if (currentKeys[SDL_SCANCODE_SPACE] && !prevKeys[SDL_SCANCODE_SPACE]) {
				gameState = STATE_PLAYING;
				init();
			}
			break;
		case 2: //Game Over
			glDrawSprite(gameoversprite, 0, 0, gameoverw, gameoverh);
			currentKeys = SDL_GetKeyboardState(NULL);
			if (currentKeys[SDL_SCANCODE_SPACE] && !prevKeys[SDL_SCANCODE_SPACE]) {
				gameState = STATE_PLAYING;
				init();
			}
			break;
		case 3: //Victory
			glDrawSprite(victorysprite, 0, 0, victoryw, victoryh);
			break;
		case 4: //Playing
		{
			//Cap framerate
			int tick = SDL_GetTicks();
			do {
				SDL_Delay(std::max(0, ticksPerFrame - (tick - prevTick)));
				tick = SDL_GetTicks();
			} while (ticksPerFrame - (tick - prevTick) > 0);
			prevTick = tick;

			//update/process input, move player
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
				//create hitbox for laser
				struct AABB* plaserbox = new struct AABB;
				plaserbox->h = playerlaserh;
				plaserbox->w = playerlaserw;
				plaserbox->x = player->box->x + player->box->w / 2 - playerlaserw / 2;
				plaserbox->y = player->box->y - playerlaserh + 10;
				plaserbox->bitmap = &playerlaserbitmap;

				//create laser and attach hitbox
				struct Laser* plaser = new struct Laser;
				plaser->box = plaserbox;
				plaser->sprite = playerlasersprite;
				plaser->alliance = 0;

				//add laser and reset shot cooldown
				plasers.push_back(*plaser);
				firecooldown = 20;
			}

			//autoscroll camera and automove player to be level with camera
			camera->y -= 5;
			player->box->y -= 5;

			//move the NPCs, depsawn offscreen ones
			int i;
			for (i = 0; i < enemies.size(); i++) {
				if (AABBIntersect(camera, enemies[i].box)) {
					if (enemies[i].cooldown == 0) { //fire enemy laser
						//create enemy laser hitbox
						struct AABB* elaserbox = new struct AABB;
						elaserbox->h = enemylaser8h;
						elaserbox->w = enemylaser8w;
						elaserbox->x = enemies[i].box->x + enemies[i].box->w / 2;
						elaserbox->y = enemies[i].box->y + enemies[i].box->h;
						elaserbox->bitmap = &enemylaser8bitmap;

						//create enemy laser and attach hitbox
						struct Laser* elaser = new struct Laser;
						elaser->alliance = 1;
						elaser->box = elaserbox;
						elaser->sprite = enemylasersprite8;

						//add enemy laser to vector and reset shot cooldown
						elasers.push_back(*elaser);
						enemies[i].cooldown = 50;
					}
					enemies[i].box->y -= enemies[i].movespeed;
				}
				if (enemies[i].box->y > camera->h + camera->y) { //check if enemy is offscreen
					enemies[i] = enemies[enemies.size() - 1];
					enemies.pop_back();
				}
			}
			//move player lasers and despawn offscreen ones
			for (i = 0; i < plasers.size(); i++) {
				plasers[i].box->y -= 20;
				if (plasers[i].box->y < camera->y) {
					plasers[i] = plasers[plasers.size() - 1];
					plasers.pop_back();
				}
			}
			//"move" enemy lasers and despawn offscreen ones
			for (i = 0; i < elasers.size(); i++) {
				if (elasers[i].box->y > camera->h + camera->y) {
					elasers[i] = elasers[elasers.size() - 1];
					elasers.pop_back();
				}
			}

			if (enemies.size() == 0) {
				gameState = STATE_VICTORY;
			}

			if (firecooldown != 0) {
				//decrement the fire cooldown timer
				firecooldown--;
			}

			// Update physics (collisions)
			while (tick > prevPhysicsTick + ticksPerPhysics) {
				if (player->box->x < 0) {
					player->box->x = 0;
				}
				if (player->box->x > 800 - player->box->w) {
					player->box->x = 800 - player->box->w;
				}
				if (player->box->y > camera->y + camera->h - player->box->h) {
					player->box->y = camera->y + camera->h - player->box->h;
				}
				if (player->box->y < camera->y) {
					player->box->y = camera->y;
				}
				int i, j, intersecth, intersectw;
				for (i = 0; i < enemies.size(); i++) {
					for (j = 0; j < plasers.size(); j++) {
						if (AABBIntersect(enemies[i].box, plasers[j].box)) { //Player laser hits enemy
							if (checkPixCollision(plasers[j].box, enemies[i].box)) {
								plasers[j] = plasers[plasers.size() - 1];
								plasers.pop_back();
								enemies[i] = enemies[enemies.size() - 1];
								enemies.pop_back();
							}
						}
					}
				}
				for (i = 0; i < enemies.size(); i++) {
					if (AABBIntersect(enemies[i].box, player->box)) { //Enemy hits player
						if (checkPixCollision(enemies[i].box, player->box)) {
							enemies[i] = enemies[enemies.size() - 1];
							enemies.pop_back();
							player->box->x = 150;
							player->box->y = camera->y + camera->h;
							player->lives -= 1;
							if (player->lives == 0) {
								gameState = STATE_GAME_OVER;
							}
						}
					}
				}
				for (i = 0; i < elasers.size(); i++) {
					if (AABBIntersect(player->box, elasers[i].box)) { //Enemy laser hits player
						if (checkPixCollision(player->box, elasers[i].box)) {
							elasers[i] = elasers[elasers.size() - 1];
							elasers.pop_back();
							player->box->x = 150;
							player->box->y = camera->y + camera->h;
							player->lives -= 1;
							if (player->lives == 0) {
								gameState = STATE_GAME_OVER;
							}
						}
					}
				}
				prevPhysicsTick += ticksPerPhysics;
			}
			drawAll();
			break;
		}
		default:
			break;
		}

		SDL_GL_SwapWindow(window);
	}
	SDL_Quit();
	return 0;
}