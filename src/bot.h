//Файл реализации ботов
//
#include "constants.h"
#include "world.hpp"
#include "serialize.hpp"
#include <map>
#include <string>

#ifdef NDEBUG
	#undef NDEBUG
	#include <cassert>
	#include <csignal>
	#define NDEBUG
#else
	#include <cassert>
	#include <csignal>
#endif


#ifndef BOT_H
#define BOT_H

class bot {
public:	
	         int    energy = 0;              //Кол-во энергии
			 int    minrNum = 0;             //Кол-во накопленных минералов
			 short  decompose = 0;           //Счётчик разложения
	         cond   condition = alive;       //Состояние
	         	    
	         short  DNA[DNA_SIZE];           //Геном
	unsigned int    IP = 0;                  //Указатель текущей инструкции

	         short  stack[SUB_SIZE];         //Стек
	unsigned short  heapPtr = 0;             //Указатель верхушки стека

	         short  registers[SUB_SIZE];     //Регистры

	         int    coorX = 0;               //Координата X
	         int    coorY = 1;               //Координата Y
	         drct   direct = up;             //Текущее направление

	         id_t   chainPrev = -1;          //Cсылка на предыдущего бота в цепочке
	         id_t   chainNext = -1;          //Cсылка на следующего  бота в цепочке
			 id_t   ownId = 1;               //Собственный ID бота

	         short  red = 0;                 //       красного
	         short  green = 0;               //Кол-во зелёного
	         short  blue = 0;                //       синего

	         int    born = 0;                //Критический порог энергии для рождения потомка

	NVX_SERIALIZABLE(&energy, &minrNum, &decompose, &condition,
	                 NVX_SERIALIZABLE_STATIC_ARRAY(DNA, DNA_SIZE), &IP,
	                 NVX_SERIALIZABLE_STATIC_ARRAY(stack, SUB_SIZE), &heapPtr,
	                 NVX_SERIALIZABLE_STATIC_ARRAY(registers, SUB_SIZE),
	                 &coorX, &coorY, &direct,
	                 &chainPrev, &chainNext, &ownId,
	                 &red, &green, &blue,
	                 &born);

	bot() = default;
	bot(const unsigned int X, const unsigned int Y, bot* parent = nullptr, const id_t N = 0, const bool free = true);
	void death(); //Процедура смерти
	
	void incIP(unsigned int num = 1); //Функция увeличения указателя текущей команды
	short getParam();                 //Функция получения параметра
	
	unsigned int getX(unsigned short direct); //Получение Х-координаты по направлению
	unsigned int getY(unsigned short direct); //Получение Y-координаты по направлению
	
	bool isRelative(bot _bot);                //Определяет, родственник ли _bot
	bool isRelative(bot* _bot);
	
	void goRed  (short power);
	void goGreen(short power);
	void goBlue (short power);
	void print(std::string a);
	
	void step(); //Главная функция жизнeдеятельности
};

void radiation();

#endif //BOT_H