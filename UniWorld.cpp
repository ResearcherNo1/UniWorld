#include "graphics.h"
#include <conio.h>

long long world[WORLD_WIDTH][((unsigned long long)WORLD_HEIGHT+2)]; //Массив мира
std::vector<bot> bots;	//Вектор ботов
int season; //9 - зима; 10 - весна, осень; 11 - лето
bool work;
uint_fast64_t lifeCount = 0; //Счётчик ходов

gui::LWindow gWindow;; //The window we'll be rendering to
SDL_Renderer* gRenderer = NULL; //The window renderer

//Scene sprites
SDL_Rect gSpriteClips[2];
gui::LTexture gSpriteBot;
gui::LTexture gSpriteOrganic;


int _cdecl main(int argc, char* args[]) {
	gui::initialize();
	printf("GUI has been initialized. Press any key to continue...");
	_getch();

	//Ставим стены
	for (size_t i = 0; i < WORLD_WIDTH; i++)
		world[i][0] = wall;
	for (size_t i = 0; i < WORLD_WIDTH; i++)
		world[i][181] = wall;
	
	//Отмечаем пустоту
	for (size_t i = 0; i < WORLD_WIDTH; i++)
		for (size_t j = 1; j < WORLD_HEIGHT+1; j++)
			world[i][j] = empty;


	bot* a = new bot(MAX_X / 2, std::lround(MAX_Y / ((double)2.0003)));
	season = 11;

	work = true;
	while (work) {
		for (size_t i = 0; i < bots.size(); i++) {
			bots[i].step();
			gui::checkEvents();
		}
		radiation();
		gui::draw();
		
		gui::updateStats();
	}
	gui::close();
	return 0;
}
