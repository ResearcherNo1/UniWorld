//���� �������� ���������� �����
//
#include "bot.h"

extern size_t world[WORLD_WIDTH][((unsigned long long)WORLD_HEIGHT)];
extern std::vector<bot> bots;
extern int season;
bot* b;

bot::bot(unsigned int X, unsigned int Y, bot* parent, size_t N, bool free) {
	srand();
	std::vector<bot>::iterator a = bots.begin() + N;
	bots.insert(a, *this);
	for (size_t i = N; i < bots.size(); i++) //���������� ����������
		bots[i].n = i;

	//���� ��� ���������
	if (parent == nullptr) {
		for (size_t i = 0; i < DNA_SIZE; i++) //��������� ��� �������� ����������
			DNA[i] = 25;
		energy = 990;
		minrNum = 0;
		chainNext = chainPrev = -1;
		red = green = blue = 170;
	}
	else {
		//� ����� ������ �� ������
		if (getRandomNumber(1, 4) == 2) {
			//��������� ��������� �������
			size_t mut_ptr = getRandomNumber(0, 248);
			//�������� ����� ��������
			for (size_t i = 0; i < DNA_SIZE; i++)
				DNA[i] = parent->DNA[i];

			const size_t abc = mut_ptr + 4;

			//��������
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
		if (parent->chainNext == -1) { //E��� � ����-������ ������ �� ���������� ���� � �������������� ������� �����
			parent->chainNext = N; //�� ��������� ���� �������������� ����
			chainPrev = parent->n; //� �������������� ������ �� ����������� ��������� �� ����-������
			chainNext = -1;        //������ �� ���������� �����, ������������� ��� �������� ������� � �������
		}
		else //���� � ����-������ ������ �� ����������� ���� � �������������� ������� ����� 
		{
			parent->chainPrev = N; // �� ��������� ���� �������������� ����
			chainNext = parent->n; // � �������������� ������ �� ���������� ��������� �� ����-������
			chainPrev = -1;        // ������ �� ����������� �����, ������������� ��� �������� ������� � ������� 
		}
#endif
	}
	//��������� ���������� � ����� ������
	IP = 0;

	//������� ���� � ��������
	for (size_t i = 0; i < SUB_SIZE; i++) {
		stack[i] = 0;
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
	if (_bot.condition < alive) //���� _bot - ��������
		return false;

	size_t dif = 0; //C������ ������������ � ������
	for (size_t i = 0; i < DNA_SIZE; i++) //��������� ������� ������
		if (DNA[i] != _bot.DNA[i]) {
			dif++;
			if (dif > 8) return false; //���� ������������ > 8, �� ���� �� ������������
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
	//���� ��� - ��������, �� ������� �� �������
	if (condition = organic_hold)
		return;
	if (condition = organic_sink) { //������� ��������
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
	//���� �� �����, �� ��� �����
	//

	//���� ��� �� ��������� � ��������� ����� �������
	if (!(condition == input))
		for (unsigned short cyc = 0; cyc < 25; cyc++) { //��������� �� ����� 25 ������ �� ���
		unsigned short command = DNA[IP]; //������� �������

		//������������� ����� �����������
		if (command == 23) {
			unsigned short param;
			if (IP = 255) {
				IP = 0;
				param = (DNA[IP] % 16);
			}
			else {
				param = (DNA[++IP] % 16); //��������� ��������� �� �������� ���� � ��������� ������� �� ������� �� 16
				param += direct; //���������� ����� ���������� � �������� ����������� ����
			}

			if (param > 7) //��������� ������ ���� � �������� 0 ... 7
				param -= 8;
			direct = static_cast<drct>(param);
			incIP(1);
		}

		//���������� ����� �����������
		else if (command == 24) {
			unsigned short param;
			if (IP = 255) {
				IP = 0;
				param = (DNA[IP] % 16);
			}
			else
				param = (DNA[++IP] % 16); //��������� ��������� �� �������� ���� � ��������� ������� �� ������� �� 16

			direct = static_cast<drct>(param);
			incIP(1);
		}

		//����������
		else if (command == 25) {
			double t = 0;
			//���������� ������������ ���������
			if (minrNum < 100)
				t = 0;
			else if (minrNum < 400)
				t = 1;
			else
				t = 2;

			int hlt = std::lround(season - ((coorY + 1) % 6) + t); //������� ���������� �������
			if (hlt > 0) {
				energy += hlt; //���������� ���������� ������� � ������� ����
				goGreen(hlt); //��� �� ����� ��������
			}
			incIP(1);
			break; //�������, ��� ��� ������� "����������" - �����������
		}

		/*��� � ������������� �����������
			����
			 ������� ������� +2
			 �����           +3
			 ��������        +4
			 ���             +5
			 �����           +6
		*/
		else if (command == 26) {
			//���� ��� ��������������
			if (chainPrev != -1 || chainNext != -1)
				//������ �� ��������� - ������ ����� ������ �������������
				incIP(1);
			else {
				unsigned short param;
				if (IP = 255) {
					IP = 0;
					param = (DNA[IP] % 16);
				}
				else
					param = (DNA[++IP] % 16); //��������� ��������� �� �������� ���� � ��������� ������� �� ������� �� 16

				unsigned int x = getX(param + direct); //�������� ���������� ������
				unsigned int y = getY(param + direct); //

				if (world[x][y] == empty) { //���� �� ������ �����
					world[x][y] = n;
					world[coorX][coorY] = empty;
					coorX = x;
					coorY = y;
					incIP(2);
				}
				else if (world[x][y] == wall) { //���� �� ������ �����
					incIP(3);
				}
				else if (bots[world[x][y]].condition <= organic_sink) { //���� �� ������ ��������
					incIP(4);
				}
				else if (isRelative(bots[world[x][y]])) { //���� �� ������ �����
					incIP(6);
				}
				else { //���������� ������� - �� ������ ���
					incIP(5);
				}
			}
			break; //�������, ��� ��� ������� "���" - �����������
		} //

		/*��� � ���������� �����������
			����
			 ������� ������� +2
			 �����           +3
			 ��������        +4
			 ���             +5
			 �����           +6
		*/
		else if (command == 27) {
			//���� ��� ��������������
			if (chainPrev != -1 || chainNext != -1)
				//������ �� ��������� - ������ ����� ������ �������������
				incIP(1);
			else {
				unsigned short param;
				if (IP = 255) {
					IP = 0;
					param = (DNA[IP] % 16);
				}
				else
					param = (DNA[++IP] % 16); //��������� ��������� �� �������� ���� � ��������� ������� �� ������� �� 16

				unsigned int x = getX(param); //�������� ���������� ������
				unsigned int y = getY(param); //

				if (world[x][y] == empty) { //���� �� ������ �����
					world[x][y] = n;
					world[coorX][coorY] = empty;
					coorX = x;
					coorY = y;
					incIP(2);
				}
				else if (world[x][y] == wall) { //���� �� ������ �����
					incIP(3);
				}
				else if (bots[world[x][y]].condition <= organic_sink) { //���� �� ������ ��������
					incIP(4);
				}
				else if (isRelative(bots[world[x][y]])) { //���� �� ������ �����
					incIP(6);
				}
				else { //���������� ������� - �� ������ ���
					incIP(5);
				}
			}
			break;

		}

		/*C����� � ������������� �����������
			����
			 �����           +2
			 �����           +3
			 ��������        +4
			 ���             +5
		*/
		else if (command == 28) {
			unsigned short param;
			if (IP = 255) {
				IP = 0;
				param = (DNA[IP] % 16);
			}
			else
				param = (DNA[++IP] % 16); //��������� ��������� �� �������� ���� � ��������� ������� �� ������� �� 16

			energy -= 4; //��� �� ���� �����, ���������� �� ����������, ������ 4 �������
			unsigned int x = getX(param + direct); //�������� ���������� ������
			unsigned int y = getY(param + direct); //


			if (world[x][y] == empty) { //���� �����
				incIP(2);
			}
			else if (world[x][y] == wall) { //���� �����
				incIP(3);
			}
			else if (bots[world[x][y]].condition <= organic_sink) { //���� ��� ��������� ��������
				bots[world[x][y]].~bot(); //������� ������
				energy += 100;            //�������� � �������
				goRed(100);               //��� ��������
				incIP(4);
			}
			else {
				//
				//--------- ����� �� ����, ������ ������� ����� ��� -------------------
				//
				int min0 = minrNum; //���-�� ��������� � ����
				int min1 = bots[world[x][y]].minrNum; //���-�� ��������� � �������������� �����
				int hl = bots[world[x][y]].energy;  //���-�� �������

				//���� � ���� ��������� ������
				if (min0 >= min1) {
					minrNum -= min1;  //���������� ��������� � ���� ����������� �� ���������� ��������� � ������
													  //����, ������ ���� ���� � ������� ������)
					bots[world[x][y]].~bot(); //������� ������
					energy += 100 + (hl / 2); //K��������� ������� � ���� ������������ �� 100 + (�������� �� ������� ������)
					goRed(hl);                //��� ��������
					incIP(5);
				}
				//���� ���� ����� ������������
				else if (min1 > min0) {
					minrNum = 0;  //�� ��� ������������ ��� �������� �� ����������� ������
					min1 -= min0; //�� � � ����� ��� �����������
					//���� ������� � 2 ���� ������
					if (energy >= 2 * min1) {
						bots[world[x][y]].~bot(); //������� ������
						energy += 100 + (hl / 2) - 2 * min1;
						goRed(hl);                //��� ��������
						incIP(5);
					}
					//���� ������� ������, ��� (��������� � ������)*2, �� ��� �������� �� ������ 
					else {
						bots[world[x][y]].minrNum = min1 - (energy / 2); //������ ����������� ���� ��������
						energy = -10; //������� ������ � �����
						incIP(5);
					}
				}
			}
			break;
		}

		/*C����� � ���������� �����������
			����
			 �����           +2
			 �����           +3
			 ��������        +4
			 ���             +5
		*/
		else if (command == 29) {
			unsigned short param;
			if (IP = 255) {
				IP = 0;
				param = (DNA[IP] % 16);
			}
			else
				param = (DNA[++IP] % 16); //��������� ��������� �� �������� ���� � ��������� ������� �� ������� �� 16

			energy -= 4; //��� �� ���� �����, ���������� �� ����������, ������ 4 �������
			unsigned int x = getX(param); //�������� ���������� ������
			unsigned int y = getY(param); //


			if (world[x][y] == empty) { //���� �����
				incIP(2);
			}
			else if (world[x][y] == wall) { //���� �����
				incIP(3);
			}
			else if (bots[world[x][y]].condition <= organic_sink) { //���� ��� ��������� ��������
				bots[world[x][y]].~bot(); //������� ������
				energy += 100;            //�������� � �������
				goRed(100);               //��� ��������
				incIP(4);
			}
			else {
				//
				//--------- ����� �� ����, ������ ������� ����� ��� -------------------
				//
				int min0 = minrNum; //���-�� ��������� � ����
				int min1 = bots[world[x][y]].minrNum; //���-�� ��������� � �������������� �����
				int hl = bots[world[x][y]].energy;  //���-�� �������

				//���� � ���� ��������� ������
				if (min0 >= min1) {
					minrNum -= min1;  //���������� ��������� � ���� ����������� �� ���������� ��������� � ������
													  //����, ������ ���� ���� � ������� ������)
					bots[world[x][y]].~bot(); //������� ������
					energy += 100 + (hl / 2); //K��������� ������� � ���� ������������ �� 100 + (�������� �� ������� ������)
					goRed(hl);                //��� ��������
					incIP(5);
				}
				//���� ���� ����� ������������
				else if (min1 > min0) {
					minrNum = 0;  //�� ��� ������������ ��� �������� �� ����������� ������
					min1 -= min0; //�� � � ����� ��� �����������
					//���� ������� � 2 ���� ������
					if (energy >= 2 * min1) {
						bots[world[x][y]].~bot(); //������� ������
						energy += 100 + (hl / 2) - 2 * min1;
						goRed(hl);                //��� ��������
						incIP(5);
					}
					//���� ������� ������, ��� (��������� � ������)*2, �� ��� �������� �� ������ 
					else {
						bots[world[x][y]].minrNum = min1 - (energy / 2); //������ ����������� ���� ��������
						energy = -10; //������� ������ � �����
						incIP(5);
					}
				}
			}
			break;
		}

		/*���������� � ������������� �����������
			����
			 �����           +2
			 �����           +3
			 ��������        +4
			 ���             +5
			 �����           +6
		*/
		else if (command == 30) {
			unsigned short param;
			if (IP = 255) {
				IP = 0;
				param = (DNA[IP] % 16);
			}
			else
				param = (DNA[++IP] % 16); //��������� ��������� �� �������� ���� � ��������� ������� �� ������� �� 16

			unsigned int x = getX(param + direct); //��������� ���������� ������
			unsigned int y = getY(param + direct); //
			size_t wc = world[x][y]; //���������� ������

			if (wc == empty) { //���� �����
				incIP(2);
			}
			else if (wc == wall) { //���� �����
				incIP(3);
			}
			else if (bots[wc].condition <= organic_sink) { //���� ��������
				incIP(4);
			}
			else if (isRelative(bots[wc])) { //���� �����
				incIP(6);
			}
			else { //���� ���
				incIP(5);
			}
		}

		/*���������� � ���������� �����������
			����
			 �����           +2
			 �����           +3
			 ��������        +4
			 ���             +5
			 �����           +6
		*/
		else if (command == 31) {
			unsigned short param;
			if (IP = 255) {
				IP = 0;
				param = (DNA[IP] % 16);
			}
			else
				param = (DNA[++IP] % 16); //��������� ��������� �� �������� ���� � ��������� ������� �� ������� �� 16

			unsigned int x = getX(param); //��������� ���������� ������
			unsigned int y = getY(param); //
			size_t wc = world[x][y]; //���������� ������

			if (wc == empty) { //���� �����
				incIP(2);
			}
			else if (wc == wall) { //���� �����
				incIP(3);
			}
			else if (bots[wc].condition <= organic_sink) { //���� ��������
				incIP(4);
			}
			else if (isRelative(bots[wc])) { //���� �����
				incIP(6);
			}
			else { //���� ���
				incIP(5);
			}
		}

		/*������� � ������������� �����������
			����
			 �����           +2
			 �����           +3
			 ��������        +4
			 �������         +5
		*/
		else if (command == 32 || command == 42) { //����� ��������� ���� ������� ���������
			unsigned short param;
			if (IP = 255) {
				IP = 0;
				param = (DNA[IP] % 16);
			}
			else
				param = (DNA[++IP] % 16); //��������� ��������� �� �������� ���� � ��������� ������� �� ������� �� 16

			unsigned int x = getX(param + direct); //��������� ���������� ������
			unsigned int y = getY(param + direct); //


			if (world[x][y] == wall) //���� �����
				incIP(2);
			else if (world[x][y] == empty)  //���� �����
				incIP(3);
			else if (bots[world[x][y]].condition <= organic_sink) //���� ��������
				incIP(4);
			else { //������� ���� ������� - ������� ����� ���
				int hlt0 = energy;                   // ��������� ���������� ������� � ���������
				int hlt1 = bots[world[x][y]].energy; // � ���� � ��� ������
				int min0 = minrNum;
				int min1 = bots[world[x][y]].minrNum;

				if (hlt0 > hlt1) //E��� � ���� ������ �������, ��� � ������
				{
					int hlt = (hlt0 - hlt1) / 2; //T� ������������ ������� �������
					energy -= hlt;
					bots[world[x][y]].energy += hlt;
				}
				if (min0 > min1) //���� � ���� ������ ���������, ��� � ������
				{
					int min = (min0 - min1) / 2; //�� ������������ �� �������
					minrNum -= min;
					bots[world[x][y]].minrNum += min;
				}
				incIP(5);
			}
		}

		/*������� � ���������� �����������
			����
			 �����           +2
			 �����           +3
			 ��������        +4
			 �������         +5
		*/
		else if (command == 33 || command == 43) { //����� ��������� ���� ������� ���������
			unsigned short param;
			if (IP = 255) {
				IP = 0;
				param = (DNA[IP] % 16);
			}
			else
				param = (DNA[++IP] % 16); //��������� ��������� �� �������� ���� � ��������� ������� �� ������� �� 16

			unsigned int x = getX(param); //��������� ���������� ������
			unsigned int y = getY(param); //


			if (world[x][y] == wall) //���� �����
				incIP(2);
			else if (world[x][y] == empty)  //���� �����
				incIP(3);
			else if (bots[world[x][y]].condition <= organic_sink) //���� ��������
				incIP(4);
			else { //������� ���� ������� - ������� ����� ���
				int hlt0 = energy;                   // ��������� ���������� ������� � ���������
				int hlt1 = bots[world[x][y]].energy; // � ���� � ��� ������
				int min0 = minrNum;
				int min1 = bots[world[x][y]].minrNum;

				if (hlt0 > hlt1) //E��� � ���� ������ �������, ��� � ������
				{
					int hlt = (hlt0 - hlt1) / 2; //T� ������������ ������� �������
					energy -= hlt;
					bots[world[x][y]].energy += hlt;
				}
				if (min0 > min1) //���� � ���� ������ ���������, ��� � ������
				{
					int min = (min0 - min1) / 2; //�� ������������ �� �������
					minrNum -= min;
					bots[world[x][y]].minrNum += min;
				}
				incIP(5);
			}
		}

		/*������ � ������������� �����������
			����
			 �����           +2
			 �����           +3
			 ��������        +4
			 �������         +5
		*/
		else if (command == 34 || command == 44) {
			unsigned short param;
			if (IP = 255) {
				IP = 0;
				param = (DNA[IP] % 16);
			}
			else
				param = (DNA[++IP] % 16); //��������� ��������� �� �������� ���� � ��������� ������� �� ������� �� 16

			unsigned int x = getX(param + direct); //�������� ���������� ������
			unsigned int y = getY(param + direct); //

			if (world[x][y] == wall) //���� �����
				incIP(2);
			else if (world[x][y] == empty) //���� �����
				incIP(3);
			else if (bots[world[x][y]].condition <= organic_sink) //���� ��������
				incIP(4);
			else { //���� ���
				energy -= (energy / 4); //��� ����� �������� ����� �������
				bots[world[x][y]].energy += (energy / 4);

				if (minrNum >= 4) { //���� � ���� ��������� ������ 4
					minrNum -= (minrNum / 4); //��� ����� �������� ���� ���������
					bots[world[x][y]].minrNum += (minrNum / 4);
					if (bots[world[x][y]].minrNum > 999)
						bots[world[x][y]].minrNum = 999;
				}
			}
		}

		/*������ � ���������� �����������
			����
			 �����           +2
			 �����           +3
			 ��������        +4
			 �������         +5
		*/
		else if (command == 35 || command == 45) {
			unsigned short param;
			if (IP = 255) {
				IP = 0;
				param = (DNA[IP] % 16);
			}
			else
				param = (DNA[++IP] % 16); //��������� ��������� �� �������� ���� � ��������� ������� �� ������� �� 16

			unsigned int x = getX(param); //�������� ���������� ������
			unsigned int y = getY(param); //

			if (world[x][y] == wall) //���� �����
				incIP(2);
			else if (world[x][y] == empty) //���� �����
				incIP(3);
			else if (bots[world[x][y]].condition <= organic_sink) //���� ��������
				incIP(4);
			else { //���� ���
				energy -= (energy / 4); //��� ����� �������� ����� �������
				bots[world[x][y]].energy += (energy / 4);

				if (minrNum >= 4) { //���� � ���� ��������� ������ 4
					minrNum -= (minrNum / 4); //��� ����� �������� ���� ���������
					bots[world[x][y]].minrNum += (minrNum / 4);
					if (bots[world[x][y]].minrNum > 999)
						bots[world[x][y]].minrNum = 999;
				}
			}
		}

		//������������ �� �����������
		else if (command == 36) {
			srand();
			if (getRandomNumber(0, 1) == 0)
				direct = left;
			else
				direct = right;
		}

		//������������ �� ���������
		else if (command == 37) {
			srand();
			if (getRandomNumber(0, 1) == 0)
				direct = up;
			else
				direct = down;
		}

		/*��������� ������ ����
			����
			 ������          +2
			 �����/������    +2
		*/
		else if (command == 38) {
			unsigned short param;
			if (IP = 255) {
				IP = 0;
				param = std::lround(DNA[IP] * HEIGHT_COEF);
			}
			else
				param = std::lround(DNA[++IP] * HEIGHT_COEF); //��������� ��������� �� �������� ���� � �������� �� �����������

			if (coorY < param)
				incIP(2);
			else
				incIP(3);
		}

		/*��������� ������ �������
			����
			 ������          +2
			 �����/������    +2
		*/
		else if (command == 39) {
			unsigned short param;
			if (IP = 255) {
				IP = 0;
				param = std::lround(DNA[IP] * ENERGY_COEF);
			}
			else
				param = std::lround(DNA[++IP] * ENERGY_COEF); //��������� ��������� �� �������� ���� � �������� �� �����������

			if (energy < param)
				incIP(2);
			else
				incIP(3);
		}

		/*��������� ������ ���������
			����
			 ������          +2
			 �����/������    +2
		*/
		else if (command == 40) {
			unsigned short param;
			if (IP = 255) {
				IP = 0;

				param = std::lround(DNA[IP] * ENERGY_COEF);
			}
			else
				param = std::lround(DNA[++IP] * ENERGY_COEF); //��������� ��������� �� �������� ���� � �������� �� �����������

			if (minrNum < param)
				incIP(2);
			else
				incIP(3);
		}

		//�������� ���������� �������� �������
		else if (command == 41) {
			energy -= 150; //��� ����������� 150 ������ �������
			if (energy < 0) break;

			int a = -1; //���������� ���������� �����������
			for (unsigned short i = 0; i < 8; i++) {
				unsigned int x = getX(i);
				unsigned int y = getY(i);
				if (world[x][y] == empty) {
					a = i;
					break;
				}
			}
			if (a == -1) { //���� ��� ���������� �����
				energy = 0; //��� ��������
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

		/*�������� �� �����������
		  ����
			��� ����� ������ +1
			����             +2
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

		/*�������� �� ������ �������
		  ����
			������������     +1
			�� ������������  +2
		*/
		else if (command == 47) {
			double t;
			if (minrNum < 100)
				t = 0;
			else if (minrNum < 400)
				t = 1;
			else
				t = 2;

			int hlt = std::lround(season - ((coorY + 1) % 6) + t); //������� ���������� �������
			if (hlt >= 3)
				incIP(1);
			else
				incIP(2);
		}

		/*�������� �� ������ ���������
		  ����
			������������     +1
			�� ������������  +2
		*/
		else if (command == 48) {
			if (coorY > MAX_Y / 2)
				incIP(1);
			else
				incIP(2);
		}

		/*�������� �� ����������������
		  ����
			���������������  +1
			� ���� �������   +2
			������ �������   +3
		*/
		else if (command == 49) {
			if (chainNext == -1 && chainPrev == -1)
				incIP(1);
			else if (chainNext == -1 || chainPrev == -1)
				incIP(2);
			else
				incIP(3);
		}

		//�������������� ��������� � �������
		else if (command == 50) {
			if (minrNum > 100) {// ������������ ���������� ���������, ������� ����� ������������� � ������� = 100
				minrNum -= 100; // 1 ������� = 4 �������
				energy += 400;  //
				goBlue(100);    // ��� �� ����� ������
			}
			else {  // ���� ��������� ������ 100, �� ��� �������� ��������� � �������
				energy += minrNum * 4;
				goBlue(minrNum);
				minrNum = 0;
			}
			break;
		}

		//�������� ���������������
		else if (command == 51) {
			energy -= 150; //��� ����������� 150 ������ �������
			if (energy < 0) break;

			int a = -1; //���������� ���������� �����������
			for (unsigned short i = 0; i < 8; i++) {
				unsigned int x = getX(i);
				unsigned int y = getY(i);
				if (world[x][y] == empty) {
					a = i;
					break;
				}
			}
			if (a == -1) { //���� ��� ���������� �����
				energy = 0; //��� ��������
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

		else
			incIP(command);
	}
	else
		condition = alive;

	//
	//���������� ����
	//

		//���� ��� � �������
		if (chainNext != -1 && chainPrev != -1) {
			//����� ��������
			unsigned int min = (minrNum + bots[chainNext].minrNum + bots[chainPrev].minrNum);
			minrNum = (min / 3) + (min % 3);
			bots[chainNext].minrNum = min;
			bots[chainPrev].minrNum = min;

			//����� �������
			//
			//��������, �������� �� ��������� � ���������� ���� � ������� ��������
			//���� ��� �� �������� ��������, �� ������������ ������� �������
			//������� ��� � ���, ��� � ������� ����� � ������� ������ ���� ������ �������
			//����� ��� ������� ����� ����� � �������� �������
			if ((bots[chainNext].chainNext != -1 || bots[chainNext].chainPrev != -1) && (bots[chainPrev].chainNext != -1 || bots[chainPrev].chainPrev != -1)) {
				unsigned int hlt = (energy + bots[chainNext].energy + bots[chainPrev].energy);
				energy = (min / 3) + (min % 3);
				bots[chainNext].energy = hlt;
				bots[chainPrev].energy = hlt;
			}
		}
		//���� ��� ����� ����������� � �������
		else if (chainPrev != -1) { 
			//���� ���������� �� �������� ������� � �������
			if (bots[chainPrev].chainNext != -1 && bots[chainPrev].chainPrev != -1) {
				//�� ������������ ������� � ������ �������� ����
				unsigned int hlt = energy + bots[chainPrev].energy;
				bots[chainPrev].energy = (hlt / 4) + (hlt % 4);
				energy = hlt / 4 * 3;
			}
		}
		//���� ��� ����� ����������� � �������
		else if (chainNext != -1) { 
			//���� ���������� �� �������� ������� � �������
			if (bots[chainNext].chainNext != -1 && bots[chainNext].chainPrev != -1) {
				//�� ������������ ������� � ������ �������� ����
				unsigned int hlt = energy + bots[chainNext].energy;
				bots[chainNext].energy = (hlt / 4) + (hlt % 4);
				energy = hlt / 4 * 3;
			}
		}

		//���� ������� ������ 999, �� ������ ������ ����
		if (energy > 999) {
			int a = -1; //���������� ���������� �����������
			for (unsigned short i = 0; i < 8; i++) {
				unsigned int x = getX(i);
				unsigned int y = getY(i);
				if (world[x][y] == empty) {
					a = i;
					break;
				}
			}
			if (a == -1) //���� ��� ���������� �����
				energy = 0; //��� ��������
			else {
				if (b != nullptr)
					delete b;
				if (chainNext == -1 && chainPrev == -1)
					b = new bot(getX(a), getY(a), this, n + 1, FREE);
				else
					b = new bot(getX(a), getY(a), this, n + 1, CHAIN);
			}
		}
		//���� ������� ����� ������ 1
		else if (energy < 1) {
			condition = organic_sink; //�������� ��� ��������
			if (chainPrev != -1)      //���� ������� �� �������������� ������� - �������
				bots[chainPrev].chainNext = -1;
			if (chainNext != -1)
				bots[chainNext].chainPrev = -1;
		}

		//���� ��� ������, ��� 48, �� �� �������� ����������� ��������
		if (coorY > 48)
			minrNum++;
		else if (coorY > 66)
			minrNum += 2;
		else if (coorY > 88)
			minrNum += 3;

		//�������� �������
		if (minrNum > 999)
			minrNum = 999;
}

bot::~bot() {
	world[coorX][coorY] = empty; //�������� ���� � �����
	auto a = bots.begin() + n;   //�� �������
	bots.erase(a);               //
	for (size_t i = n; i < bots.size(); i++) //���������� ����������
		bots[i].n = i;

	if (chainPrev > 0)           //���� �� �������������� ������� - ���� �������
		bots[chainPrev].chainNext = -1;
	if (chainNext > 0)
		bots[chainNext].chainPrev = -1;
}