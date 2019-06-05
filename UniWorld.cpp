#include <iostream>
#include "bot.h"

int world[WORLD_WIDTH][WORLD_HEIGHT]; //Массив мира
std::vector<bot> bots;	//Вектор ботов
int season; //9 - зима; 10 - весна, осень; 11 - лето

int _cdecl main(int argc, char* args[]) {
	std::vector<int> vec;
	for (int i = 0; i < 10; ++i)
		vec.push_back(i);
	int *ptr = &vec[5];
	int *front = &vec[0];
	std::cout << vec.max_size() << std::endl;
	return 0;
}

