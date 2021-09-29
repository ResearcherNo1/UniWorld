#include "gFunc.h"

extern bool work;
extern bool pause;
extern uint64_t lifeCount;
extern bool paintMode;
extern int season;
extern gui::LWindow gWindow;

extern abstractWorld<id_t> world;
extern std::map<id_t, bot*> bots;
std::vector<id_t> deleted;
id_t currentId = 1;

void gInit() {
	gui::initialize();
	printf("GUI has been initialized. Press any key to continue...");
	if (_getch() == 27) return;

	world = abstractWorld<id_t>(WORLD_WIDTH, WORLD_HEIGHT + 2, empty);

	//Ставим стены
	for (size_t i = 0; i < WORLD_WIDTH; i++)
		world[i][0] = wall;
	for (size_t i = 0; i < WORLD_WIDTH; i++)
		world[i][WORLD_HEIGHT + 1] = wall;

	season = 11;
	work = true;
	pause = false;
	paintMode = true;
	
	srand();

	id_t tempId = currentId++;
	bots[tempId] = new bot(getRandomNumber<size_t>(0, MAX_X), getRandomNumber<size_t>(1, MAX_Y), nullptr, tempId, true);
}

void gUnpack(std::string fname) {
	gui::initialize();
	printf("GUI has been initialized. Press any key to continue...");
	if (_getch() == 27) return;

	std::ifstream fin(fname, std::ios::binary | std::ios::in);
	auto din = nvx::archive<std::ifstream>(&fin);

	size_t botsCount;
	din >> &botsCount;

	din >> &currentId;
	din >> &season;
	din >> &lifeCount;
	din >> &world;

	for (size_t i = 0; i < botsCount; i++) {
		id_t id; bot bbuf;
		din >> &id; din >> &bbuf;
		bots[id] = new bot(bbuf);
	}

	fin.close();

	work = true;
	pause = false;
	paintMode = true;
}

void gSave(std::string fname) {
	std::ofstream fout(fname, std::ios::binary | std::ios::trunc | std::ios::out);
	auto sout = nvx::archive<std::ofstream>(&fout);

	size_t botsCount = bots.size();
	sout << &botsCount;

	sout << &currentId << &season << &lifeCount << &world;
	for (auto pair : bots) {
		auto id = std::get<0>(pair);
		auto bot = std::get<1>(pair);
		sout << &id;
		sout << bot;
	}

	fout.close();
}

void gStep() {
	bool showPercent = false;
	if (pause && bots.size() >= 100)
		showPercent = true;

	long double i = 0.l;
	for (auto pair : bots) {
		auto bot = std::get<1>(pair);

		bot->step();
		gui::checkEvents();
		if (showPercent) {
			double percent = (i++ / bots.size()) * ((long double)100);
			char buffer[27];
			sprintf_s(buffer, 27, "UniWorld - Paused (%.1f%%)", percent);
			gWindow.rename(buffer);
		}
	}
	for (auto key : deleted) {
		delete bots[key];
		bots.erase(key);
	}
	deleted.clear();

	radiation();
	gui::draw();

	gui::updateStats();
}