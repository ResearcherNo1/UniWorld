//Файл реализации ботов
//
#include "constants.h"
#include <ctime>
#include <vector>
#include <string>
#undef NDEBUG
#include <cassert>
#include <csignal>
#define NDEBUG

#ifndef BOT_H
#define BOT_H

class bot {
public:	
	         int    energy = 0;              //Кол-во энергии
			 int    minrNum = 0;             //Кол-во накопленных минералов
			 short  decompose = 0;           //Счётчик разложения
	         cond   condition = cond::alive; //Состояние
	         	    
	         short  DNA[DNA_SIZE];           //Геном
	unsigned int    IP = 0;                  //Указатель текущей инструкции

	         short  stack[SUB_SIZE];         //Стек
	unsigned short  heapPtr = 0;             //Указатель верхушки стека

	         short  registers[SUB_SIZE];     //Регистры

	         int    coorX = 0;               //Координата X
	         int    coorY = 1;               //Координата Y
	         drct   direct = drct::up;       //Текущее направление

	    long long   chainPrev = -1;          //Cсылка на предыдущего бота в цепочке
	    long long   chainNext = -1;          //Cсылка на следующего  бота в цепочке
	         size_t n = 0;                   //Итератор в векторе bots 

	         short  red = 0;                 //       красного
	         short  green = 0;               //Кол-во зелёного
	         short  blue = 0;                //       синего

	         int    born = 0;      //Критический порог энергии для рождения потомка

	bot(const unsigned int X, const unsigned int Y, bot* parent = nullptr, size_t N = 0, const bool free = true);
	void death(); //Процедура смерти

	void incIP(unsigned int num = 1); //Функция увeличения указателя текущей команды
	unsigned short getParam();        //Функция получения параметра

	unsigned int getX(unsigned short direct); //Получение Х-координаты по направлению
	unsigned int getY(unsigned short direct); //Получение Y-координаты по направлению

	bool isRelative(bot _bot);                //Определяет, родственник ли _bot

	void goRed  (short power);
	void goGreen(short power);
	void goBlue (short power);
	void print(std::string a);

	void step(); //Главная функция жизнeдеятельности
};

void radiation();

#endif //BOT_H