#include "gFunc.h"

extern bool work;
extern bool pause;
extern bool paintMode;
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
	if (pause && bots.size() >= 100)
		showPercent = true;

	for (size_t i = 0; i < bots.size(); i++) {
		bots[i].step();
		gui::checkEvents();
		if (showPercent) {
			double percent = (long double(i) / bots.size()) * ((long double)100);
			char buffer[27];
			sprintf_s(buffer, 27, "UniWorld - Paused (%.1f%%)", percent);
			gWindow.rename(buffer);
		}
	}
	radiation();
	gui::draw();

	gui::updateStats();
}