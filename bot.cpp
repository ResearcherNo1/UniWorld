//���� �������� ���������� �����
//
#include "bot.h"

extern int world[WORLD_WIDTH][WORLD_HEIGHT];
extern std::vector<bot> bots;
extern int season;

bot::bot(unsigned int X, unsigned int Y, bot* parent, size_t n) {
	srand();
	//���� ��� ���������
	if (parent == nullptr) {
		for (size_t i = 0; i < DNA_SIZE; i++) //��������� ��� �������� ����������
			DNA[i] = 25;
		energy = 990;
		minrNum = 0;
		chainNext = chainPrev = 0;
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
			//��������
			for (mut_ptr; mut_ptr < mut_ptr + 8; mut_ptr++)
				DNA[mut_ptr] = getRandomNumber(0, 255);
		}
		energy = (parent->energy) / 2;
		minrNum = (parent->minrNum) / 2;
		red = parent->red;
		green = parent->green;
		blue = parent->blue;
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

	for (unsigned short cyc = 0; cyc < 15; cyc++) { //��������� �� ����� 15 ������ �� ���
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
				param - 8;
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
			else {
				if (minrNum < 400)
					t = 1;
				else
					t = 2;
			}
			double hlt = season - ((coorY + 1) % 6) + t; //������� ���������� �������
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
			if (chainPrev > 0 || chainNext > 0)
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
			if (chainPrev > 0 || chainNext > 0)
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
				unsigned int min0 = minrNum; //���-�� ��������� � ����
				unsigned int min1 = bots[world[x][y]].minrNum; //���-�� ��������� � �������������� �����
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
				unsigned int min0 = minrNum; //���-�� ��������� � ����
				unsigned int min1 = bots[world[x][y]].minrNum; //���-�� ��������� � �������������� �����
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
			int wc = world[x][y]; //���������� ������

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
			int wc = world[x][y]; //���������� ������

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
			

			if (world[x][y] == empty) //���� �����
				incIP(2);
			else if (world[x][y] == wall)  //���� �����
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


			if (world[x][y] == empty) //���� �����
				incIP(2);
			else if (world[x][y] == wall)  //���� �����
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

		}

		/*������ � ���������� �����������
			����
			 �����           +2
			 �����           +3
			 ��������        +4
			 �������         +5
		*/
		else if (command == 35 || command == 45) {

		}

	}
}

bot::~bot() {
	world[coorX][coorY] = empty; //�������� ���� � �����
	auto a = bots.begin() + n;   //�� �������
	bots.erase(a);               //

	if (chainPrev > 0)           //���� �� �������������� ������� - ���� �������
		bots[chainPrev].chainNext = 0;
	if (chainNext > 0)
		bots[chainNext].chainPrev = 0;
}