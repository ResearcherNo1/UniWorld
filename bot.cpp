//Ôàéë îñíîâíîé ðåàëèçàöèè áîòîâ
//
#include "bot.h"


extern long long world[WORLD_WIDTH][((unsigned long long)WORLD_HEIGHT + 2)];
extern std::vector<bot> bots;
extern int season;
bot* b;

bot::bot(unsigned int X, unsigned int Y, bot* parent, size_t N, bool free) {
#pragma warning(push)
#pragma warning(disable : 4996)
	srand();
#pragma warning(pop)

	//Åñëè íåò ðîäèòåëåé
	if (parent == nullptr) {
		for (size_t i = 0; i < DNA_SIZE; i++) //Çàïîëíÿåì êîä êîìàíäîé ôîòîñèíòåç
			DNA[i] = 25;
		energy = 990;
		minrNum = 0;
		chainNext = chainPrev = -1;
		red = green = blue = 170;
	}
	else {
		//Â îäíîì ñëó÷àå èç ÷åòûð¸õ
		if (getRandomNumber(1, 4) == 2) {
			//Âû÷èñëÿåì óêàçàòåëü ìóòàöèè
			size_t mut_ptr = getRandomNumber(0, 248);
			//Êîïèðóåì ãåíîì ðîäèòåëÿ
			for (size_t i = 0; i < DNA_SIZE; i++)
				DNA[i] = parent->DNA[i];

			const size_t abc = mut_ptr + 4;

			//Ìóòèðóåì
			for (mut_ptr; mut_ptr < abc; mut_ptr++)
				DNA[mut_ptr] = getRandomNumber(0, 255);
		}
		else //Êîïèðóåì ãåíîì ðîäèòåëÿ
			for (size_t i = 0; i < DNA_SIZE; i++)
				DNA[i] = parent->DNA[i];

		energy = (parent->energy) / 2;
		parent->energy /= 2;

		minrNum = (parent->minrNum) / 2;
		parent->minrNum /= 2;

		red = parent->red;
		green = parent->green;
		blue = parent->blue;

		if (free == false) {
			if (parent->chainNext == -1) { //Eñëè ó áîòà-ïðåäêà ññûëêà íà ñëåäóþùåãî áîòà â ìíîãîêëåòî÷íîé öåïî÷êå ïóñòà
				parent->chainNext = N; //Òî âñòàâëÿåì òóäà íîâîðîæäåííîãî áîòà
				chainPrev = parent->n; //Ó íîâîðîæäåííîãî ññûëêà íà ïðåäûäóùåãî óêàçûâàåò íà áîòà-ïðåäêà
				chainNext = -1;        //Ññûëêà íà ñëåäóþùåãî ïóñòà, íîâîðîæäåííûé áîò ÿâëÿåòñÿ êðàéíèì â öåïî÷êå
			}
			else //Åñëè ó áîòà-ïðåäêà ññûëêà íà ïðåäûäóùåãî áîòà â ìíîãîêëåòî÷íîé öåïî÷êå ïóñòà 
			{
				parent->chainPrev = N; // òî âñòàâëÿåì òóäà íîâîðîæäåííîãî áîòà
				chainNext = parent->n; // ó íîâîðîæäåííîãî ññûëêà íà ñëåäóþùåãî óêàçûâàåò íà áîòà-ïðåäêà
				chainPrev = -1;        // ññûëêà íà ïðåäûäóùåãî ïóñòà, íîâîðîæäåííûé áîò ÿâëÿåòñÿ êðàéíèì â öåïî÷êå 
			}
		}
		else {
			chainPrev = -1;
			chainNext = -1;
		}
	}


	//Óêàçàòåëü èíñòðóêöèè â ñàìîì íà÷àëå
	IP = 0;

	//Î÷èùàåì ñòåê è ðåãèñòðû
	for (size_t i = 0; i < SUB_SIZE; i++) {
		stack[i] = -1;
		registers[i] = 0;
	}
	heapPtr = 0;

	coorX = X; coorY = Y;
	world[X][Y] = N;

	condition = alive;
	decompose = 0;
	direct = down;

	bots.push_back(*this);

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
	if (_bot.condition < alive) //Åñëè _bot - îðãàíèêà
		return false;

	size_t dif = 0; //C÷¸ò÷èê íåñîâïàäåíèé â ãåíîìå
	for (size_t i = 0; i < DNA_SIZE; i++) //Ïîÿ÷åå÷íî ñâåðÿåì ãåíîìû
		if (DNA[i] != _bot.DNA[i]) {
			dif++;
			if (dif > 8) return false; //Åñëè íåñîâïàäåíèé > 8, òî áîòû íå ðîäñòâåííèêè
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

void bot::print(std::string a) {
	printf("\t\t %s \n\n", a.c_str());
	printf("ENERGY - %i   ", energy); printf("MINERALS - %i\n", minrNum);
	printf("IP - %i   ", IP); printf("Command - %i\n", DNA[IP]);
	printf("X - %i   ", coorX); printf("Y - %i\n\n", coorY);
}

void bot::step() {
	//Åñëè áîò - îðãàíèêà, ðàçëàãàåìñÿ
	if (condition == organic_hold) {
		if (decompose == 100) {
			death();
			return;
		}
		decompose++;
	}
	if (condition == organic_sink) { //Ïàäåíèå îðãàíèêè
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
	//Åñëè ìû çäåñü, òî áîò æèâîé
	//

	//Åñëè áîò íå íàõîäèòñÿ â ñîñòîÿíèè ïðè¸ìà
	if (!(condition == input))
		for (unsigned short cyc = 0; cyc < 25; cyc++) { //Ðàçðåøåíî íå áîëåå 25 êîìàíä çà õîä
		unsigned short command = DNA[IP]; //Òåêóùàÿ êîìàíäà

		//Îòíîñèòåëüíàÿ ñìåíà íàïðàâëåíèÿ
		if (command == 23) {
			long param = getParam() % 8;

			param += direct; //Ïîëó÷åííîå ÷èñëî ïðèáàâëÿåì ê çíà÷åíèþ íàïðàâëåíèÿ áîòà
			if (param > 7) //Ðåçóëüòàò äîëæåí áûòü â ïðåäåëàõ 0 ... 7
				param -= 8;
			direct = static_cast<drct>(param);
			incIP(2);
		}

		//Àáñîëþòíàÿ ñìåíà íàïðàâëåíèÿ
		else if (command == 24) {
			long param = getParam() % 8;

			direct = static_cast<drct>(param);
			incIP(1);
		}

		//Ôîòîñèíòåç
		else if (command == 25) {
			double t = 0;
			//Âû÷èñëåíèå êîîôýèöèåíòà ìèíåðàëîâ
			if (minrNum < 100)
				t = 0;
			else if (minrNum < 400)
				t = 1;
			else
				t = 2;

			int hlt = std::lround(season - ((coorY -  1) % 9) + t); //Ôîðìóëà âû÷èñëåíèÿ ýíåðãèè
			if (hlt > 0) {
				energy += hlt; //Ïðèáàâëÿåì ïîëó÷åííóþ ýíåðãèÿ ê ýíåðãèè áîòà
				goGreen(hlt); //Áîò îò ýòîãî çåëåíååò
			}
			incIP(1);
			break; //Âûõîäèì, òàê êàê êîìàíäà "ôîòîñèíòåç" - çàâåðøàþùàÿ
		}

		/*Øàã â îòíîñèòåëüíîì íàïðàâëåíèè
			Åñëè
			 óäàëîñü ïåðåéòè +2
			 ñòåíà           +3
			 îðãàíèêà        +4
			 áîò             +5
			 ðîäíÿ           +6
		*/
		else if (command == 26) {
			//Åñëè áîò ìíîãîêëåòî÷íûé
			if (chainPrev != -1 || chainNext != -1)
				//Íèêóäà íå äâèãàåìñÿ - õîäèòü ìîãóò òîëüêî îäíîêëåòî÷íûå
				incIP(1);
			else {
				unsigned short param = getParam() % 8;

				unsigned int x = getX(param + direct); //Ïîëó÷àåì êîîðäèíàòû êëåòêè
				unsigned int y = getY(param + direct); //

				if (world[x][y] == empty) { //Åñëè íà êëåòêå ïóñòî
					world[x][y] = n;
					world[coorX][coorY] = empty;
					coorX = x;
					coorY = y;
					incIP(2);
				}
				else if (world[x][y] == wall) { //Åñëè íà êëåòêå ñòåíà
					incIP(3);
				}
				else if (bots[world[x][y]].condition <= organic_sink) { //Åñëè íà êëåòêå îðãàíèêà
					incIP(4);
				}
				else if (isRelative(bots[world[x][y]])) { //Åñëè íà êëåòêå ðîäíÿ
					incIP(6);
				}
				else { //Îñòàâøèéñÿ âàðèàíò - íà êëåòêå áîò
					incIP(5);
				}
			}
			break; //Âûõîäèì, òàê êàê êîìàíäà "Øàã" - çàâåðøàþùàÿ
		} //

		/*Øàã â àáñîëþòíîì íàïðàâëåíèè
			Åñëè
			 óäàëîñü ïåðåéòè +2
			 ñòåíà           +3
			 îðãàíèêà        +4
			 áîò             +5
			 ðîäíÿ           +6
		*/
		else if (command == 27) {
			//Åñëè áîò ìíîãîêëåòî÷íûé
			if (chainPrev != -1 || chainNext != -1)
				//Íèêóäà íå äâèãàåìñÿ - õîäèòü ìîãóò òîëüêî îäíîêëåòî÷íûå
				incIP(1);
			else {
				unsigned short param = getParam() % 8;

				unsigned int x = getX(param); //Ïîëó÷àåì êîîðäèíàòû êëåòêè
				unsigned int y = getY(param); //

				if (world[x][y] == empty) { //Åñëè íà êëåòêå ïóñòî
					world[x][y] = n;
					world[coorX][coorY] = empty;
					coorX = x;
					coorY = y;
					incIP(2);
				}
				else if (world[x][y] == wall) { //Åñëè íà êëåòêå ñòåíà
					incIP(3);
				}
				else if (bots[world[x][y]].condition <= organic_sink) { //Åñëè íà êëåòêå îðãàíèêà
					incIP(4);
				}
				else if (isRelative(bots[world[x][y]])) { //Åñëè íà êëåòêå ðîäíÿ
					incIP(6);
				}
				else { //Îñòàâøèéñÿ âàðèàíò - íà êëåòêå áîò
					incIP(5);
				}
			}
			break;

		}

		/*Cúåñòü â îòíîñèòåëüíîì íàïðàâëåíèè
			Åñëè
			 ïóñòî           +2
			 ñòåíà           +3
			 îðãàíèêà        +4
			 áîò             +5
		*/
		else if (command == 28) {
			unsigned short param = getParam() % 8; 

			energy -= 4; //Áîò íà ýòîì ýòàïå, íåçàâèñèìî îò ðåçóëüòàòà, òåðÿåò 4 ýíåðãèè
			unsigned int x = getX(param + direct); //Ïîëó÷àåì êîîðäèíàòû êëåòêè
			unsigned int y = getY(param + direct); //


			if (world[x][y] == empty) { //Åñëè ïóñòî
				incIP(2);
			}
			else if (world[x][y] == wall) { //Åñëè ñòåíà
				incIP(3);
			}
			else if (bots[world[x][y]].condition <= organic_sink) { //Åñëè òàì îêàçàëàñü îðãàíèêà
				bots[world[x][y]].death(); //Óäàëÿåì æåðòâó
				energy += 100;            //Ïðèáàâêà ê ýíåðãèè
				goRed(100);               //Áîò êðàñíååò
				incIP(4);
			}
			else {
				//
				//--------- äîøëè äî ñþäà, çíà÷èò âïåðåäè æèâîé áîò -------------------
				//
				int min0 = minrNum; //Êîë-âî ìèíåðàëîâ ó áîòà
				int min1 = bots[world[x][y]].minrNum; //Êîë-âî ìèíåðàëîâ ó ïîòåíöèàëüíîãî îáåäà
				int hl = bots[world[x][y]].energy;  //Êîë-âî ýíåðãèè

				//Åñëè ó áîòà ìèíåðàëîâ áîëüøå
				if (min0 >= min1) {
					minrNum -= min1;  //Êîëè÷åñòâî ìèíåðàëîâ ó áîòà óìåíüøàåòñÿ íà êîëè÷åñòâî ìèíåðàëîâ ó æåðòâû
													  //Òèïà, ñòåñàë ñâîè çóáû î ïàíöèðü æåðòâû)
					bots[world[x][y]].death(); //Óäàëÿåì æåðòâó
					energy += 100 + (hl / 2); //Kîëè÷åñòâî ýíåðãèè ó áîòà ïðèáàâëÿåòñÿ íà 100 + (ïîëîâèíà îò ýíåðãèè æåðòâû)
					goRed(hl);                //Áîò êðàñíååò
					incIP(5);
				}
				//Åñëè îáåä áîëåå áðîíèðîâàíûé
				else if (min1 > min0) {
					minrNum = 0;  //Òî áîò èçðàñõîäîâàë âñå ìèíåðàëû íà ïðåîäîëåíèå çàùèòû
					min1 -= min0; //Íî è ó îáåäà îíè óìåíüøèëèñü
					//Åñëè ýíåðãèè â 2 ðàçà áîëüøå
					if (energy >= 2 * min1) {
						bots[world[x][y]].death(); //Óäàëÿåì æåðòâó
						energy += 100 + (hl / 2) - 2 * min1;
						goRed(hl);                //Áîò êðàñíååò
						incIP(5);
					}
					//Åñëè ýíåðãèè ìåíüøå, ÷åì (ìèíåðàëîâ ó æåðòâû)*2, òî áîò ïîãèáàåò îò æåðòâû 
					else {
						bots[world[x][y]].minrNum = min1 - (energy / 2); //Æåðòâà èñòðà÷èâàåò ñâîè ìèíåðàëû
						energy = -10; //Ýíåðãèÿ óõîäèò â ìèíóñ
						incIP(5);
					}
				}
			}
			break;
		}

		/*Cúåñòü â àáñîëþòíîì íàïðàâëåíèè
			Åñëè
			 ïóñòî           +2
			 ñòåíà           +3
			 îðãàíèêà        +4
			 áîò             +5
		*/
		else if (command == 29) {
			unsigned short param = getParam() % 8;

			energy -= 4; //Áîò íà ýòîì ýòàïå, íåçàâèñèìî îò ðåçóëüòàòà, òåðÿåò 4 ýíåðãèè
			unsigned int x = getX(param); //Ïîëó÷àåì êîîðäèíàòû êëåòêè
			unsigned int y = getY(param); //


			if (world[x][y] == empty) { //Åñëè ïóñòî
				incIP(2);
			}
			else if (world[x][y] == wall) { //Åñëè ñòåíà
				incIP(3);
			}
			else if (bots[world[x][y]].condition <= organic_sink) { //Åñëè òàì îêàçàëàñü îðãàíèêà
				bots[world[x][y]].death(); //Óäàëÿåì æåðòâó
				energy += 100;            //Ïðèáàâêà ê ýíåðãèè
				goRed(100);               //Áîò êðàñíååò
				incIP(4);
			}
			else {
				//
				//--------- äîøëè äî ñþäà, çíà÷èò âïåðåäè æèâîé áîò -------------------
				//
				int min0 = minrNum; //Êîë-âî ìèíåðàëîâ ó áîòà
				int min1 = bots[world[x][y]].minrNum; //Êîë-âî ìèíåðàëîâ ó ïîòåíöèàëüíîãî îáåäà
				int hl = bots[world[x][y]].energy;  //Êîë-âî ýíåðãèè

				//Åñëè ó áîòà ìèíåðàëîâ áîëüøå
				if (min0 >= min1) {
					minrNum -= min1;  //Êîëè÷åñòâî ìèíåðàëîâ ó áîòà óìåíüøàåòñÿ íà êîëè÷åñòâî ìèíåðàëîâ ó æåðòâû
													  //Òèïà, ñòåñàë ñâîè çóáû î ïàíöèðü æåðòâû)
					bots[world[x][y]].death(); //Óäàëÿåì æåðòâó
					energy += 100 + (hl / 2); //Kîëè÷åñòâî ýíåðãèè ó áîòà ïðèáàâëÿåòñÿ íà 100 + (ïîëîâèíà îò ýíåðãèè æåðòâû)
					goRed(hl);                //Áîò êðàñíååò
					incIP(5);
				}
				//Åñëè îáåä áîëåå áðîíèðîâàíûé
				else if (min1 > min0) {
					minrNum = 0;  //Òî áîò èçðàñõîäîâàë âñå ìèíåðàëû íà ïðåîäîëåíèå çàùèòû
					min1 -= min0; //Íî è ó îáåäà îíè óìåíüøèëèñü
					//Åñëè ýíåðãèè â 2 ðàçà áîëüøå
					if (energy >= 2 * min1) {
						bots[world[x][y]].death(); //Óäàëÿåì æåðòâó
						energy += 100 + (hl / 2) - 2 * min1;
						goRed(hl);                //Áîò êðàñíååò
						incIP(5);
					}
					//Åñëè ýíåðãèè ìåíüøå, ÷åì (ìèíåðàëîâ ó æåðòâû)*2, òî áîò ïîãèáàåò îò æåðòâû 
					else {
						bots[world[x][y]].minrNum = min1 - (energy / 2); //Æåðòâà èñòðà÷èâàåò ñâîè ìèíåðàëû
						energy = -10; //Ýíåðãèÿ óõîäèò â ìèíóñ
						incIP(5);
					}
				}
			}
			break;
		}

		/*Ïîñìîòðåòü â îòíîñèòåëüíîì íàïðàâëåíèè
			Åñëè
			 ïóñòî           +2
			 ñòåíà           +3
			 îðãàíèêà        +4
			 áîò             +5
			 ðîäíÿ           +6
		*/
		else if (command == 30) {
			unsigned short param = getParam();

			unsigned int x = getX(param + direct); //Âû÷èñëÿåì êîîðäèíàòû êëåòêè
			unsigned int y = getY(param + direct); //
			size_t wc = world[x][y]; //Îïðåäåëÿåì îáúåêò

			if (wc == empty) { //Åñëè ïóñòî
				incIP(2);
			}
			else if (wc == wall) { //Åñëè ñòåíà
				incIP(3);
			}
			else if (bots[wc].condition <= organic_sink) { //Åñëè îðãàíèêà
				incIP(4);
			}
			else if (isRelative(bots[wc])) { //Åñëè ðîäíÿ
				incIP(6);
			}
			else { //Åñëè áîò
				incIP(5);
			}
		}

		/*Ïîñìîòðåòü â àáñîëþòíîì íàïðàâëåíèè
			Åñëè
			 ïóñòî           +2
			 ñòåíà           +3
			 îðãàíèêà        +4
			 áîò             +5
			 ðîäíÿ           +6
		*/
		else if (command == 31) {
			unsigned short param = getParam() % 8;

			unsigned int x = getX(param); //Âû÷èñëÿåì êîîðäèíàòû êëåòêè
			unsigned int y = getY(param); //
			size_t wc = world[x][y]; //Îïðåäåëÿåì îáúåêò

			if (wc == empty) { //Åñëè ïóñòî
				incIP(2);
			}
			else if (wc == wall) { //Åñëè ñòåíà
				incIP(3);
			}
			else if (bots[wc].condition <= organic_sink) { //Åñëè îðãàíèêà
				incIP(4);
			}
			else if (isRelative(bots[wc])) { //Åñëè ðîäíÿ
				incIP(6);
			}
			else { //Åñëè áîò
				incIP(5);
			}
		}

		/*Äåëèòñÿ â îòíîñèòåëüíîì íàïðàâëåíèè
			Åñëè
			 ñòåíà           +2
			 ïóñòî           +3
			 îðãàíèêà        +4
			 óñïåøíî         +5
		*/
		else if (command == 32 || command == 42) { //Øàíñû ïîÿâëåíèÿ ýòîé êîìàíäû óâåëè÷åíû
			unsigned short param = getParam() % 8;

			unsigned int x = getX(param + direct); //Âû÷èñëÿåì êîîðäèíàòû êëåòêè
			unsigned int y = getY(param + direct); //


			if (world[x][y] == wall) //Åñëè ñòåíà
				incIP(2);
			else if (world[x][y] == empty)  //Åñëè ïóñòî
				incIP(3);
			else if (bots[world[x][y]].condition <= organic_sink) //Åñëè îðãàíèêà
				incIP(4);
			else { //Îñòàëñÿ îäèí âàðèàíò - âïåðåäè æèâîé áîò
				int hlt0 = energy;                   // îïðåäåëèì êîëè÷åñòâî ýíåðãèè è ìèíåðàëîâ
				int hlt1 = bots[world[x][y]].energy; // ó áîòà è åãî ñîñåäà
				int min0 = minrNum;
				int min1 = bots[world[x][y]].minrNum;

				if (hlt0 > hlt1) //Eñëè ó áîòà áîëüøå ýíåðãèè, ÷åì ó ñîñåäà
				{
					int hlt = (hlt0 - hlt1) / 2; //Tî ðàñïðåäåëÿåì ýíåðãèþ ïîðîâíó
					energy -= hlt;
					bots[world[x][y]].energy += hlt;
				}
				if (min0 > min1) //Åñëè ó áîòà áîëüøå ìèíåðàëîâ, ÷åì ó ñîñåäà
				{
					int min = (min0 - min1) / 2; //Òî ðàñïðåäåëÿåì èõ ïîðîâíó
					minrNum -= min;
					bots[world[x][y]].minrNum += min;
				}
				incIP(5);
			}
		}

		/*Äåëèòñÿ â àáñîëþòíîì íàïðàâëåíèè
			Åñëè
			 ñòåíà           +2
			 ïóñòî           +3
			 îðãàíèêà        +4
			 óñïåøíî         +5
		*/
		else if (command == 33 || command == 43) { //Øàíñû ïîÿâëåíèÿ ýòîé êîìàíäû óâåëè÷åíû
			unsigned short param = getParam() % 8;

			unsigned int x = getX(param); //Âû÷èñëÿåì êîîðäèíàòû êëåòêè
			unsigned int y = getY(param); //


			if (world[x][y] == wall) //Åñëè ñòåíà
				incIP(2);
			else if (world[x][y] == empty)  //Åñëè ïóñòî
				incIP(3);
			else if (bots[world[x][y]].condition <= organic_sink) //Åñëè îðãàíèêà
				incIP(4);
			else { //Îñòàëñÿ îäèí âàðèàíò - âïåðåäè æèâîé áîò
				int hlt0 = energy;                   // îïðåäåëèì êîëè÷åñòâî ýíåðãèè è ìèíåðàëîâ
				int hlt1 = bots[world[x][y]].energy; // ó áîòà è åãî ñîñåäà
				int min0 = minrNum;
				int min1 = bots[world[x][y]].minrNum;

				if (hlt0 > hlt1) //Eñëè ó áîòà áîëüøå ýíåðãèè, ÷åì ó ñîñåäà
				{
					int hlt = (hlt0 - hlt1) / 2; //Tî ðàñïðåäåëÿåì ýíåðãèþ ïîðîâíó
					energy -= hlt;
					bots[world[x][y]].energy += hlt;
				}
				if (min0 > min1) //Åñëè ó áîòà áîëüøå ìèíåðàëîâ, ÷åì ó ñîñåäà
				{
					int min = (min0 - min1) / 2; //Òî ðàñïðåäåëÿåì èõ ïîðîâíó
					minrNum -= min;
					bots[world[x][y]].minrNum += min;
				}
				incIP(5);
			}
		}

		/*Îòäàòü â îòíîñèòåëüíîì íàïðàâëåíèè
			Åñëè
			 ñòåíà           +2
			 ïóñòî           +3
			 îðãàíèêà        +4
			 óñïåøíî         +5
		*/
		else if (command == 34 || command == 44) {
			unsigned short param;
			if (IP = 255) {
				IP = 0;
				param = (DNA[IP] % 16);
			}
			else
				param = (DNA[IP + 1] % 16); //Ñ÷èòûâàåì ñëåäóþùèé çà êîìàíäîé áàéò è âû÷èñëÿåì îñòàòîê îò äåëåíèÿ íà 16

			unsigned int x = getX(param + direct); //Ïîëó÷àåì êîîðäèíàòû êëåòêè
			unsigned int y = getY(param + direct); //

			if (world[x][y] == wall) //Åñëè ñòåíà
				incIP(2);
			else if (world[x][y] == empty) //Åñëè ïóñòî
				incIP(3);
			else if (bots[world[x][y]].condition <= organic_sink) //Åñëè îðãàíèêà
				incIP(4);
			else { //Åñëè áîò
				energy -= (energy / 4); //Áîò îòäà¸ò ÷åòâåðòü ñâîåé ýíåðãèè
				bots[world[x][y]].energy += (energy / 4);

				if (minrNum >= 4) { //Åñëè ó áîòà ìèíåðàëîâ áîëüøå 4
					minrNum -= (minrNum / 4); //Áîò îòäà¸ò ÷åòâåðòü ñâîõ ìèíåðàëîâ
					bots[world[x][y]].minrNum += (minrNum / 4);
					if (bots[world[x][y]].minrNum > 999)
						bots[world[x][y]].minrNum = 999;
				}
			}
		}

		/*Îòäàòü â àáñîëþòíîì íàïðàâëåíèè
			Åñëè
			 ñòåíà           +2
			 ïóñòî           +3
			 îðãàíèêà        +4
			 óñïåøíî         +5
		*/
		else if (command == 35 || command == 45) {
			unsigned short param = getParam() % 8;

			unsigned int x = getX(param); //Ïîëó÷àåì êîîðäèíàòû êëåòêè
			unsigned int y = getY(param); //

			if (world[x][y] == wall) //Åñëè ñòåíà
				incIP(2);
			else if (world[x][y] == empty) //Åñëè ïóñòî
				incIP(3);
			else if (bots[world[x][y]].condition <= organic_sink) //Åñëè îðãàíèêà
				incIP(4);
			else { //Åñëè áîò
				energy -= (energy / 4); //Áîò îòäà¸ò ÷åòâåðòü ñâîåé ýíåðãèè
				bots[world[x][y]].energy += (energy / 4);

				if (minrNum >= 4) { //Åñëè ó áîòà ìèíåðàëîâ áîëüøå 4
					minrNum -= (minrNum / 4); //Áîò îòäà¸ò ÷åòâåðòü ñâîõ ìèíåðàëîâ
					bots[world[x][y]].minrNum += (minrNum / 4);
					if (bots[world[x][y]].minrNum > 999)
						bots[world[x][y]].minrNum = 999;
				}
			}
		}

		//Âûðàâíèâàíèå ïî ãîðèçîíòàëè
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

		//Âûðàâíèâàíèå ïî âåðòèêàëè
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

		/*Ïîëó÷åíèå âûñîòû áîòà
			Åñëè
			 ìåíüøå          +2
			 ðàâíî/áîëüøå    +2
		*/
		else if (command == 38) {
			unsigned long param = std::lround(getParam() * HEIGHT_COEF); //Ñ÷èòûâàåì ñëåäóþùèé çà êîìàíäîé áàéò è óìíîæàåì íà êîýôôèöèåíò

			if (coorY < param)
				incIP(2);
			else
				incIP(3);
		}

		/*Ïîëó÷åíèå óðîâíÿ ýíåðãèè
			Åñëè
			 ìåíüøå          +2
			 ðàâíî/áîëüøå    +2
		*/
		else if (command == 39) {
			long param = std::lround(getParam() * ENERGY_COEF); //Ñ÷èòûâàåì ñëåäóþùèé çà êîìàíäîé áàéò è óìíîæàåì íà êîýôôèöèåíò

			if (energy < param)
				incIP(2);
			else
				incIP(3);
		}

		/*Ïîëó÷åíèå óðîâíÿ ìèíåðàëîâ
			Åñëè
			 ìåíüøå          +2
			 ðàâíî/áîëüøå    +2
		*/
		else if (command == 40) {
			long param = std::lround(DNA[IP + 1] * ENERGY_COEF); //Ñ÷èòûâàåì ñëåäóþùèé çà êîìàíäîé áàéò è óìíîæàåì íà êîýôôèöèåíò

			if (minrNum < param)
				incIP(2);
			else
				incIP(3);
		}

		//Ñîçäàíèå ñâîáîäíîãî æèâóùåãî ïîòîìêà
		else if (command == 41) {
			energy -= 150; //Áîò çàòðà÷èâàåò 150 åäèíèö ýíåðãèè
			if (energy < 0) break;

			int a = -1; //Ïåðåìåííàÿ ñâîáîäíîãî íàïðàâëåíèÿ
			for (unsigned short i = 0; i < 8; i++) {
				unsigned int x = getX(i);
				unsigned int y = getY(i);
				if (world[x][y] == empty) {
					a = i;
					break;
				}
			}
			if (a == -1) { //Åñëè íåò ñâîáîäíîãî ìåñòà
				energy = 0; //Áîò ïîãèáàåò
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

		/*Ïðîâåðêà íà îêðóæ¸ííîñòü
		  Åñëè
			íåò ìåñòà âîêðóã +1
			åñòü             +2
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

		/*Ïðîâåðêà íà ïðèõîä ýíåðãèè
		  Åñëè
			ïðèáàâëÿåòñÿ     +1
			íå ïðèáàâëÿåòñÿ  +2
		*/
		else if (command == 47) {
			double t;
			if (minrNum < 100)
				t = 0;
			else if (minrNum < 400)
				t = 1;
			else
				t = 2;

			int hlt = std::lround(season - ((coorY + 1) % 6) + t); //Ôîðìóëà âû÷èñëåíèÿ ýíåðãèè
			if (hlt >= 3)
				incIP(1);
			else
				incIP(2);
		}

		/*Ïðîâåðêà íà ïðèõîä ìèíåðàëîâ
		  Åñëè
			ïðèáàâëÿåòñÿ     +1
			íå ïðèáàâëÿåòñÿ  +2
		*/
		else if (command == 48) {
			if (coorY > MAX_Y / 2)
				incIP(1);
			else
				incIP(2);
		}

		/*Ïðîâåðêà íà ìíîãîêëåòî÷íîñòü
		  Åñëè
			ñâîáîäíîæèâóùèé  +1
			ñ êðàþ öåïî÷êè   +2
			âíóòðè öåïî÷êè   +3
		*/
		else if (command == 49) {
			if (chainNext == -1 && chainPrev == -1)
				incIP(1);
			else if (chainNext == -1 || chainPrev == -1)
				incIP(2);
			else
				incIP(3);
		}

		//Ïðåîáðàçîâàíèå ìèíåðàëîâ â ýíåðãèþ
		else if (command == 50) {
			if (minrNum > 100) {// ìàêñèìàëüíîå êîëè÷åñòâî ìèíåðàëîâ, êîòîðûå ìîæíî ïðåîáðàçîâàòü â ýíåðãèþ = 100
				minrNum -= 100; // 1 ìèíåðàë = 4 ýíåðãèè
				energy += 400;  //
				goBlue(100);    // áîò îò ýòîãî ñèíååò
			}
			else {  // åñëè ìèíåðàëîâ ìåíüøå 100, òî âñå ìèíåðàëû ïåðåõîäÿò â ýíåðãèþ
				energy += minrNum * 4;
				goBlue(minrNum);
				minrNum = 0;
			}
			break;
		}

		//Ñîçäàíèå ìíîãîêëåòî÷íîãî
		else if (command == 51) {
			energy -= 150; //Áîò çàòðà÷èâàåò 150 åäèíèö ýíåðãèè
			if (energy < 0) break;

			int a = -1; //Ïåðåìåííàÿ ñâîáîäíîãî íàïðàâëåíèÿ
			for (unsigned short i = 0; i < 8; i++) {
				unsigned int x = getX(i);
				unsigned int y = getY(i);
				if (world[x][y] == empty) {
					a = i;
					break;
				}
			}
			if (a == -1) { //Åñëè íåò ñâîáîäíîãî ìåñòà
				energy = 0; //Áîò ïîãèáàåò
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
		//Íà÷àëî áëîêà "êîìïüþòåðíûõ" êîìàíä
		////////////////////////////////////

		//Ïîìåùåíèå ÷èñëà â ðåãèñòð A
		else if (command == 52) {
			unsigned short param = getParam();

			registers[0] = param;
			incIP(1);
		}

		//Ïîìåùåíèå ÷èñëà â ðåãèñòð B
		else if (command == 53) {
			unsigned short param = getParam();

			registers[1] = param;
			incIP(1);
		}

		//Ïîìåùåíèå ÷èñëà â ðåãèñòð C
		else if (command == 54) {
			unsigned short param = getParam();

			registers[2] = param;
			incIP(1);
		}

		//Ïîìåùåíèå ÷èñëà â ðåãèñòð D
		else if (command == 55) {
			unsigned short param = getParam();

			registers[3] = param;
			incIP(1);
		}

		//Ïîìåùåíèå ÷èñëà â ðåãèñòð E
		else if (command == 56) {
			unsigned short param = getParam();

			registers[4] = param;
			incIP(1);
		}

		//Ïîìåùåíèå ÷èñëà â ðåãèñòð F
		else if (command == 57) {
			unsigned short param = getParam();

			registers[5] = param;
			incIP(1);
		}

		//Ïîìåùåíèå ÷èñëà â ðåãèñòð G
		else if (command == 58) {
			unsigned short param = getParam();

			registers[6] = param;
			incIP(1);
		}

		//Ïîìåùåíèå ÷èñëà â ðåãèñòð H
		else if (command == 59) {
			unsigned short param = getParam();

			registers[7] = param;
			incIP(1);
		}

		////////////////////////////////////

		//Ïîìåùåíèå ÷èñëà èç ðåãèñòðà A
		else if (command == 60) {
			incIP(1);
			DNA[IP] = registers[0];
		}

		//Ïîìåùåíèå ÷èñëà èç ðåãèñòðà B
		else if (command == 61) {
			incIP(1);
			DNA[IP] = registers[1];
		}

		//Ïîìåùåíèå ÷èñëà èç ðåãèñòðà C
		else if (command == 62) {
			incIP(1);
			DNA[IP] = registers[2];
		}

		//Ïîìåùåíèå ÷èñëà èç ðåãèñòðà D
		else if (command == 63) {
			incIP(1);
			DNA[IP] = registers[3];
		}

		//Ïîìåùåíèå ÷èñëà èç ðåãèñòðà E
		else if (command == 64) {
			incIP(1);
			DNA[IP] = registers[4];
		}

		//Ïîìåùåíèå ÷èñëà èç ðåãèñòðà F
		else if (command == 65) {
			incIP(1);
			DNA[IP] = registers[5];
		}

		//Ïîìåùåíèå ÷èñëà èç ðåãèñòðà G
		else if (command == 66) {
			incIP(1);
			DNA[IP] = registers[6];
		}

		//Ïîìåùåíèå ÷èñëà èç ðåãèñòðà H
		else if (command == 67) {
			incIP(1);
			DNA[IP] = registers[7];
		}

		////////////////////////////////////

		//Îáìåí çíà÷åíèé ïàðàìåòðà è ðåãèñòðà A
		else if (command == 68) {
			incIP(1);
			std::swap(DNA[IP], registers[0]);
		}

		//Îáìåí çíà÷åíèé ïàðàìåòðà è ðåãèñòðà B
		else if (command == 69) {
			incIP(1);
			std::swap(DNA[IP], registers[1]);
		}

		//Îáìåí çíà÷åíèé ïàðàìåòðà è ðåãèñòðà C
		else if (command == 70) {
			incIP(1);
			std::swap(DNA[IP], registers[2]);
		}

		//Îáìåí çíà÷åíèé ïàðàìåòðà è ðåãèñòðà D
		else if (command == 71) {
			incIP(1);
			std::swap(DNA[IP], registers[3]);
		}

		//Îáìåí çíà÷åíèé ïàðàìåòðà è ðåãèñòðà E
		else if (command == 72) {
			incIP(1);
			std::swap(DNA[IP], registers[4]);
		}

		//Îáìåí çíà÷åíèé ïàðàìåòðà è ðåãèñòðà F
		else if (command == 73) {
			incIP(1);
			std::swap(DNA[IP], registers[5]);
		}

		//Îáìåí çíà÷åíèé ïàðàìåòðà è ðåãèñòðà G
		else if (command == 74) {
			incIP(1);
			std::swap(DNA[IP], registers[6]);
		}

		//Îáìåí çíà÷åíèé ïàðàìåòðà è ðåãèñòðà H
		else if (command == 75) {
			incIP(1);
			std::swap(DNA[IP], registers[7]);
		}

		////////////////////////////////////

		//Êîïèðîâàíèå èç ðåãèñòðà A â ïàðàìåòîðíûé
		else if (command == 76) {
			unsigned short param = getParam() % 8;

			registers[param] = registers[0];
			incIP(1);
		}

		//Êîïèðîâàíèå èç ðåãèñòðà B â ïàðàìåòîðíûé
		else if (command == 77) {
			unsigned short param = getParam() % 8;

			registers[param] = registers[1];
			incIP(1);
		}

		//Êîïèðîâàíèå èç ðåãèñòðà C â ïàðàìåòîðíûé
		else if (command == 78) {
			unsigned short param = getParam() % 8;

			registers[param] = registers[2];
			incIP(1);
		}

		//Êîïèðîâàíèå èç ðåãèñòðà D â ïàðàìåòîðíûé
		else if (command == 79) {
			unsigned short param = getParam() % 8;

			registers[param] = registers[3];
			incIP(1);
		}

		//Êîïèðîâàíèå èç ðåãèñòðà E â ïàðàìåòîðíûé
		else if (command == 80) {
			unsigned short param = getParam() % 8;

			registers[param] = registers[4];
			incIP(1);
		}

		//Êîïèðîâàíèå èç ðåãèñòðà F â ïàðàìåòîðíûé
		else if (command == 81) {
			unsigned short param = getParam() % 8;

			registers[param] = registers[5];
			incIP(1);
		}

		//Êîïèðîâàíèå èç ðåãèñòðà G â ïàðàìåòîðíûé
		else if (command == 82) {
			unsigned short param = getParam() % 8;

			registers[param] = registers[6];
			incIP(1);
		}

		//Êîïèðîâàíèå èç ðåãèñòðà H â ïàðàìåòîðíûé
		else if (command == 83) {
			unsigned short param = getParam() % 8;

			registers[param] = registers[7];
			incIP(1);
		}

		////////////////////////////////////

		//Îáìåí çíà÷åíèé ðåãèñòðà À è ïàðàìåòîðíîãî
		else if (command == 84) {
			unsigned short param = getParam() % 8;
		
			std::swap(registers[param], registers[0]);
			incIP(1);
		}
		
		//Îáìåí çíà÷åíèé ðåãèñòðà B è ïàðàìåòîðíîãî
		else if (command == 85) {
			unsigned short param = getParam() % 8;

			std::swap(registers[param], registers[1]);
			incIP(1);
		}

		//Îáìåí çíà÷åíèé ðåãèñòðà C è ïàðàìåòîðíîãî
		else if (command == 86) {
			unsigned short param = getParam() % 8;

			std::swap(registers[param], registers[2]);
			incIP(1);
		}

		//Îáìåí çíà÷åíèé ðåãèñòðà D è ïàðàìåòîðíîãî
		else if (command == 87) {
			unsigned short param = getParam() % 8;

			std::swap(registers[param], registers[3]);
			incIP(1);
		}

		//Îáìåí çíà÷åíèé ðåãèñòðà E è ïàðàìåòîðíîãî
		else if (command == 88) {
			unsigned short param = getParam() % 8;

			std::swap(registers[param], registers[4]);
			incIP(1);
		}

		//Îáìåí çíà÷åíèé ðåãèñòðà F è ïàðàìåòîðíîãî
		else if (command == 89) {
			unsigned short param = getParam() % 8;

			std::swap(registers[param], registers[5]);
			incIP(1);
		}

		//Îáìåí çíà÷åíèé ðåãèñòðà G è ïàðàìåòîðíîãî
		else if (command == 90) {
			unsigned short param = getParam() % 8;

			std::swap(registers[param], registers[6]);
			incIP(1);
		}

		//Îáìåí çíà÷åíèé ðåãèñòðà H è ïàðàìåòîðíîãî
		else if (command == 91) {
			unsigned short param = getParam() % 8;

			std::swap(registers[param], registers[7]);
			incIP(1);
		}

		////////////////////////////////////

		//Èíêðåìåíòèðîâàíèå ïàðàìåòîðíîãî ðåãèñòðà
		else if (command == 92) {
			unsigned short param = getParam() % 8;

			registers[param]++;
			incIP(1);
		}

		//Äåêðåìåíòèðîâàíèå ïàðàìåòîðíîãî ðåãèñòðà
		else if (command == 93) {
			unsigned short param = getParam() % 8;

			registers[param]--;
			incIP(1);
		}

		//Cäâèã áèòîâ âëåâî
		else if (command == 102) {
			unsigned short param = getParam() % 8;
#pragma warning(push)
#pragma warning(disable : 4244)
			unsigned char a = registers[param];
#pragma warning(pop)
			registers[param] = a << 1;
			incIP(1);
		}

		//Cäâèã áèòîâ âïðàâî
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
		
		/*Ñðàâíåíèå ðåãèñòðà À è ïàðàìåòðîâîãî
		   Åñëè
			 îäèíàêîâû       +2
			 ðàçëè÷íû        +3
		*/
		else if (command == 94) {
			unsigned short param = getParam() % 8;
			
			if (registers[0] == registers[param])
				incIP(2);
			else
				incIP(3);
		}

		/*Ñðàâíåíèå ðåãèñòðà B è ïàðàìåòðîâîãî
			Åñëè
			 îäèíàêîâû       +2
			 ðàçëè÷íû        +3
		*/
		else if (command == 95) {
			unsigned short param = getParam() % 8;

			if (registers[1] == registers[param])
				incIP(2);
			else
				incIP(3);
		}

		/*Ñðàâíåíèå ðåãèñòðà C è ïàðàìåòðîâîãî
			Åñëè
			 îäèíàêîâû       +2
			 ðàçëè÷íû        +3
		*/
		else if (command == 96) {
			unsigned short param = getParam() % 8;

			if (registers[2] == registers[param])
				incIP(2);
			else
				incIP(3);
		}

		/*Ñðàâíåíèå ðåãèñòðà D è ïàðàìåòðîâîãî
			Åñëè
			 îäèíàêîâû       +2
			 ðàçëè÷íû        +3
		*/
		else if (command == 97) {
			unsigned short param = getParam() % 8;

			if (registers[3] == registers[param])
				incIP(2);
			else
				incIP(3);
		}

		/*Ñðàâíåíèå ðåãèñòðà E è ïàðàìåòðîâîãî
			Åñëè
			 îäèíàêîâû       +2
			 ðàçëè÷íû        +3
		*/
		else if (command == 98) {
			unsigned short param = getParam() % 8;

			if (registers[4] == registers[param])
				incIP(2);
			else
				incIP(3);
		}

		/*Ñðàâíåíèå ðåãèñòðà F è ïàðàìåòðîâîãî
			Åñëè
			 îäèíàêîâû       +2
			 ðàçëè÷íû        +3
		*/
		else if (command == 99) {
			unsigned short param = getParam() % 8;

			if (registers[5] == registers[param])
				incIP(2);
			else
				incIP(3);
		}

		/*Ñðàâíåíèå ðåãèñòðà G è ïàðàìåòðîâîãî
			Åñëè
			 îäèíàêîâû       +2
			 ðàçëè÷íû        +3
		*/
		else if (command == 100) {
			unsigned short param = getParam() % 8;

			if (registers[6] == registers[param])
				incIP(2);
			else
				incIP(3);
		}

		/*Ñðàâíåíèå ðåãèñòðà H è ïàðàìåòðîâîãî
			Åñëè
			 îäèíàêîâû       +2
			 ðàçëè÷íû        +3
		*/
		else if (command == 101) {
			unsigned short param = getParam() % 8;

			if (registers[7] == registers[param])
				incIP(2);
			else
				incIP(3);
		}

		////////////////////////////////////

		/*Áåçóñëîâíûé ïåðåõîä, åñëè ðåãèñòð À ðàâåí 0
		   Åñëè íå ðàâåí     +2
		*/
		else if (command = 104) {
			if (registers[0] == 0)
				IP = getParam();
			else
				incIP(2);
		}

		/*Áåçóñëîâíûé ïåðåõîä, åñëè ðåãèñòð B ðàâåí 0
		   Åñëè íå ðàâåí     +2
		*/
		else if (command = 105) {
			if (registers[1] == 0)
				IP = getParam();
			else
				incIP(2);
		}

		/*Áåçóñëîâíûé ïåðåõîä, åñëè ðåãèñòð C ðàâåí 0
			Åñëè íå ðàâåí     +2
		*/
		else if (command = 106) {
			if (registers[2] == 0)
				IP = getParam();
			else
				incIP(2);
		}

		/*Áåçóñëîâíûé ïåðåõîä, åñëè ðåãèñòð D ðàâåí 0
			Åñëè íå ðàâåí     +2
		*/
		else if (command = 107) {
			if (registers[3] == 0)
				IP = getParam();
			else
				incIP(2);
		}

		/*Áåçóñëîâíûé ïåðåõîä, åñëè ðåãèñòð E ðàâåí 0
			Åñëè íå ðàâåí     +2
		*/
		else if (command = 108) {
			if (registers[4] == 0)
				IP = getParam();
			else
				incIP(2);
		}

		/*Áåçóñëîâíûé ïåðåõîä, åñëè ðåãèñòð F ðàâåí 0
			Åñëè íå ðàâåí     +2
		*/
		else if (command = 109) {
			if (registers[5] == 0)
				IP = getParam();
			else
				incIP(2);
		}

		/*Áåçóñëîâíûé ïåðåõîä, åñëè ðåãèñòð G ðàâåí 0
			Åñëè íå ðàâåí     +2
		*/
		else if (command = 110) {
			if (registers[6] == 0)
				IP = getParam();
			else
				incIP(2);
		}

		/*Áåçóñëîâíûé ïåðåõîä, åñëè ðåãèñòð H ðàâåí 0
			Åñëè íå ðàâåí     +2
		*/
		else if (command = 111) {
			if (registers[7] == 0)
				IP = getParam();
			else
				incIP(2);
		}
		
		////////////////////////////////////
		////////////////////////////////////

		/*Êîìàíäà âñòàâêè ïàðàìåòðà â ñòåê
		   Åñëè
			 Óäà÷íî           +2
			 Ïåðåïîëíåí       +3
		*/
		else if (command == 112) {
			unsigned short param = getParam();
			if (heapPtr == 8)
				incIP(3);
				//energy -= 10; //Äëÿ ìîòèâàöèè ;)
			else {
				stack[heapPtr] = param;
				heapPtr++;
				incIP(2);
			}
		}

		/*Êîìàíàäà âçÿòèÿ èç ñòåêà â ïàðàìåòðîâûé ðåãèñòð
			Åñëè
			 Óäà÷íî          +2
			 Ïóñò            +3
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

		/*Êàäðîâûé ïðîñìîòð ñòåêà
		  Ïîìåùåíèå çíà÷åíèÿ ñòåêà ñ ïàðàìåòðà â ðåãèñòð À
		   Åñëè
			 Óäà÷íî          +2
			 Ïóñò            +3
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

		/*Êàäðîâûé ïðîñìîòð ñòåêà
		  Ïîìåùåíèå çíà÷åíèÿ ñòåêà ñ ïàðàìåòðà â ðåãèñòð B
			Åñëè
			 Óäà÷íî          +2
			 Ïóñò            +3
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

		/*Êàäðîâûé ïðîñìîòð ñòåêà
		  Ïîìåùåíèå çíà÷åíèÿ ñòåêà ñ ïàðàìåòðà â ðåãèñòð C
			Åñëè
			 Óäà÷íî          +2
			 Ïóñò            +3
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

		/*Êàäðîâûé ïðîñìîòð ñòåêà
		  Ïîìåùåíèå çíà÷åíèÿ ñòåêà ñ ïàðàìåòðà â ðåãèñòð D
			Åñëè
			 Óäà÷íî          +2
			 Ïóñò            +3
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

		/*Êàäðîâûé ïðîñìîòð ñòåêà
	     Ïîìåùåíèå çíà÷åíèÿ ñòåêà ñ ïàðàìåòðà â ðåãèñòð E
			Åñëè
			 Óäà÷íî          +2
			 Ïóñò            +3
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

		/*Êàäðîâûé ïðîñìîòð ñòåêà
		  Ïîìåùåíèå çíà÷åíèÿ ñòåêà ñ ïàðàìåòðà â ðåãèñòð F
			Åñëè
			 Óäà÷íî          +2
			 Ïóñò            +3
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

		/*Êàäðîâûé ïðîñìîòð ñòåêà
		  Ïîìåùåíèå çíà÷åíèÿ ñòåêà ñ ïàðàìåòðà â ðåãèñòð G
			Åñëè
			 Óäà÷íî          +2
			 Ïóñò            +3
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

		/*Êàäðîâûé ïðîñìîòð ñòåêà
		  Ïîìåùåíèå çíà÷åíèÿ ñòåêà ñ ïàðàìåòðà â ðåãèñòð H
			Åñëè
			 Óäà÷íî          +2
			 Ïóñò            +3
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

		//Îáìåí çíà÷åíèé ñòåêà è ïàðàìåòðîâîãî ðåãèñòðà
		else if (command == 122) {
			if (heapPtr > 0)
				std::swap(stack[heapPtr - 1], registers[(getParam() % 8)]);
		}

		////////////////////////////////////

		//Ïåðåõîä ïî àäðåñó ñ âåðõóøêè ñòåêà
		//(Åñëè îí ðàâåí -1, ïåðåéòè íà ñëó÷àéíûé)
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

		//Çàïèñü òåêóùåãî àäðåñà â ñòåê	
		//Ïðè íåóäà÷å       +2
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

		//Åñëè íå ÿâëÿåòñÿ êîìàíäîé, ñîâåðøèòü óñëîâíûé ïåðåõîä
		else
			incIP(command);
	}
	else
		condition = alive;

	//
	//Çàâåðøåíèå øàãà
	//

	energy -= 4;

		//Åñëè áîò â öåïî÷êå
		if (chainNext != -1 && chainPrev != -1) {
			//Äåëèì ìèíåðàëû
			unsigned int min = minrNum + bots[chainNext].minrNum + bots[chainPrev].minrNum;
			minrNum = (min / 3) + (min % 3);
			bots[chainNext].minrNum = min;
			bots[chainPrev].minrNum = min;

			//Äåëèì ýíåðãèþ
			//
			//Ïðîâåðèì, ÿâëÿþòñÿ ëè ñëåäóþùèé è ïðåäûäóùèé áîòû â öåïî÷êå êðàéíèìè
			//Åñëè îíè íå ÿâëÿþòñÿ êðàéíèìè, òî ðàñïðåäåëÿåì ýíåðãèþ ïîðîâíó
			//Ñâÿçàíî ýòî ñ òåì, ÷òî â êðàéíèõ áîòàõ â öåïî÷êå äîëæíî áûòü áîëüøå ýíåðãèè
			//×òîáû îíè ïëîäèëè íîâûõ áîòîâ è óäëèíÿëè öåïî÷êó
			if ((bots[chainNext].chainNext != -1 || bots[chainNext].chainPrev != -1) && (bots[chainPrev].chainNext != -1 || bots[chainPrev].chainPrev != -1)) {
				unsigned int hlt = (energy + bots[chainNext].energy + bots[chainPrev].energy);
				energy = (min / 3) + (min % 3);
				bots[chainNext].energy = hlt;
				bots[chainPrev].energy = hlt;
			}
		}
		//Åñëè áîò èìååò ïðåäûäóùåãî â öåïî÷êå
		else if (chainPrev != -1) { 
			//Åñëè ïðåäûäóùèé íå ÿâëÿåòñÿ êðàéíèì â öåïî÷êå
			if (bots[chainPrev].chainNext != -1 && bots[chainPrev].chainPrev != -1) {
				//Òî ðàñïðåäåëÿåì ýíåðãèþ â ïîëüçó òåêóù¸ãî áîòà
				unsigned int hlt = energy + bots[chainPrev].energy;
				bots[chainPrev].energy = (hlt / 4) + (hlt % 4);
				energy = hlt / 4 * 3;
			}
		}
		//Åñëè áîò èìååò ïðåäûäóùåãî â öåïî÷êå
		else if (chainNext != -1) { 
			//Åñëè ïðåäûäóùèé íå ÿâëÿåòñÿ êðàéíèì â öåïî÷êå
			if (bots[chainNext].chainNext != -1 && bots[chainNext].chainPrev != -1) {
				//Òî ðàñïðåäåëÿåì ýíåðãèþ â ïîëüçó òåêóù¸ãî áîòà
				unsigned int hlt = energy + bots[chainNext].energy;
				bots[chainNext].energy = (hlt / 4) + (hlt % 4);
				energy = hlt / 4 * 3;
			}
		}

		//Åñëè ýíåðãèè áîëüøå 600, òî ïëîäèì íîâîãî áîòà
		if (energy > 600) {
			int a = -1; //Ïåðåìåííàÿ ñâîáîäíîãî íàïðàâëåíèÿ
			for (unsigned short i = 0; i < 8; i++) {
				unsigned int x = getX(i);
				unsigned int y = getY(i);
				if (world[x][y] == empty) {
					a = i;
					break;
				}
			}
			if (a == -1) //Åñëè íåò ñâîáîäíîãî ìåñòà
				energy = 0; //Áîò ïîãèáàåò
			else {
				if (b != nullptr)
					delete b;
				if (chainNext == -1 && chainPrev == -1)
					b = new bot(getX(a), getY(a), this, n + 1, FREE);
				else
					b = new bot(getX(a), getY(a), this, n + 1, CHAIN);
			}
		}
		//Åñëè ýíåðãèè ñòàëî ìåíüøå 1
		else if (energy < 1) {
			condition = organic_sink; //Îòìå÷àåì êàê îðãàíèêó
			if (chainPrev != -1)      //Åñëè ñîñòîèò âî ìíîãîêëåòî÷íîé öåïî÷êå - óäàëÿåì
				bots[chainPrev].chainNext = -1;
			if (chainNext != -1)
				bots[chainNext].chainPrev = -1;
		}

#pragma warning(push)
#pragma warning(disable : 4018)
		//Åñëè áîò ãëóáæå, ÷åì MAX_Y / 2, òî îí íà÷èíàåò íàêàïëèâàòü ìèíåðàëû
		if (coorY > std::lround(MAX_Y / 2))
			minrNum++;
		else if (coorY > std::lround(MAX_Y / 2 + MAX_Y / 4))
			minrNum += 2;
		else if (coorY > std::lround(MAX_Y - MAX_Y / 10))
			minrNum += 3;
#pragma warning(pop)

		//Ïðîâåðêà ïðåäåëà
		if (minrNum > 999)
			minrNum = 999;
}

void bot::death() {
	world[coorX][coorY] = empty; //Óäàëåíèå áîòà ñ êàðòû
	auto a = bots.begin() + n;   //Èç âåêòîðà
	bots.erase(a);               //
	for (size_t i = n; i < bots.size(); i++) //Îáíîâëåíèå èòåðàòîðîâ
		bots[i].n = i;

	if (chainPrev > 0)           //Åñëè âî ìíîãîêëåòî÷íîé öåïî÷êå - òîæå óäàëÿåì
		bots[chainPrev].chainNext = -1;
	if (chainNext > 0)
		bots[chainNext].chainPrev = -1;
}

//Íåîáÿçàòåëüíàÿ ôóíêöèÿ
//Èçìåíÿåò îäèí ñëó÷àéíûé ãåí ó áîòîâ,
//íàõîäÿùèõñÿ íà ñàìîì âåðõíåì óðîâíå
void radiation() {
	for (size_t i = 0; i < WORLD_WIDTH; i++)
		if (world[i][1] != empty)
			bots[world[i][1]].DNA[getRandomNumber(0, 255)] = getRandomNumber(0, 255);
}
