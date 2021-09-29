//Файл основных констант
//
#include <iostream>

#ifndef CONSTANTS_H
#define CONSTANTS_H

//Global constants
constexpr auto SCREEN_WIDTH  = 1266;
constexpr auto SCREEN_HEIGHT = 766-150;
constexpr auto WORLD_WIDTH = SCREEN_WIDTH / 4;
constexpr auto WORLD_HEIGHT = SCREEN_HEIGHT / 4;
constexpr auto BOTS_MAX = WORLD_WIDTH * WORLD_HEIGHT;
constexpr auto SAVE_STEP = 250;

//Bot constants
constexpr auto DNA_SIZE = 256;
constexpr auto DNA_MAX_INDEX = DNA_SIZE - 1;
constexpr auto MAX_CYC = 32;
constexpr auto SUB_SIZE = 8;
constexpr auto SUB_MAX_INDEX = SUB_SIZE - 1;

typedef unsigned short cond;
constexpr cond organic = 1; 
constexpr cond alive = 2;
constexpr cond input = 3;

typedef unsigned short drct;
constexpr drct upper_left = 0;
constexpr drct up = 1;
constexpr drct upper_right = 2;
constexpr drct right = 3;
constexpr drct bottom_right = 4;
constexpr drct down = 5;
constexpr drct bottom_left = 6;
constexpr drct left = 7;

constexpr double HEIGHT_COEF = WORLD_HEIGHT / DNA_SIZE;
constexpr double ENERGY_COEF = ((double)999) / DNA_SIZE;
constexpr auto FREE = true;
constexpr auto CHAIN = false;
constexpr auto DECOMPOSE_TIME = 2;
constexpr auto MUT_COEF = 32;


//Navigation constants
typedef unsigned long long id_t;
constexpr id_t empty = 0;
constexpr id_t wall  = -1;
constexpr size_t MAX_X = WORLD_WIDTH - 1;
constexpr size_t MAX_Y = WORLD_HEIGHT;

/* Получить текущее время для генерации рандомайза.*/
#define srand(); srand(time(NULL));

//Функция выбора случайного числа в выбраном диапазоне
//
//Перед запуском вызвать srand();
template <class T>
inline T getRandomNumber(T min, T max) {
	
	if (min >= max)
		std::cout << std::endl << __LINE__ << "|" << "	getRandomNumber():Максимальный порог диапазона равен или больше минимального" << std::endl;

	constexpr long double fraction = 1.0 / (long double)RAND_MAX;
	//Равномерно распределяем рандомное число в нашем диапазоне
	return static_cast<T>(rand() * fraction * max + min);
}

#endif //CONSTANTS_H