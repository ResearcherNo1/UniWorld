#include "constants.h"
#include "bot.h"
#include "graphics.h"
#include "gFunc.h"
#include <Windows.h>

long long world[WORLD_WIDTH][((unsigned long long)WORLD_HEIGHT+2)]; //Массив мира
std::pmr::vector<bot> bots;	//Вектор ботов
int season; //9 - зима; 10 - весна, осень; 11 - лето
bool work;
bool pause;
uint_fast64_t lifeCount = 0; //Счётчик ходов
uint_fast64_t EnterlifeCount = 0;
bool paintMode; //Режим отображения

gui::LWindow gWindow;; //The window we'll be rendering to
SDL_Renderer* gRenderer = NULL; //The window renderer

//Scene sprites
SDL_Rect gSpriteClips[2];
gui::LTexture gSpriteBot;
gui::LTexture gSpriteOrganic;


int _cdecl main(int argc, char* args[]) {
	gInit();
	bot* a = new bot(MAX_X / 2, std::lround(MAX_Y / ((double)2.0003)));

	while (work) {
		if (!pause)
			gStep();
		else {
			if (EnterlifeCount > lifeCount)
				gStep();

			gui::checkEvents();
			Sleep(100);
		}
	}
	gui::close();
	return 0;
}
