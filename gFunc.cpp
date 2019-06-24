#include "gFunc.h"

extern bool work;
extern bool pause;
extern bool paintMode;
extern uint_fast64_t lifeCount;
extern uint_fast64_t EnterlifeCount;
extern int season;
extern gui::LWindow gWindow;

extern long long world[WORLD_WIDTH][((unsigned long long)WORLD_HEIGHT + 2)];
extern std::pmr::vector<bot> bots;

void gInit() {
	gui::initialize();
	printf("GUI has been initialized. Press any key to continue...");
	_getch();

	//Ставим стены
	for (size_t i = 0; i < WORLD_WIDTH; i++)
		world[i][0] = wall;
	for (size_t i = 0; i < WORLD_WIDTH; i++)
		world[i][((unsigned long long(WORLD_HEIGHT + 1)))] = wall;

	//Отмечаем пустоту
	for (size_t i = 0; i < WORLD_WIDTH; i++)
		for (size_t j = 1; j < WORLD_HEIGHT + 1; j++)
			world[i][j] = empty;

	season = 11;
	work = true;
	pause = false;
	paintMode = true;
}

void gStep() {
	bool showPercent = false;
	if (pause && bots.size() >= 1000)
		showPercent = true;

	for (long double i = 0; i < bots.size(); i++) {
		bots[std::lroundl(i)].step();
		gui::checkEvents();
		if (showPercent) {
			double percent = (i / bots.size()) * ((double)100);
			char buffer[27];
			sprintf_s(buffer, 27, "UniWorld - Paused (%.1f%%)", percent);
			gWindow.rename(buffer);
		}
	}
	radiation();
	gui::draw();

	gui::updateStats();
}