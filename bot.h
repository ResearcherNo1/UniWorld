//Файл реализации ботов
//
#include "constants.h"
#include <ctime>
#include <vector>
#include <string>

#ifndef BOT_H
#define BOT_H

class bot {
public:
	         int    energy;    //Кол-во энергии
				int    minrNum;   //Кол-во накопленных минералов
				short  decompose; //Счётчик разложения
				cond   condition; //Состояние

				short  DNA[DNA_SIZE];   //Геном
	unsigned int    IP;              //Указатель текущей инструкции

	         short  stack[SUB_SIZE]; //Стек
	unsigned short  heapPtr;         //Указатель верхушки стека

	         short  registers[SUB_SIZE]; //Регистры

	unsigned int    coorX;     //Координата X
	unsigned int    coorY;     //Координата Y
				drct   direct;    //Текущее направление

	 	 long long   chainPrev; //Cсылка на предыдущего бота в цепочке
		 long long   chainNext; //Cсылка на следующего  бота в цепочке
				size_t n;         //Итератор в векторе bots 

	         short  red;       //       красного
	         short  green;     //Кол-во зелёного
	         short  blue;      //       синего

				int    born = 0;      //Критический порог энергии для рождения потомка

	bot(unsigned int X, unsigned int Y, bot* parent = nullptr, size_t N = 0, bool free = true);
	void death(); //Процедура смерти

	void incIP(unsigned int num = 1); //Функция увeличения указателя текущей команды
	unsigned short getParam();        //Функция получения параметра

	unsigned int getX(unsigned short n); //Получение Х-координаты по направлению
	unsigned int getY(unsigned short n); //Получение Y-координаты по направлению
	bool isRelative(bot _bot);           //Определяет, родственник ли _bot

	void goRed  (short n);
	void goGreen(short n);
	void goBlue (short n);
	void print(std::string a);

	void step(); //Главная функция жизнeдеятельности
};

void radiation();

#endif //BOT_H