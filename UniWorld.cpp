//Точка входа
//

#include "src/constants.h"
#include "src/bot.h"
#include "src/graphics.h"
#include "src/gFunc.h"
#include <thread>
#include <chrono>

abstractWorld<id_t> world; //Массив мира
std::map<id_t, bot*> bots;	//Вектор ботов
int season; //9 - зима; 10 - весна, осень; 11 - лето
bool work;
bool pause;
uint64_t lifeCount = 0; //Счётчик ходов
uint64_t EnterlifeCount = 0;
bool paintMode; //Режим отображения

gui::LWindow gWindow; //The window we'll be rendering to
SDL_Renderer* gRenderer = NULL; //The window renderer

//Scene sprites
SDL_Rect gSpriteClips[2];
gui::LTexture gSpriteBot;
gui::LTexture gSpriteOrganic;

extern "C" 
int _cdecl main(int argc, char* argv[]) {
	if (argc > 1)
		gUnpack(argv[1]);
	else
		gInit();

	while (work) {
		if (!pause)
			gStep();
		else {
			if (EnterlifeCount > lifeCount)
				gStep();

			gui::checkEvents();
		}

		if (lifeCount % SAVE_STEP == 0)
			gSave("simulation.bin");
	}

	gui::close();
	return 0;
}
