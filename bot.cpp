//Файл основной реализации ботов
//
#include "bot.h"

extern size_t world[WORLD_WIDTH][((unsigned long long)WORLD_HEIGHT)];
extern std::vector<bot> bots;
extern int season;
bot* b;

bot::bot(unsigned int X, unsigned int Y, bot* parent, size_t N, bool free) {
#pragma warning(push)
#pragma warning(disable : 4996)
	srand();
#pragma warning(pop)

	std::vector<bot>::iterator a = bots.begin() + N;
	bots.insert(a, *this);
	for (size_t i = N; i < bots.size(); i++) //Обновление итераторов
		bots[i].n = i;

	//Если нет родителей
	if (parent == nullptr) {
		for (size_t i = 0; i < DNA_SIZE; i++) //Заполняем код командой фотосинтез
			DNA[i] = 25;
		energy = 990;
		minrNum = 0;
		chainNext = chainPrev = -1;
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

			const size_t abc = mut_ptr + 4;

			//Мутируем
			for (mut_ptr; mut_ptr < abc; mut_ptr++)
				DNA[mut_ptr] = getRandomNumber(0, 255);
		}

		energy = (parent->energy) / 2;
		parent->energy /= 2;

		minrNum = (parent->minrNum) / 2;
		parent->minrNum /= 2;

		red = parent->red;
		green = parent->green;
		blue = parent->blue;

#if free == false
		if (parent->chainNext == -1) { //Eсли у бота-предка ссылка на следующего бота в многоклеточной цепочке пуста
			parent->chainNext = N; //То вставляем туда новорожденного бота
			chainPrev = parent->n; //У новорожденного ссылка на предыдущего указывает на бота-предка
			chainNext = -1;        //Ссылка на следующего пуста, новорожденный бот является крайним в цепочке
		}
		else //Если у бота-предка ссылка на предыдущего бота в многоклеточной цепочке пуста 
		{
			parent->chainPrev = N; // то вставляем туда новорожденного бота
			chainNext = parent->n; // у новорожденного ссылка на следующего указывает на бота-предка
			chainPrev = -1;        // ссылка на предыдущего пуста, новорожденный бот является крайним в цепочке 
		}
#endif
	}
	//Указатель инструкции в самом начале
	IP = 0;

	//Очищаем стек и регистры
	for (size_t i = 0; i < SUB_SIZE; i++) {
		stack[i] = -1;
		registers[i] = 0;
	}
	heapPtr = 0;

	coorX = X; coorY = Y;
	world[X][Y] = N;

	condition = alive;
	direct = down;
}

void bot::incIP(unsigned int num) {
	IP += num;
	if (IP > DNA_MAX_INDEX)
		IP -= DNA_SIZE;
}

unsigned short bot::getParam() {
	unsigned int ptr = IP + 1;

	if (ptr > DNA_MAX_INDEX)
		ptr -= DNA_SIZE;

	return DNA[ptr];
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

void bot::goRed(short n) {
	red += n;
	if (red > 255)
		red = 255;

	green -= n;
	blue -= n;
	if (green < 0)
		green = 0;
	if (blue < 0)
		blue = 0;
}

void bot::goGreen(short n) {
	green += n;
	if (green > 255)
		green = 255;

	red -= n;
	blue -= n;
	if (red < 0)
		red = 0;
	if (blue < 0)
		blue = 0;
}

void bot::goBlue(short n) {
	blue += n;
	if (blue > 255)
		blue = 255;

	red -= n;
	green -= n;
	if (red < 0)
		red = 0;
	if (green < 0)
		green = 0;
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

	//Если бот не находится в состоянии приёма
	if (!(condition == input))
		for (unsigned short cyc = 0; cyc < 25; cyc++) { //Разрешено не более 25 команд за ход
		unsigned short command = DNA[IP]; //Текущая команда

		//Относительная смена направления
		if (command == 23) {
			long param = getParam() % 8;

			param += direct; //Полученное число прибавляем к значению направления бота
			if (param > 7) //Результат должен быть в пределах 0 ... 7
				param -= 8;
			direct = static_cast<drct>(param);
			incIP(2);
		}

		//Абсолютная смена направления
		else if (command == 24) {
			long param = getParam() % 8;

			direct = static_cast<drct>(param);
			incIP(1);
		}

		//Фотосинтез
		else if (command == 25) {
			double t = 0;
			//Вычисление коофэициента минералов
			if (minrNum < 100)
				t = 0;
			else if (minrNum < 400)
				t = 1;
			else
				t = 2;

			int hlt = std::lround(season - ((coorY + 1) % 6) + t); //Формула вычисления энергии
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
			if (chainPrev != -1 || chainNext != -1)
				//Никуда не двигаемся - ходить могут только одноклеточные
				incIP(1);
			else {
				unsigned short param = getParam() % 8;

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
			if (chainPrev != -1 || chainNext != -1)
				//Никуда не двигаемся - ходить могут только одноклеточные
				incIP(1);
			else {
				unsigned short param = getParam() % 8;

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
			unsigned short param = getParam() % 8; 

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
				int min0 = minrNum; //Кол-во минералов у бота
				int min1 = bots[world[x][y]].minrNum; //Кол-во минералов у потенциального обеда
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
			unsigned short param = getParam() % 8;

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
				int min0 = minrNum; //Кол-во минералов у бота
				int min1 = bots[world[x][y]].minrNum; //Кол-во минералов у потенциального обеда
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
			unsigned short param = getParam();

			unsigned int x = getX(param + direct); //Вычисляем координаты клетки
			unsigned int y = getY(param + direct); //
			size_t wc = world[x][y]; //Определяем объект

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
			unsigned short param = getParam() % 8;

			unsigned int x = getX(param); //Вычисляем координаты клетки
			unsigned int y = getY(param); //
			size_t wc = world[x][y]; //Определяем объект

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
			unsigned short param = getParam() % 8;

			unsigned int x = getX(param + direct); //Вычисляем координаты клетки
			unsigned int y = getY(param + direct); //


			if (world[x][y] == wall) //Если стена
				incIP(2);
			else if (world[x][y] == empty)  //Если пусто
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
			unsigned short param = getParam() % 8;

			unsigned int x = getX(param); //Вычисляем координаты клетки
			unsigned int y = getY(param); //


			if (world[x][y] == wall) //Если стена
				incIP(2);
			else if (world[x][y] == empty)  //Если пусто
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
			unsigned short param;
			if (IP = 255) {
				IP = 0;
				param = (DNA[IP] % 16);
			}
			else
				param = (DNA[IP + 1] % 16); //Считываем следующий за командой байт и вычисляем остаток от деления на 16

			unsigned int x = getX(param + direct); //Получаем координаты клетки
			unsigned int y = getY(param + direct); //

			if (world[x][y] == wall) //Если стена
				incIP(2);
			else if (world[x][y] == empty) //Если пусто
				incIP(3);
			else if (bots[world[x][y]].condition <= organic_sink) //Если органика
				incIP(4);
			else { //Если бот
				energy -= (energy / 4); //Бот отдаёт четверть своей энергии
				bots[world[x][y]].energy += (energy / 4);

				if (minrNum >= 4) { //Если у бота минералов больше 4
					minrNum -= (minrNum / 4); //Бот отдаёт четверть свох минералов
					bots[world[x][y]].minrNum += (minrNum / 4);
					if (bots[world[x][y]].minrNum > 999)
						bots[world[x][y]].minrNum = 999;
				}
			}
		}

		/*Отдать в абсолютном направлении
			Если
			 стена           +2
			 пусто           +3
			 органика        +4
			 успешно         +5
		*/
		else if (command == 35 || command == 45) {
			unsigned short param = getParam() % 8;

			unsigned int x = getX(param); //Получаем координаты клетки
			unsigned int y = getY(param); //

			if (world[x][y] == wall) //Если стена
				incIP(2);
			else if (world[x][y] == empty) //Если пусто
				incIP(3);
			else if (bots[world[x][y]].condition <= organic_sink) //Если органика
				incIP(4);
			else { //Если бот
				energy -= (energy / 4); //Бот отдаёт четверть своей энергии
				bots[world[x][y]].energy += (energy / 4);

				if (minrNum >= 4) { //Если у бота минералов больше 4
					minrNum -= (minrNum / 4); //Бот отдаёт четверть свох минералов
					bots[world[x][y]].minrNum += (minrNum / 4);
					if (bots[world[x][y]].minrNum > 999)
						bots[world[x][y]].minrNum = 999;
				}
			}
		}

		//Выравнивание по горизонтали
		else if (command == 36) {
#pragma warning(push)
#pragma warning(disable : 4996)
			srand();
#pragma warning(pop)
			if (getRandomNumber(0, 1) == 0)
				direct = left;
			else
				direct = right;
		}

		//Выравнивание по вертикали
		else if (command == 37) {
#pragma warning(push)
#pragma warning(disable : 4996)
			srand();
#pragma warning(pop)
			if (getRandomNumber(0, 1) == 0)
				direct = up;
			else
				direct = down;
		}

		/*Получение высоты бота
			Если
			 меньше          +2
			 равно/больше    +2
		*/
		else if (command == 38) {
			unsigned long param = std::lround(getParam() * HEIGHT_COEF); //Считываем следующий за командой байт и умножаем на коэффициент

			if (coorY < param)
				incIP(2);
			else
				incIP(3);
		}

		/*Получение уровня энергии
			Если
			 меньше          +2
			 равно/больше    +2
		*/
		else if (command == 39) {
			long param = std::lround(getParam() * ENERGY_COEF); //Считываем следующий за командой байт и умножаем на коэффициент

			if (energy < param)
				incIP(2);
			else
				incIP(3);
		}

		/*Получение уровня минералов
			Если
			 меньше          +2
			 равно/больше    +2
		*/
		else if (command == 40) {
			long param = std::lround(DNA[IP + 1] * ENERGY_COEF); //Считываем следующий за командой байт и умножаем на коэффициент

			if (minrNum < param)
				incIP(2);
			else
				incIP(3);
		}

		//Создание свободного живущего потомка
		else if (command == 41) {
			energy -= 150; //Бот затрачивает 150 единиц энергии
			if (energy < 0) break;

			int a = -1; //Переменная свободного направления
			for (unsigned short i = 0; i < 8; i++) {
				unsigned int x = getX(i);
				unsigned int y = getY(i);
				if (world[x][y] == empty) {
					a = i;
					break;
				}
			}
			if (a == -1) { //Если нет свободного места
				energy = 0; //Бот погибает
				break;
			}

			if (b != nullptr)
				delete b;

			if (chainNext != -1 && chainPrev != -1)
				b = new bot(getX(a), getY(a), this, n + 1, CHAIN);
			else
				b = new bot(getX(a), getY(a), this, n + 1, FREE);

			incIP(1);
			break;
		}

		/*Проверка на окружённость
		  Если
			нет места вокруг +1
			есть             +2
		*/
		else if (command == 46) {
			for (short i = 0; i < 8; i++) {
				unsigned int x = getX(i);
				unsigned int y = getY(i);
				if (world[x][y] == empty) {
					incIP(1); break;
				}
				if (i == 7) {
					incIP(2); break;
				}
			}
		}

		/*Проверка на приход энергии
		  Если
			прибавляется     +1
			не прибавляется  +2
		*/
		else if (command == 47) {
			double t;
			if (minrNum < 100)
				t = 0;
			else if (minrNum < 400)
				t = 1;
			else
				t = 2;

			int hlt = std::lround(season - ((coorY + 1) % 6) + t); //Формула вычисления энергии
			if (hlt >= 3)
				incIP(1);
			else
				incIP(2);
		}

		/*Проверка на приход минералов
		  Если
			прибавляется     +1
			не прибавляется  +2
		*/
		else if (command == 48) {
			if (coorY > MAX_Y / 2)
				incIP(1);
			else
				incIP(2);
		}

		/*Проверка на многоклеточность
		  Если
			свободноживущий  +1
			с краю цепочки   +2
			внутри цепочки   +3
		*/
		else if (command == 49) {
			if (chainNext == -1 && chainPrev == -1)
				incIP(1);
			else if (chainNext == -1 || chainPrev == -1)
				incIP(2);
			else
				incIP(3);
		}

		//Преобразование минералов в энергию
		else if (command == 50) {
			if (minrNum > 100) {// максимальное количество минералов, которые можно преобразовать в энергию = 100
				minrNum -= 100; // 1 минерал = 4 энергии
				energy += 400;  //
				goBlue(100);    // бот от этого синеет
			}
			else {  // если минералов меньше 100, то все минералы переходят в энергию
				energy += minrNum * 4;
				goBlue(minrNum);
				minrNum = 0;
			}
			break;
		}

		//Создание многоклеточного
		else if (command == 51) {
			energy -= 150; //Бот затрачивает 150 единиц энергии
			if (energy < 0) break;

			int a = -1; //Переменная свободного направления
			for (unsigned short i = 0; i < 8; i++) {
				unsigned int x = getX(i);
				unsigned int y = getY(i);
				if (world[x][y] == empty) {
					a = i;
					break;
				}
			}
			if (a == -1) { //Если нет свободного места
				energy = 0; //Бот погибает
				break;
			}

			if (b != nullptr)
				delete b;

			if (chainNext != -1 && chainPrev != -1)
				b = new bot(getX(a), getY(a), this, n + 1, FREE);
			else
				b = new bot(getX(a), getY(a), this, n + 1, CHAIN);

			incIP(1);
			break;
		}

		////////////////////////////////////
		//Начало блока "компьютерных" команд
		////////////////////////////////////

		//Помещение числа в регистр A
		else if (command == 52) {
			unsigned short param = getParam();

			registers[0] = param;
			incIP(1);
		}

		//Помещение числа в регистр B
		else if (command == 53) {
			unsigned short param = getParam();

			registers[1] = param;
			incIP(1);
		}

		//Помещение числа в регистр C
		else if (command == 54) {
			unsigned short param = getParam();

			registers[2] = param;
			incIP(1);
		}

		//Помещение числа в регистр D
		else if (command == 55) {
			unsigned short param = getParam();

			registers[3] = param;
			incIP(1);
		}

		//Помещение числа в регистр E
		else if (command == 56) {
			unsigned short param = getParam();

			registers[4] = param;
			incIP(1);
		}

		//Помещение числа в регистр F
		else if (command == 57) {
			unsigned short param = getParam();

			registers[5] = param;
			incIP(1);
		}

		//Помещение числа в регистр G
		else if (command == 58) {
			unsigned short param = getParam();

			registers[6] = param;
			incIP(1);
		}

		//Помещение числа в регистр H
		else if (command == 59) {
			unsigned short param = getParam();

			registers[7] = param;
			incIP(1);
		}

		////////////////////////////////////

		//Помещение числа из регистра A
		else if (command == 60) {
			incIP(1);
			DNA[IP] = registers[0];
		}

		//Помещение числа из регистра B
		else if (command == 61) {
			incIP(1);
			DNA[IP] = registers[1];
		}

		//Помещение числа из регистра C
		else if (command == 62) {
			incIP(1);
			DNA[IP] = registers[2];
		}

		//Помещение числа из регистра D
		else if (command == 63) {
			incIP(1);
			DNA[IP] = registers[3];
		}

		//Помещение числа из регистра E
		else if (command == 64) {
			incIP(1);
			DNA[IP] = registers[4];
		}

		//Помещение числа из регистра F
		else if (command == 65) {
			incIP(1);
			DNA[IP] = registers[5];
		}

		//Помещение числа из регистра G
		else if (command == 66) {
			incIP(1);
			DNA[IP] = registers[6];
		}

		//Помещение числа из регистра H
		else if (command == 67) {
			incIP(1);
			DNA[IP] = registers[7];
		}

		////////////////////////////////////

		//Обмен значений параметра и регистра A
		else if (command == 68) {
			incIP(1);
			std::swap(DNA[IP], registers[0]);
		}

		//Обмен значений параметра и регистра B
		else if (command == 69) {
			incIP(1);
			std::swap(DNA[IP], registers[1]);
		}

		//Обмен значений параметра и регистра C
		else if (command == 70) {
			incIP(1);
			std::swap(DNA[IP], registers[2]);
		}

		//Обмен значений параметра и регистра D
		else if (command == 71) {
			incIP(1);
			std::swap(DNA[IP], registers[3]);
		}

		//Обмен значений параметра и регистра E
		else if (command == 72) {
			incIP(1);
			std::swap(DNA[IP], registers[4]);
		}

		//Обмен значений параметра и регистра F
		else if (command == 73) {
			incIP(1);
			std::swap(DNA[IP], registers[5]);
		}

		//Обмен значений параметра и регистра G
		else if (command == 74) {
			incIP(1);
			std::swap(DNA[IP], registers[6]);
		}

		//Обмен значений параметра и регистра H
		else if (command == 75) {
			incIP(1);
			std::swap(DNA[IP], registers[7]);
		}

		////////////////////////////////////

		//Копирование из регистра A в параметорный
		else if (command == 76) {
			unsigned short param = getParam() % 8;

			registers[param] = registers[0];
			incIP(1);
		}

		//Копирование из регистра B в параметорный
		else if (command == 77) {
			unsigned short param = getParam() % 8;

			registers[param] = registers[1];
			incIP(1);
		}

		//Копирование из регистра C в параметорный
		else if (command == 78) {
			unsigned short param = getParam() % 8;

			registers[param] = registers[2];
			incIP(1);
		}

		//Копирование из регистра D в параметорный
		else if (command == 79) {
			unsigned short param = getParam() % 8;

			registers[param] = registers[3];
			incIP(1);
		}

		//Копирование из регистра E в параметорный
		else if (command == 80) {
			unsigned short param = getParam() % 8;

			registers[param] = registers[4];
			incIP(1);
		}

		//Копирование из регистра F в параметорный
		else if (command == 81) {
			unsigned short param = getParam() % 8;

			registers[param] = registers[5];
			incIP(1);
		}

		//Копирование из регистра G в параметорный
		else if (command == 82) {
			unsigned short param = getParam() % 8;

			registers[param] = registers[6];
			incIP(1);
		}

		//Копирование из регистра H в параметорный
		else if (command == 83) {
			unsigned short param = getParam() % 8;

			registers[param] = registers[7];
			incIP(1);
		}

		////////////////////////////////////

		//Обмен значений регистра А и параметорного
		else if (command == 84) {
			unsigned short param = getParam() % 8;
		
			std::swap(registers[param], registers[0]);
			incIP(1);
		}
		
		//Обмен значений регистра B и параметорного
		else if (command == 85) {
			unsigned short param = getParam() % 8;

			std::swap(registers[param], registers[1]);
			incIP(1);
		}

		//Обмен значений регистра C и параметорного
		else if (command == 86) {
			unsigned short param = getParam() % 8;

			std::swap(registers[param], registers[2]);
			incIP(1);
		}

		//Обмен значений регистра D и параметорного
		else if (command == 87) {
			unsigned short param = getParam() % 8;

			std::swap(registers[param], registers[3]);
			incIP(1);
		}

		//Обмен значений регистра E и параметорного
		else if (command == 88) {
			unsigned short param = getParam() % 8;

			std::swap(registers[param], registers[4]);
			incIP(1);
		}

		//Обмен значений регистра F и параметорного
		else if (command == 89) {
			unsigned short param = getParam() % 8;

			std::swap(registers[param], registers[5]);
			incIP(1);
		}

		//Обмен значений регистра G и параметорного
		else if (command == 90) {
			unsigned short param = getParam() % 8;

			std::swap(registers[param], registers[6]);
			incIP(1);
		}

		//Обмен значений регистра H и параметорного
		else if (command == 91) {
			unsigned short param = getParam() % 8;

			std::swap(registers[param], registers[7]);
			incIP(1);
		}

		////////////////////////////////////

		//Инкрементирование параметорного регистра
		else if (command == 92) {
			unsigned short param = getParam() % 8;

			registers[param]++;
			incIP(1);
		}

		//Декрементирование параметорного регистра
		else if (command == 93) {
			unsigned short param = getParam() % 8;

			registers[param]--;
			incIP(1);
		}

		//Cдвиг битов влево
		else if (command == 102) {
			unsigned short param = getParam() % 8;
#pragma warning(push)
#pragma warning(disable : 4244)
			unsigned char a = registers[param];
#pragma warning(pop)
			registers[param] = a << 1;
			incIP(1);
		}

		//Cдвиг битов вправо
		else if (command == 103) {
			unsigned short param = getParam() % 8;
#pragma warning(push)
#pragma warning(disable : 4244)
			unsigned char a = registers[param];
#pragma warning(pop)
			registers[param] = a >> 1;
			incIP(1);
		}

		////////////////////////////////////
		
		/*Сравнение регистра А и параметрового
		   Если
			 одинаковы       +2
			 различны        +3
		*/
		else if (command == 94) {
			unsigned short param = getParam() % 8;
			
			if (registers[0] == registers[param])
				incIP(2);
			else
				incIP(3);
		}

		/*Сравнение регистра B и параметрового
			Если
			 одинаковы       +2
			 различны        +3
		*/
		else if (command == 95) {
			unsigned short param = getParam() % 8;

			if (registers[1] == registers[param])
				incIP(2);
			else
				incIP(3);
		}

		/*Сравнение регистра C и параметрового
			Если
			 одинаковы       +2
			 различны        +3
		*/
		else if (command == 96) {
			unsigned short param = getParam() % 8;

			if (registers[2] == registers[param])
				incIP(2);
			else
				incIP(3);
		}

		/*Сравнение регистра D и параметрового
			Если
			 одинаковы       +2
			 различны        +3
		*/
		else if (command == 97) {
			unsigned short param = getParam() % 8;

			if (registers[3] == registers[param])
				incIP(2);
			else
				incIP(3);
		}

		/*Сравнение регистра E и параметрового
			Если
			 одинаковы       +2
			 различны        +3
		*/
		else if (command == 98) {
			unsigned short param = getParam() % 8;

			if (registers[4] == registers[param])
				incIP(2);
			else
				incIP(3);
		}

		/*Сравнение регистра F и параметрового
			Если
			 одинаковы       +2
			 различны        +3
		*/
		else if (command == 99) {
			unsigned short param = getParam() % 8;

			if (registers[5] == registers[param])
				incIP(2);
			else
				incIP(3);
		}

		/*Сравнение регистра G и параметрового
			Если
			 одинаковы       +2
			 различны        +3
		*/
		else if (command == 100) {
			unsigned short param = getParam() % 8;

			if (registers[6] == registers[param])
				incIP(2);
			else
				incIP(3);
		}

		/*Сравнение регистра H и параметрового
			Если
			 одинаковы       +2
			 различны        +3
		*/
		else if (command == 101) {
			unsigned short param = getParam() % 8;

			if (registers[7] == registers[param])
				incIP(2);
			else
				incIP(3);
		}

		////////////////////////////////////

		/*Безусловный переход, если регистр А равен 0
		   Если не равен     +2
		*/
		else if (command = 104) {
			if (registers[0] == 0)
				IP = getParam();
			else
				incIP(2);
		}

		/*Безусловный переход, если регистр B равен 0
		   Если не равен     +2
		*/
		else if (command = 105) {
			if (registers[1] == 0)
				IP = getParam();
			else
				incIP(2);
		}

		/*Безусловный переход, если регистр C равен 0
			Если не равен     +2
		*/
		else if (command = 106) {
			if (registers[2] == 0)
				IP = getParam();
			else
				incIP(2);
		}

		/*Безусловный переход, если регистр D равен 0
			Если не равен     +2
		*/
		else if (command = 107) {
			if (registers[3] == 0)
				IP = getParam();
			else
				incIP(2);
		}

		/*Безусловный переход, если регистр E равен 0
			Если не равен     +2
		*/
		else if (command = 108) {
			if (registers[4] == 0)
				IP = getParam();
			else
				incIP(2);
		}

		/*Безусловный переход, если регистр F равен 0
			Если не равен     +2
		*/
		else if (command = 109) {
			if (registers[5] == 0)
				IP = getParam();
			else
				incIP(2);
		}

		/*Безусловный переход, если регистр G равен 0
			Если не равен     +2
		*/
		else if (command = 110) {
			if (registers[6] == 0)
				IP = getParam();
			else
				incIP(2);
		}

		/*Безусловный переход, если регистр H равен 0
			Если не равен     +2
		*/
		else if (command = 111) {
			if (registers[7] == 0)
				IP = getParam();
			else
				incIP(2);
		}
		
		////////////////////////////////////
		////////////////////////////////////

		/*Команда вставки параметра в стек
		   Если
			 Удачно           +2
			 Переполнен       +3
		*/
		else if (command == 112) {
			unsigned short param = getParam();
			if (heapPtr == 8)
				incIP(3);
				//energy -= 10; //Для мотивации ;)
			else {
				stack[heapPtr] = param;
				heapPtr++;
				incIP(2);
			}
		}

		/*Команада взятия из стека в параметровый регистр
			Если
			 Удачно          +2
			 Пуст            +3
		*/
		else if (command == 113) {
			if (heapPtr == 0)
				incIP(3);
				//energy -= 10;
			else {
				unsigned short param = getParam() % 8;
				heapPtr--;
				registers[param] = stack[heapPtr];
				stack[heapPtr] = -1;
				incIP(2);
			}
		}

		////////////////////////////////////

		/*Кадровый просмотр стека
		  Помещение значения стека с параметра в регистр А
		   Если
			 Удачно          +2
			 Пуст            +3
		*/
		else if (command == 114) {
			unsigned short param = getParam() % 8;
			if (stack[param] == -1)
				incIP(3);
			 //energy -= 10;
			else {
				registers[0] = stack[param];
			}
		}

		/*Кадровый просмотр стека
		  Помещение значения стека с параметра в регистр B
			Если
			 Удачно          +2
			 Пуст            +3
		*/
		else if (command == 115) {
			unsigned short param = getParam() % 8;
			if (stack[param] == -1)
				incIP(3);
			//energy -= 10;
			else {
				registers[1] = stack[param];
			}
		}

		/*Кадровый просмотр стека
		  Помещение значения стека с параметра в регистр C
			Если
			 Удачно          +2
			 Пуст            +3
		*/
		else if (command == 116) {
			unsigned short param = getParam() % 8;
			if (stack[param] == -1)
				incIP(3);
			//energy -= 10;
			else {
				registers[2] = stack[param];
			}
		}

		/*Кадровый просмотр стека
		  Помещение значения стека с параметра в регистр D
			Если
			 Удачно          +2
			 Пуст            +3
		*/		
		else if (command == 117) {
			unsigned short param = getParam() % 8;
			if (stack[param] == -1)
				incIP(3);
			//energy -= 10;
			else {
				registers[3] = stack[param];
			}
		}

		/*Кадровый просмотр стека
	     Помещение значения стека с параметра в регистр E
			Если
			 Удачно          +2
			 Пуст            +3
		*/
		else if (command == 118) {
			unsigned short param = getParam() % 8;
			if (stack[param] == -1)
				incIP(3);
			//energy -= 10;
			else {
				registers[4] = stack[param];
			}
		}

		/*Кадровый просмотр стека
		  Помещение значения стека с параметра в регистр F
			Если
			 Удачно          +2
			 Пуст            +3
		*/
		else if (command == 119) {
			unsigned short param = getParam() % 8;
			if (stack[param] == -1)
				incIP(3);
			//energy -= 10;
			else {
				registers[5] = stack[param];
			}
		}

		/*Кадровый просмотр стека
		  Помещение значения стека с параметра в регистр G
			Если
			 Удачно          +2
			 Пуст            +3
		*/
		else if (command == 120) {
			unsigned short param = getParam() % 8;
			if (stack[param] == -1)
				incIP(3);
			//energy -= 10;
			else {
				registers[6] = stack[param];
			}
		}

		/*Кадровый просмотр стека
		  Помещение значения стека с параметра в регистр H
			Если
			 Удачно          +2
			 Пуст            +3
		*/
		else if (command == 121) {
			unsigned short param = getParam() % 8;
			if (stack[param] == -1)
				incIP(3);
			//energy -= 10;
			else {
				registers[7] = stack[param];
			}
		}
		
		////////////////////////////////////

		//Обмен значений стека и параметрового регистра
		else if (command == 122) {
			if (heapPtr > 0)
				std::swap(stack[heapPtr - 1], registers[(getParam() % 8)]);
		}

		////////////////////////////////////

		//Переход по адресу с верхушки стека
		//(Если он равен -1, перейти на случайный)
		else if (command == 123) {
			if (heapPtr == 0) {
#pragma warning(push)
#pragma warning(disable : 4996)
				srand();
#pragma warning(pop)
				IP = getRandomNumber(0, 255);
			}
			else {
				heapPtr--;
				IP = stack[heapPtr];
				stack[heapPtr] = -1;
			}
		}

		//Запись текущего адреса в стек	
		//При неудаче       +2
		else if (command == 124) {
			if (heapPtr == 8)
				incIP(2);
			else {
				stack[heapPtr] = IP;
				heapPtr++;
			}
		}

		////////////////////////////////////
		////////////////////////////////////

		//Если не является командой, совершить условный переход
		else
			incIP(command);
	}
	else
		condition = alive;

	//
	//Завершение шага
	//

		//Если бот в цепочке
		if (chainNext != -1 && chainPrev != -1) {
			//Делим минералы
			unsigned int min = minrNum + bots[chainNext].minrNum + bots[chainPrev].minrNum;
			minrNum = (min / 3) + (min % 3);
			bots[chainNext].minrNum = min;
			bots[chainPrev].minrNum = min;

			//Делим энергию
			//
			//Проверим, являются ли следующий и предыдущий боты в цепочке крайними
			//Если они не являются крайними, то распределяем энергию поровну
			//Связано это с тем, что в крайних ботах в цепочке должно быть больше энергии
			//Чтобы они плодили новых ботов и удлиняли цепочку
			if ((bots[chainNext].chainNext != -1 || bots[chainNext].chainPrev != -1) && (bots[chainPrev].chainNext != -1 || bots[chainPrev].chainPrev != -1)) {
				unsigned int hlt = (energy + bots[chainNext].energy + bots[chainPrev].energy);
				energy = (min / 3) + (min % 3);
				bots[chainNext].energy = hlt;
				bots[chainPrev].energy = hlt;
			}
		}
		//Если бот имеет предыдущего в цепочке
		else if (chainPrev != -1) { 
			//Если предыдущий не является крайним в цепочке
			if (bots[chainPrev].chainNext != -1 && bots[chainPrev].chainPrev != -1) {
				//То распределяем энергию в пользу текущёго бота
				unsigned int hlt = energy + bots[chainPrev].energy;
				bots[chainPrev].energy = (hlt / 4) + (hlt % 4);
				energy = hlt / 4 * 3;
			}
		}
		//Если бот имеет предыдущего в цепочке
		else if (chainNext != -1) { 
			//Если предыдущий не является крайним в цепочке
			if (bots[chainNext].chainNext != -1 && bots[chainNext].chainPrev != -1) {
				//То распределяем энергию в пользу текущёго бота
				unsigned int hlt = energy + bots[chainNext].energy;
				bots[chainNext].energy = (hlt / 4) + (hlt % 4);
				energy = hlt / 4 * 3;
			}
		}

		//Если энергии больше 999, то плодим нового бота
		if (energy > 999) {
			int a = -1; //Переменная свободного направления
			for (unsigned short i = 0; i < 8; i++) {
				unsigned int x = getX(i);
				unsigned int y = getY(i);
				if (world[x][y] == empty) {
					a = i;
					break;
				}
			}
			if (a == -1) //Если нет свободного места
				energy = 0; //Бот погибает
			else {
				if (b != nullptr)
					delete b;
				if (chainNext == -1 && chainPrev == -1)
					b = new bot(getX(a), getY(a), this, n + 1, FREE);
				else
					b = new bot(getX(a), getY(a), this, n + 1, CHAIN);
			}
		}
		//Если энергии стало меньше 1
		else if (energy < 1) {
			condition = organic_sink; //Отмечаем как органику
			if (chainPrev != -1)      //Если состоит во многоклеточной цепочке - удаляем
				bots[chainPrev].chainNext = -1;
			if (chainNext != -1)
				bots[chainNext].chainPrev = -1;
		}

#pragma warning(push)
#pragma warning(disable : 4018)
		//Если бот глубже, чем MAX_Y / 2, то он начинает накапливать минералы
		if (coorY > std::lround(MAX_Y / 2))
			minrNum++;
		else if (coorY > std::lround(MAX_Y / 2 + MAX_Y / 4))
			minrNum += 2;
		else if (coorY > std::lround(MAX_Y - MAX_Y / 10))
			minrNum += 3;
#pragma warning(pop)

		//Проверка предела
		if (minrNum > 999)
			minrNum = 999;
}

bot::~bot() {
	world[coorX][coorY] = empty; //Удаление бота с карты
	auto a = bots.begin() + n;   //Из вектора
	bots.erase(a);               //
	for (size_t i = n; i < bots.size(); i++) //Обновление итераторов
		bots[i].n = i;

	if (chainPrev > 0)           //Если во многоклеточной цепочке - тоже удаляем
		bots[chainPrev].chainNext = -1;
	if (chainNext > 0)
		bots[chainNext].chainPrev = -1;
}

//Необязательная функция
//Изменяет один случайный ген у ботов,
//находящихся на самом верхнем уровне
void radiation() {
	for (size_t i = 0; i < WORLD_WIDTH; i++)
		if (world[i][1] != empty)
			bots[world[i][1]].DNA[getRandomNumber(0, 255)] = getRandomNumber(0, 255);
}