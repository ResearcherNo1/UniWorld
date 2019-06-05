//Файл реализации ботов
//
#include "constants.h"
#include <ctime>
#include <vector>
#ifndef BOT_H
#define BOT_H

class bot {
public:
	         int    energy; //Кол-во энергии
	unsigned short  DNA[DNA_SIZE]; //Геном
	unsigned int    IP; //Указатель текущей инструкции

	unsigned short  stack[SUB_SIZE]; //Стек
	unsigned short  heapPtr;  //Указатель верхушки стека

	unsigned short  registers[SUB_SIZE]; //Регистры

	unsigned int    coorX;     //Координата X
	unsigned int    coorY;     //Координата Y
	         int    minrNum;   //Кол-во накопленных минералов
	         cond   condition; //Состояние

	unsigned short  red;       //       красного
	unsigned short  green;     //Кол-во зелёного
	unsigned short  blue;      //       синего
	         drct   direct;    //Текущее направление
	         int    chainPrev; //Cсылка на предыдущего бота в цепочке
	         int    chainNext; //Cсылка на следующего  бота в цепочке
	         size_t n;         //Итератор в векторе bots 

	bot(unsigned int X, unsigned int Y, bot* parent = nullptr, size_t N = 0, bool free = true);
	void incIP(unsigned int num); //Функция увeличения указателя текущей команды
	unsigned int getX(unsigned short n); //Получение Х-координаты по направлению
	unsigned int getY(unsigned short n); //Получение Y-координаты по направлению
	bool isRelative(bot _bot); //Определяет, родственник ли _bot

	void step(); //Главная функция жизнидеятельности
	~bot();
};

#endif //BOT_H