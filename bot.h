//Файл реализации ботов
//
#include "constants.h"
#include <ctime>
#include <vector>
#include <algorithm> 

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

	         short  red;       //       красного
	         short  green;     //Кол-во зелёного
	         short  blue;      //       синего
	         drct   direct;    //Текущее направление
				size_t chainPrev; //Cсылка на предыдущего бота в цепочке
				size_t chainNext; //Cсылка на следующего  бота в цепочке
	         size_t n;         //Итератор в векторе bots 

	bot(unsigned int X, unsigned int Y, bot* parent = nullptr, size_t N = 0, bool free = true);

	void incIP(unsigned int num = 1); //Функция увeличения указателя текущей команды
	unsigned short getParam(); //Функция получения параметра

	unsigned int getX(unsigned short n); //Получение Х-координаты по направлению
	unsigned int getY(unsigned short n); //Получение Y-координаты по направлению
	bool isRelative(bot _bot); //Определяет, родственник ли _bot

	void goRed  (short n);
	void goGreen(short n);
	void goBlue (short n);
	 
	void step(); //Главная функция жизнидеятельности
	~bot();
};

#endif //BOT_H