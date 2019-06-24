//Файл основных констант
//
#include <iostream>

#ifndef CONSTANTS_H
#define CONSTANTS_H

//Global constants
constexpr auto SCREEN_WIDTH  = 1280;
constexpr auto SCREEN_HEIGHT = 720;
constexpr auto WORLD_WIDTH = SCREEN_WIDTH / 4;
constexpr double WORLD_HEIGHT = SCREEN_HEIGHT / 4;
constexpr auto BOTS_MAX = WORLD_WIDTH * WORLD_HEIGHT;

//Bot constants
constexpr auto DNA_SIZE = 256;
constexpr auto DNA_MAX_INDEX = DNA_SIZE - 1;
constexpr auto SUB_SIZE = 8;
constexpr auto SUB_MAX_INDEX = SUB_SIZE - 1;
enum cond {organic_hold = 1, organic_sink, alive, input };
enum drct : unsigned short { upper_left, up = 1, upper_right, right = 3, bottom_right, down = 5, bottom_left, left = 7 };
constexpr double HEIGHT_COEF = WORLD_HEIGHT / DNA_SIZE;
constexpr double ENERGY_COEF = ((double)999) / DNA_SIZE;
constexpr auto FREE = true;
constexpr auto CHAIN = false;
constexpr auto DECOMPOSE_TIME = 50;
constexpr auto MUT_COEF = 32;


//Navigation constants
constexpr auto empty = -1;
constexpr long wall = static_cast<long>(BOTS_MAX + 10000);
constexpr auto MAX_X = WORLD_WIDTH - 1;
constexpr auto MAX_Y = WORLD_HEIGHT - 1;

#define srand(); 	/* Получить текущее время для генерации рандомайза.*/ /* ! Лучше делать с миллисекундами */ struct tm *st; const time_t timer = time(NULL); st = localtime(&timer); /* Перевод времени в секунды */ int rnd_seed = 3600 * st->tm_hour + 60 * st->tm_min + st->tm_sec; srand(rnd_seed);

//Функция выбора случайного числа в выбраном диапазоне
template <class T>
inline T getRandomNumber(T min, T max) {
	//Перед запуском вызвать srand();

	if (min < 0) //Создание исключения при маленьком минимальном числе
	{
		std::cout << std::endl << __LINE__ << "|" << "	getRandomNumber():Слишком маленький низкий порог диапазона" << std::endl;
		system("exit");
	}

	if (max > RAND_MAX) //Создание исключения при превышении RAND_MAX
	{
		std::cout << std::endl << __LINE__ << "|" << "	getRandomNumber():Превышен максимальный порог диапазона" << std::endl;
		system("exit");
	}

	if (min >= max)
	{
		std::cout << std::endl << __LINE__ << "|" << "	getRandomNumber():Максимальный порог диапазона равен или больше минимального" << std::endl;
		system("exit");
	}

	static const long double fraction = 1.0 / (static_cast<long double>(RAND_MAX) + 1.0);
	//Равномерно распределяем рандомное число в нашем диапазоне
	return static_cast<T>(rand() * fraction * (max - min + 1) + min);
}

#endif //CONSTANTS_H