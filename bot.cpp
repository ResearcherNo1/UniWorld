//Файл основной реализации ботов
//
#include "bot.h"

extern int world[WORLD_WIDTH][WORLD_HEIGHT];
extern std::vector<bot> bots;
extern int season;

bot::bot(unsigned int X, unsigned int Y, bot* parent, size_t n) {
	srand();
	//Если нет родителей
	if (parent == nullptr) {
		for (size_t i = 0; i < DNA_SIZE; i++) //Заполняем код командой фотосинтез
			DNA[i] = 25;
		energy = 990;
		minrNum = 0;
		chainNext = chainPrev = 0;
		red = green = blue = 170;
	}
	else {
		//В одном случае из четырёх
		if (getRandomNumber(1, 4) == 2) {
			//Вычисляем указатель мутации
			size_t mut_ptr = getRandomNumber(0, 248);
			//Копируем геном родителя
			for (size_t i = 0; i < DNA_SIZE; i++)
				DNA[i] = parent->DNA[i];
			//Мутируем
			for (mut_ptr; mut_ptr < mut_ptr + 8; mut_ptr++)
				DNA[mut_ptr] = getRandomNumber(0, 255);
		}
		energy = (parent->energy) / 2;
		minrNum = (parent->minrNum) / 2;
		red = parent->red;
		green = parent->green;
		blue = parent->blue;
	}
	//Указатель инструкции в самом начале
	IP = 0;

	//Очищаем стек и регистры
	for (size_t i = 0; i < SUB_SIZE; i++) {
		stack[i] = 0;
		registers[i] = 0;
	}
	heapPtr = 0;

	coorX = X; coorY = Y;
	condition = alive;
	direct = down;
	std::vector<bot>::iterator a = bots.begin() + n;
	bots.insert(a, *this);
	world[X][Y] = n;
}

void bot::incIP(unsigned int num) {
	IP += num;
	if (IP > DNA_MAX_INDEX)
		IP -= DNA_SIZE;
}

unsigned int bot::getX(unsigned short n) {
	unsigned int x = coorX;
	if (n > 7)
		n -= 8;

	if (n == 0 || n == 6 || n == 7) {
		x--;
		if (x < 0)
			x = MAX_X;
	}
	else if (n == 2 || n == 3 || n == 4) {
		x++;
		if (x > MAX_X)
			x = 0;
	}
	return x;
}

unsigned int bot::getY(unsigned short n) {
	unsigned int y = coorY;
	if (n > 7)
		n -= 8;

	if (n < 3)
		y--;
	else if (n == 4 || n == 5 || n == 6)
		y++;
	return y;
}

bool bot::isRelative(bot _bot) {
	if (_bot.condition < alive) //Если _bot - органика
		return false;

	size_t dif = 0; //Cчётчик несовпадений в геноме
	for (size_t i = 0; i < DNA_SIZE; i++) //Поячеечно сверяем геномы
		if (DNA[i] != _bot.DNA[i]) {
			dif++;
			if (dif > 8) return false; //Если несовпадений > 8, то боты не родственники
		}
	return true;
}

void bot::step() {
	//Если бот - органика, то выходим из функции
	if (condition = organic_hold)
		return;
	if (condition = organic_sink) { //Падение органики
		if (world[coorX][coorY + 1] == empty) {
			world[coorX][coorY] = empty;
			world[coorX][coorY + 1] = n;
			coorY++;
		}
		else
			condition = organic_hold;
		return;
	}

	//
	//Если мы здесь, то бот живой
	//

	for (unsigned short cyc = 0; cyc < 15; cyc++) { //Разрешено не более 15 команд за ход
		unsigned short command = DNA[IP]; //Текущая команда

		//Относительная смена направления
		if (command == 23) {
			unsigned short param;
			if (IP = 255) {
				IP = 0;
				param = (DNA[IP] % 16);
			}
			else {
				param = (DNA[++IP] % 16); //Считываем следующий за командой байт и вычисляем остаток от деления на 16
				param += direct; //Полученное число прибавляем к значению направления бота
			}

			if (param > 7) //Результат должен быть в пределах 0 ... 7
				param - 8;
			direct = static_cast<drct>(param);
			incIP(1);
		}

		//Абсолютная смена направления
		else if (command == 24) {
			unsigned short param;
			if (IP = 255) {
				IP = 0;
				param = (DNA[IP] % 16);
			}
			else
				param = (DNA[++IP] % 16); //Считываем следующий за командой байт и вычисляем остаток от деления на 16

			direct = static_cast<drct>(param);
			incIP(1);
		}

		//Фотосинтез
		else if (command == 25) {
			double t = 0;
			//Вычисление коофэициента минералов
			if (minrNum < 100)
				t = 0;
			else {
				if (minrNum < 400)
					t = 1;
				else
					t = 2;
			}
			double hlt = season - ((coorY + 1) % 6) + t; //Формула вычисления энергии
			if (hlt > 0) {
				energy += hlt; //Прибавляем полученную энергия к энергии бота
				goGreen(hlt); //Бот от этого зеленеет
			}
			incIP(1);
			break; //Выходим, так как команда "фотосинтез" - завершающая
		}

		/*Шаг в относительном направлении
		   Если
		    удалось перейти +2
			 стена           +3
			 органика        +4
			 бот             +5
			 родня           +6
		*/
		else if (command == 26) {
			//Если бот многоклеточный
			if (chainPrev > 0 || chainNext > 0)
				//Никуда не двигаемся - ходить могут только одноклеточные
				incIP(1);
			else {
				unsigned short param;
				if (IP = 255) {
					IP = 0;
					param = (DNA[IP] % 16);
				}
				else
					param = (DNA[++IP] % 16); //Считываем следующий за командой байт и вычисляем остаток от деления на 16

				unsigned int x = getX(param + direct); //Получаем координаты клетки
				unsigned int y = getY(param + direct); //

				if (world[x][y] == empty) { //Если на клетке пусто
					world[x][y] = n;
					world[coorX][coorY] = empty;
					coorX = x;
					coorY = y;
					incIP(2);
				}
				else if (world[x][y] == wall) { //Если на клетке стена
					incIP(3);
				}
				else if (bots[world[x][y]].condition <= organic_sink) { //Если на клетке органика
					incIP(4);
				}
				else if (isRelative(bots[world[x][y]])) { //Если на клетке родня
					incIP(6);
				}
				else { //Оставшийся вариант - на клетке бот
					incIP(5);
				}
			}
			break; //Выходим, так как команда "Шаг" - завершающая
		} //

		/*Шаг в абсолютном направлении
		   Если
			 удалось перейти +2
			 стена           +3
			 органика        +4
			 бот             +5
			 родня           +6
		*/
		else if (command == 27) {
			//Если бот многоклеточный
			if (chainPrev > 0 || chainNext > 0)
				//Никуда не двигаемся - ходить могут только одноклеточные
				incIP(1);
			else {
				unsigned short param;
				if (IP = 255) {
					IP = 0;
					param = (DNA[IP] % 16);
				}
				else
					param = (DNA[++IP] % 16); //Считываем следующий за командой байт и вычисляем остаток от деления на 16

				unsigned int x = getX(param); //Получаем координаты клетки
				unsigned int y = getY(param); //

				if (world[x][y] == empty) { //Если на клетке пусто
					world[x][y] = n;
					world[coorX][coorY] = empty;
					coorX = x;
					coorY = y;
					incIP(2);
				}
				else if (world[x][y] == wall) { //Если на клетке стена
					incIP(3);
				}
				else if (bots[world[x][y]].condition <= organic_sink) { //Если на клетке органика
					incIP(4);
				}
				else if (isRelative(bots[world[x][y]])) { //Если на клетке родня
					incIP(6);
				}
				else { //Оставшийся вариант - на клетке бот
					incIP(5);
				}
			}
			break;

		}

		/*Cъесть в относительном направлении
			Если
			 пусто           +2
			 стена           +3
			 органика        +4
			 бот             +5
		*/
		else if (command == 28) {
			unsigned short param;
			if (IP = 255) {
				IP = 0;
				param = (DNA[IP] % 16);
			}
			else
				param = (DNA[++IP] % 16); //Считываем следующий за командой байт и вычисляем остаток от деления на 16

			energy -= 4; //Бот на этом этапе, независимо от результата, теряет 4 энергии
			unsigned int x = getX(param + direct); //Получаем координаты клетки
			unsigned int y = getY(param + direct); //


			if (world[x][y] == empty) { //Если пусто
				incIP(2);
			}
			else if (world[x][y] == wall) { //Если стена
				incIP(3);
			}
			else if (bots[world[x][y]].condition <= organic_sink) { //Если там оказалась органика
				bots[world[x][y]].~bot(); //Удаляем жертву
				energy += 100;            //Прибавка к энергии
				goRed(100);               //Бот краснеет
				incIP(4);
			}
			else {
				//
				//--------- дошли до сюда, значит впереди живой бот -------------------
				//
				unsigned int min0 = minrNum; //Кол-во минералов у бота
				unsigned int min1 = bots[world[x][y]].minrNum; //Кол-во минералов у потенциального обеда
				int hl = bots[world[x][y]].energy;  //Кол-во энергии

				//Если у бота минералов больше
				if (min0 >= min1) {
					minrNum -= min1;  //Количество минералов у бота уменьшается на количество минералов у жертвы
													  //Типа, стесал свои зубы о панцирь жертвы)
					bots[world[x][y]].~bot(); //Удаляем жертву
					energy += 100 + (hl / 2); //Kоличество энергии у бота прибавляется на 100 + (половина от энергии жертвы)
					goRed(hl);                //Бот краснеет
					incIP(5);
				}
				//Если обед более бронированый
				else if (min1 > min0) {
					minrNum = 0;  //То бот израсходовал все минералы на преодоление защиты
					min1 -= min0; //Но и у обеда они уменьшились
					//Если энергии в 2 раза больше
					if (energy >= 2 * min1) {
						bots[world[x][y]].~bot(); //Удаляем жертву
						energy += 100 + (hl / 2) - 2 * min1;
						goRed(hl);                //Бот краснеет
						incIP(5);
					}
					//Если энергии меньше, чем (минералов у жертвы)*2, то бот погибает от жертвы 
					else {
						bots[world[x][y]].minrNum = min1 - (energy / 2); //Жертва истрачивает свои минералы
						energy = -10; //Энергия уходит в минус
						incIP(5);
					}
				}
			}
			break;
		}

		/*Cъесть в абсолютном направлении
			Если
			 пусто           +2
			 стена           +3
			 органика        +4
			 бот             +5
		*/
		else if (command == 29) {
			unsigned short param;
			if (IP = 255) {
				IP = 0;
				param = (DNA[IP] % 16);
			}
			else
				param = (DNA[++IP] % 16); //Считываем следующий за командой байт и вычисляем остаток от деления на 16

			energy -= 4; //Бот на этом этапе, независимо от результата, теряет 4 энергии
			unsigned int x = getX(param); //Получаем координаты клетки
			unsigned int y = getY(param); //


			if (world[x][y] == empty) { //Если пусто
				incIP(2);
			}
			else if (world[x][y] == wall) { //Если стена
				incIP(3);
			}
			else if (bots[world[x][y]].condition <= organic_sink) { //Если там оказалась органика
				bots[world[x][y]].~bot(); //Удаляем жертву
				energy += 100;            //Прибавка к энергии
				goRed(100);               //Бот краснеет
				incIP(4);
			}
			else {
				//
				//--------- дошли до сюда, значит впереди живой бот -------------------
				//
				unsigned int min0 = minrNum; //Кол-во минералов у бота
				unsigned int min1 = bots[world[x][y]].minrNum; //Кол-во минералов у потенциального обеда
				int hl = bots[world[x][y]].energy;  //Кол-во энергии

				//Если у бота минералов больше
				if (min0 >= min1) {
					minrNum -= min1;  //Количество минералов у бота уменьшается на количество минералов у жертвы
													  //Типа, стесал свои зубы о панцирь жертвы)
					bots[world[x][y]].~bot(); //Удаляем жертву
					energy += 100 + (hl / 2); //Kоличество энергии у бота прибавляется на 100 + (половина от энергии жертвы)
					goRed(hl);                //Бот краснеет
					incIP(5);
				}
				//Если обед более бронированый
				else if (min1 > min0) {
					minrNum = 0;  //То бот израсходовал все минералы на преодоление защиты
					min1 -= min0; //Но и у обеда они уменьшились
					//Если энергии в 2 раза больше
					if (energy >= 2 * min1) {
						bots[world[x][y]].~bot(); //Удаляем жертву
						energy += 100 + (hl / 2) - 2 * min1;
						goRed(hl);                //Бот краснеет
						incIP(5);
					}
					//Если энергии меньше, чем (минералов у жертвы)*2, то бот погибает от жертвы 
					else {
						bots[world[x][y]].minrNum = min1 - (energy / 2); //Жертва истрачивает свои минералы
						energy = -10; //Энергия уходит в минус
						incIP(5);
					}
				}
			}
			break;
		}

		/*Посмотреть в относительном направлении
		   Если
			 пусто           +2
			 стена           +3
			 органика        +4
			 бот             +5
			 родня           +6
		*/
		else if (command == 30) {
			unsigned short param;
			if (IP = 255) {
				IP = 0;
				param = (DNA[IP] % 16);
			}
			else
				param = (DNA[++IP] % 16); //Считываем следующий за командой байт и вычисляем остаток от деления на 16

			unsigned int x = getX(param + direct); //Вычисляем координаты клетки
			unsigned int y = getY(param + direct); //
			int wc = world[x][y]; //Определяем объект

			if (wc == empty) { //Если пусто
				incIP(2);
			}
			else if (wc == wall) { //Если стена
				incIP(3);
			}
			else if (bots[wc].condition <= organic_sink) { //Если органика
				incIP(4);
			}
			else if (isRelative(bots[wc])) { //Если родня
				incIP(6);
			}
			else { //Если бот
				incIP(5);
			}
		}

		/*Посмотреть в абсолютном направлении
			Если
			 пусто           +2
			 стена           +3
			 органика        +4
			 бот             +5
			 родня           +6
		*/
		else if (command == 31) {
			unsigned short param;
			if (IP = 255) {
				IP = 0;
				param = (DNA[IP] % 16);
			}
			else
				param = (DNA[++IP] % 16); //Считываем следующий за командой байт и вычисляем остаток от деления на 16

			unsigned int x = getX(param); //Вычисляем координаты клетки
			unsigned int y = getY(param); //
			int wc = world[x][y]; //Определяем объект

			if (wc == empty) { //Если пусто
				incIP(2);
			}
			else if (wc == wall) { //Если стена
				incIP(3);
			}
			else if (bots[wc].condition <= organic_sink) { //Если органика
				incIP(4);
			}
			else if (isRelative(bots[wc])) { //Если родня
				incIP(6);
			}
			else { //Если бот
				incIP(5);
			}
		}

		/*Делится в относительном направлении
		   Если
			 стена           +2
			 пусто           +3
			 органика        +4
			 успешно         +5
		*/
		else if (command == 32 || command == 42) { //Шансы появления этой команды увеличены
			unsigned short param;
			if (IP = 255) {
				IP = 0;
				param = (DNA[IP] % 16);
			}
			else
				param = (DNA[++IP] % 16); //Считываем следующий за командой байт и вычисляем остаток от деления на 16

			unsigned int x = getX(param + direct); //Вычисляем координаты клетки
			unsigned int y = getY(param + direct); //
			

			if (world[x][y] == empty) //Если пусто
				incIP(2);
			else if (world[x][y] == wall)  //Если стена
				incIP(3);
			else if (bots[world[x][y]].condition <= organic_sink) //Если органика
				incIP(4);
			else { //Остался один вариант - впереди живой бот
				int hlt0 = energy;                   // определим количество энергии и минералов
				int hlt1 = bots[world[x][y]].energy; // у бота и его соседа
				int min0 = minrNum;
				int min1 = bots[world[x][y]].minrNum;

				if (hlt0 > hlt1) //Eсли у бота больше энергии, чем у соседа
				{
					int hlt = (hlt0 - hlt1) / 2; //Tо распределяем энергию поровну
					energy -= hlt;
					bots[world[x][y]].energy += hlt;
				}
				if (min0 > min1) //Если у бота больше минералов, чем у соседа
				{
					int min = (min0 - min1) / 2; //То распределяем их поровну
					minrNum -= min;
					bots[world[x][y]].minrNum += min;
				}
				incIP(5);
			}
		}

		/*Делится в абсолютном направлении
			Если
			 стена           +2
			 пусто           +3
			 органика        +4
			 успешно         +5
		*/
		else if (command == 33 || command == 43) { //Шансы появления этой команды увеличены
			unsigned short param;
			if (IP = 255) {
				IP = 0;
				param = (DNA[IP] % 16);
			}
			else
				param = (DNA[++IP] % 16); //Считываем следующий за командой байт и вычисляем остаток от деления на 16

			unsigned int x = getX(param); //Вычисляем координаты клетки
			unsigned int y = getY(param); //


			if (world[x][y] == empty) //Если пусто
				incIP(2);
			else if (world[x][y] == wall)  //Если стена
				incIP(3);
			else if (bots[world[x][y]].condition <= organic_sink) //Если органика
				incIP(4);
			else { //Остался один вариант - впереди живой бот
				int hlt0 = energy;                   // определим количество энергии и минералов
				int hlt1 = bots[world[x][y]].energy; // у бота и его соседа
				int min0 = minrNum;
				int min1 = bots[world[x][y]].minrNum;

				if (hlt0 > hlt1) //Eсли у бота больше энергии, чем у соседа
				{
					int hlt = (hlt0 - hlt1) / 2; //Tо распределяем энергию поровну
					energy -= hlt;
					bots[world[x][y]].energy += hlt;
				}
				if (min0 > min1) //Если у бота больше минералов, чем у соседа
				{
					int min = (min0 - min1) / 2; //То распределяем их поровну
					minrNum -= min;
					bots[world[x][y]].minrNum += min;
				}
				incIP(5);
			}
		}
		
		/*Отдать в относительном направлении
			Если
			 стена           +2
			 пусто           +3
			 органика        +4
			 успешно         +5
		*/
		else if (command == 34 || command == 44) {

		}

		/*Отдать в абсолютном направлении
			Если
			 стена           +2
			 пусто           +3
			 органика        +4
			 успешно         +5
		*/
		else if (command == 35 || command == 45) {

		}

	}
}

bot::~bot() {
	world[coorX][coorY] = empty; //Удаление бота с карты
	auto a = bots.begin() + n;   //Из вектора
	bots.erase(a);               //

	if (chainPrev > 0)           //Если во многоклеточной цепочке - тоже удаляем
		bots[chainPrev].chainNext = 0;
	if (chainNext > 0)
		bots[chainNext].chainPrev = 0;
}