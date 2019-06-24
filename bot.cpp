//���� �������� ���������� �����
//
#include "bot.h"

extern long long world[WORLD_WIDTH][((unsigned long long)WORLD_HEIGHT + 2)];
extern std::pmr::vector<bot> bots;
extern int season;
bot* b;

void signal_handler(int signal) {
	if (signal == SIGABRT)
		std::cerr << "SIGABRT captured\n";
}

bot::bot(const unsigned int X, const unsigned int Y, bot* parent, size_t N, const bool free) {
#pragma warning(push)
#pragma warning(disable : 4996)
	srand();
#pragma warning(pop)

	auto handler = std::signal(SIGABRT, signal_handler);
	if (handler == SIG_ERR)
		std::cerr << "Signal setup failed\n";

	//���� ��� ���������
	if (parent == nullptr) {
		for (size_t i = 0; i < DNA_SIZE; i++) //��������� ��� �������� ����������
			DNA[i] = 25;
		energy = 900;
		minrNum = 0;
		chainNext = chainPrev = -1;
		red = green = blue = 170;
	}
	else {
		//��������� ��������� �������
		size_t mutPtr = getRandomNumber(0, 255);
		//�������� ����� ��������
		for (size_t i = 0; i < DNA_SIZE; i++)
			DNA[i] = parent->DNA[i];
		//��������
		for (short i = 0; i < MUT_COEF; i++) {
			DNA[mutPtr] = getRandomNumber(0, 255);
			mutPtr++;
			if (mutPtr > DNA_MAX_INDEX)
				mutPtr -= DNA_SIZE;
		}

		//�������� �������� ������� � ��������
		energy = (parent->energy) / 2;
		minrNum = (parent->minrNum) / 2;
		parent->energy /= 2;
		parent->minrNum /= 2;

		red = parent->red;
		green = parent->green;
		blue = parent->blue;

		if (free == false) {
			if (parent->chainNext <= -1) { //E��� � ����-������ ������ �� ���������� ���� � �������������� ������� �����
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
		} 
		else {
			chainPrev = -1;
			chainNext = -1;
		}
	}

	//��������� ����� �����������
	for (size_t i = 0; i < DNA_SIZE; i++)
		born += DNA[i];
	born /= 32;

	//�������� ��������� ���������� � ����e ������
	IP = 0;
	//������� ���� � ��������
	for (size_t i = 0; i < SUB_SIZE; i++) {
		stack[i] = -1;
		registers[i] = 0;
	}
	heapPtr = 0;

	assert(coorX >= 0 && coorX < WORLD_WIDTH);
	assert(coorY > 0 && coorY <= WORLD_HEIGHT + 1);
	coorX = X; coorY = Y;

	condition = alive;
	direct = down;
	decompose = 0;


	auto a = bots.begin(); //�������� ��� �������

	//���� ��� ��������� - ��������� � ����� �������
	if (parent == nullptr) {
		bots.push_back(*this);
		world[X][Y] = N;
	}
	else {
		//���� N ������ ������� ������� - ��������� �� ����� ��������
		if (N >= bots.size()) {
			n = parent->n;
			a = bots.begin() + n;
			bots.insert(a, *this);
		}
		//� ������ ������ - ���������� ��������������� N
		else {
			a = bots.begin() + N;
			n = N;
			bots.insert(a, *this);
		}

		world[X][Y] = n;

		size_t i = n;
		if (i == 0)
			i = 0;
		else
			i--;
		//���������� ����������
		for (i; i < bots.size(); i++) { 
			bots[i].n = i;
			int x = bots[i].coorX;
			int y = bots[i].coorY;
			if (y == 0)
				y = 1;
			else if (y == WORLD_HEIGHT + 2)
				y = 131;

				assert(x >= 0 && x < WORLD_WIDTH);
				assert(y > 0 && y <= WORLD_HEIGHT + 1);
				world[bots[i].coorX][bots[i].coorY] = i;
		}
	}
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
	long x = coorX;
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
	int y = coorY;
	if (n > 7)
		n -= 8;

	if (n < 3)
		y--;
	else if (n == 4 || n == 5 || n == 6)
		y++;

	if (y > MAX_Y)
		y = MAX_Y;
	else if (y < 0)
		y = 0;
	return y;
}

bool bot::isRelative(bot _bot) {
	if (_bot.condition < alive) //���� _bot - ��������
		return false;

	size_t dif = 0; //C������ ������������ � ������
	for (size_t i = 0; i < DNA_SIZE; i++) //��������� ������� ������
		if (DNA[i] != _bot.DNA[i]) {
			dif++;
			if (dif > MUT_COEF) return false; //���� ������������ > ����������� ���-�� �������, �� ���� �� ������������
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
	auto handler = std::signal(SIGABRT, signal_handler);
	if (handler == SIG_ERR)
		std::cerr << "Signal setup failed\n";

	//���� ��� - ��������, �������
	if (condition == organic_hold) {
		decompose++;
		if (decompose >= DECOMPOSE_TIME)
			death();
		return;
	}
	if (condition == organic_sink) { //������� ��������
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

	//���� ��� �� ��������� � ��������� �����
	if (!(condition == input))
		//��������� �� ����� 25 ������ �� ���
		for (unsigned short cyc = 0; cyc < 25; cyc++) {
		unsigned short command = DNA[IP]; //������� �������

		//������������� ����� �����������
		if (command == 23) {
			long param = getParam() % 8;

			param += direct; //���������� ����� ���������� � �������� ����������� ����
			if (param > 7) //��������� ������ ���� � �������� 0 ... 7
				param -= 8;
			direct = static_cast<drct>(param);
			incIP(2);
		}

		//���������� ����� �����������
		else if (command == 24) {
			long param = getParam() % 8;

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

			int hlt = std::lround(((season * 10) - coorY) / (3 - t)); //������� ���������� �������
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
			if (chainPrev > -1 || chainNext > -1)
				//������ �� ��������� - ������ ����� ������ �������������
				incIP(1);
			else {
				unsigned short param = getParam() % 8;

				unsigned int x = getX(param + direct); //�������� ���������� ������
				unsigned int y = getY(param + direct); //

				long long size = static_cast<long long>(bots.size());
				assert(world[x][y] < size);

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
		}

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
			if (chainPrev > -1 || chainNext > -1)
				//������ �� ��������� - ������ ����� ������ �������������
				incIP(1);
			else {
				unsigned short param = getParam() % 8;

				unsigned int x = getX(param); //�������� ���������� ������
				unsigned int y = getY(param); //

				long long size = static_cast<long long>(bots.size());
				assert(world[x][y] < size);

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
			unsigned short param = getParam() % 8; 

			energy -= 4; //��� �� ���� �����, ���������� �� ����������, ������ 4 �������
			unsigned int x = getX(param + direct); //�������� ���������� ������
			unsigned int y = getY(param + direct); //

			long long size = static_cast<long long>(bots.size());
			assert(world[x][y] < size);

			if (world[x][y] == empty) { //���� �����
				incIP(2);
			}
			else if (world[x][y] == wall) { //���� �����
				incIP(3);
			}
			else if (bots[world[x][y]].condition <= organic_sink) { //���� ��� ��������� ��������
				bots[world[x][y]].death(); //������� ������
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
					bots[world[x][y]].death(); //������� ������
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
						bots[world[x][y]].death(); //������� ������
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
			unsigned short param = getParam() % 8;

			energy -= 4; //��� �� ���� �����, ���������� �� ����������, ������ 4 �������
			unsigned int x = getX(param); //�������� ���������� ������
			unsigned int y = getY(param); //

			long long size = static_cast<long long>(bots.size());
			assert(world[x][y] < size);

			if (world[x][y] == empty) { //���� �����
				incIP(2);
			}
			else if (world[x][y] == wall) { //���� �����
				incIP(3);
			}
			else if (bots[world[x][y]].condition <= organic_sink) { //���� ��� ��������� ��������
				bots[world[x][y]].death(); //������� ������
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
					bots[world[x][y]].death(); //������� ������
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
						bots[world[x][y]].death(); //������� ������
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
			unsigned short param = getParam();

			unsigned int x = getX(param + direct); //��������� ���������� ������
			unsigned int y = getY(param + direct); //
			long long wc = world[x][y]; //���������� ������

			long long size = static_cast<long long>(bots.size());
			assert(wc < size);

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
			unsigned short param = getParam() % 8;

			unsigned int x = getX(param); //��������� ���������� ������
			unsigned int y = getY(param); //
			long long wc = world[x][y]; //���������� ������

			long long size = static_cast<long long>(bots.size());
			assert(wc < size);

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
			unsigned short param = getParam() % 8;

			unsigned int x = getX(param + direct); //��������� ���������� ������
			unsigned int y = getY(param + direct); //

			long long size = static_cast<long long>(bots.size());
			assert(world[x][y] < size);

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
			unsigned short param = getParam() % 8;

			unsigned int x = getX(param); //��������� ���������� ������
			unsigned int y = getY(param); //

			long long size = static_cast<long long>(bots.size());
			assert(world[x][y] < size);

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
			if (IP == 255) {
				IP = 0;
				param = (DNA[IP] % 16);
			}
			else
				param = (DNA[IP + 1] % 16); //��������� ��������� �� �������� ���� � ��������� ������� �� ������� �� 16

			unsigned int x = getX(param + direct); //�������� ���������� ������
			unsigned int y = getY(param + direct); //

			long long size = static_cast<long long>(bots.size());
			assert(world[x][y] < size);

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
			unsigned short param = getParam() % 8;

			unsigned int x = getX(param); //�������� ���������� ������
			unsigned int y = getY(param); //

			long long size = static_cast<long long>(bots.size());
			assert(world[x][y] < size);

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
#pragma warning(push)
#pragma warning(disable : 4996)
			srand();
#pragma warning(pop)
			if (getRandomNumber(0, 1) == 0)
				direct = left;
			else
				direct = right;
		}

		//������������ �� ���������
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

		/*��������� ������ ����
			����
			 ������          +2
			 �����/������    +2
		*/
		else if (command == 38) {
			long param = std::lround(getParam() * HEIGHT_COEF); //��������� ��������� �� �������� ���� � �������� �� �����������

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
			long param = std::lround(getParam() * ENERGY_COEF); //��������� ��������� �� �������� ���� � �������� �� �����������

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
			long param = std::lround(getParam() * ENERGY_COEF); //��������� ��������� �� �������� ���� � �������� �� �����������

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
				if (world[x][y] == empty && y > 0 && y < WORLD_HEIGHT - 1) {
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

			incIP(1);
			if (chainNext > -1 && chainPrev > -1)
				b = new bot(getX(a), getY(a), this, n + 1, CHAIN);
			else
				b = new bot(getX(a), getY(a), this, n + 1, FREE);

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
			if (chainNext <= -1 && chainPrev <= -1)
				incIP(1);
			else if (chainNext <= -1 || chainPrev <= -1)
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
				if (world[x][y] == empty && y > 0 && y < WORLD_HEIGHT - 1) {
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

			incIP(1);
			if (chainNext > -1 && chainPrev > -1)
				b = new bot(getX(a), getY(a), this, n + 1, FREE);
			else
				b = new bot(getX(a), getY(a), this, n + 1, CHAIN);

			break;
		}

		////////////////////////////////////
		//������ ����� "������������" ������
		////////////////////////////////////

		//��������� ����� � ������� A
		else if (command == 52) {
			unsigned short param = getParam();

			registers[0] = param;
			incIP(1);
		}

		//��������� ����� � ������� B
		else if (command == 53) {
			unsigned short param = getParam();

			registers[1] = param;
			incIP(1);
		}

		//��������� ����� � ������� C
		else if (command == 54) {
			unsigned short param = getParam();

			registers[2] = param;
			incIP(1);
		}

		//��������� ����� � ������� D
		else if (command == 55) {
			unsigned short param = getParam();

			registers[3] = param;
			incIP(1);
		}

		//��������� ����� � ������� E
		else if (command == 56) {
			unsigned short param = getParam();

			registers[4] = param;
			incIP(1);
		}

		//��������� ����� � ������� F
		else if (command == 57) {
			unsigned short param = getParam();

			registers[5] = param;
			incIP(1);
		}

		//��������� ����� � ������� G
		else if (command == 58) {
			unsigned short param = getParam();

			registers[6] = param;
			incIP(1);
		}

		//��������� ����� � ������� H
		else if (command == 59) {
			unsigned short param = getParam();

			registers[7] = param;
			incIP(1);
		}

		////////////////////////////////////

		//��������� ����� �� �������� A
		else if (command == 60) {
			incIP(1);
			DNA[IP] = registers[0];
		}

		//��������� ����� �� �������� B
		else if (command == 61) {
			incIP(1);
			DNA[IP] = registers[1];
		}

		//��������� ����� �� �������� C
		else if (command == 62) {
			incIP(1);
			DNA[IP] = registers[2];
		}

		//��������� ����� �� �������� D
		else if (command == 63) {
			incIP(1);
			DNA[IP] = registers[3];
		}

		//��������� ����� �� �������� E
		else if (command == 64) {
			incIP(1);
			DNA[IP] = registers[4];
		}

		//��������� ����� �� �������� F
		else if (command == 65) {
			incIP(1);
			DNA[IP] = registers[5];
		}

		//��������� ����� �� �������� G
		else if (command == 66) {
			incIP(1);
			DNA[IP] = registers[6];
		}

		//��������� ����� �� �������� H
		else if (command == 67) {
			incIP(1);
			DNA[IP] = registers[7];
		}

		////////////////////////////////////

		//����� �������� ��������� � �������� A
		else if (command == 68) {
			incIP(1);
			std::swap(DNA[IP], registers[0]);
		}

		//����� �������� ��������� � �������� B
		else if (command == 69) {
			incIP(1);
			std::swap(DNA[IP], registers[1]);
		}

		//����� �������� ��������� � �������� C
		else if (command == 70) {
			incIP(1);
			std::swap(DNA[IP], registers[2]);
		}

		//����� �������� ��������� � �������� D
		else if (command == 71) {
			incIP(1);
			std::swap(DNA[IP], registers[3]);
		}

		//����� �������� ��������� � �������� E
		else if (command == 72) {
			incIP(1);
			std::swap(DNA[IP], registers[4]);
		}

		//����� �������� ��������� � �������� F
		else if (command == 73) {
			incIP(1);
			std::swap(DNA[IP], registers[5]);
		}

		//����� �������� ��������� � �������� G
		else if (command == 74) {
			incIP(1);
			std::swap(DNA[IP], registers[6]);
		}

		//����� �������� ��������� � �������� H
		else if (command == 75) {
			incIP(1);
			std::swap(DNA[IP], registers[7]);
		}

		////////////////////////////////////

		//����������� �� �������� A � ������������
		else if (command == 76) {
			unsigned short param = getParam() % 8;

			registers[param] = registers[0];
			incIP(1);
		}

		//����������� �� �������� B � ������������
		else if (command == 77) {
			unsigned short param = getParam() % 8;

			registers[param] = registers[1];
			incIP(1);
		}

		//����������� �� �������� C � ������������
		else if (command == 78) {
			unsigned short param = getParam() % 8;

			registers[param] = registers[2];
			incIP(1);
		}

		//����������� �� �������� D � ������������
		else if (command == 79) {
			unsigned short param = getParam() % 8;

			registers[param] = registers[3];
			incIP(1);
		}

		//����������� �� �������� E � ������������
		else if (command == 80) {
			unsigned short param = getParam() % 8;

			registers[param] = registers[4];
			incIP(1);
		}

		//����������� �� �������� F � ������������
		else if (command == 81) {
			unsigned short param = getParam() % 8;

			registers[param] = registers[5];
			incIP(1);
		}

		//����������� �� �������� G � ������������
		else if (command == 82) {
			unsigned short param = getParam() % 8;

			registers[param] = registers[6];
			incIP(1);
		}

		//����������� �� �������� H � ������������
		else if (command == 83) {
			unsigned short param = getParam() % 8;

			registers[param] = registers[7];
			incIP(1);
		}

		////////////////////////////////////

		//����� �������� �������� � � �������������
		else if (command == 84) {
			unsigned short param = getParam() % 8;
		
			std::swap(registers[param], registers[0]);
			incIP(1);
		}
		
		//����� �������� �������� B � �������������
		else if (command == 85) {
			unsigned short param = getParam() % 8;

			std::swap(registers[param], registers[1]);
			incIP(1);
		}

		//����� �������� �������� C � �������������
		else if (command == 86) {
			unsigned short param = getParam() % 8;

			std::swap(registers[param], registers[2]);
			incIP(1);
		}

		//����� �������� �������� D � �������������
		else if (command == 87) {
			unsigned short param = getParam() % 8;

			std::swap(registers[param], registers[3]);
			incIP(1);
		}

		//����� �������� �������� E � �������������
		else if (command == 88) {
			unsigned short param = getParam() % 8;

			std::swap(registers[param], registers[4]);
			incIP(1);
		}

		//����� �������� �������� F � �������������
		else if (command == 89) {
			unsigned short param = getParam() % 8;

			std::swap(registers[param], registers[5]);
			incIP(1);
		}

		//����� �������� �������� G � �������������
		else if (command == 90) {
			unsigned short param = getParam() % 8;

			std::swap(registers[param], registers[6]);
			incIP(1);
		}

		//����� �������� �������� H � �������������
		else if (command == 91) {
			unsigned short param = getParam() % 8;

			std::swap(registers[param], registers[7]);
			incIP(1);
		}

		////////////////////////////////////

		//����������������� ������������� ��������
		else if (command == 92) {
			unsigned short param = getParam() % 8;

			registers[param]++;
			incIP(1);
		}

		//����������������� ������������� ��������
		else if (command == 93) {
			unsigned short param = getParam() % 8;

			registers[param]--;
			incIP(1);
		}

		//C���� ����� �����
		else if (command == 102) {
			unsigned short param = getParam() % 8;
#pragma warning(push)
#pragma warning(disable : 4244)
			unsigned char a = registers[param];
#pragma warning(pop)
			registers[param] = a << 1;
			incIP(1);
		}

		//C���� ����� ������
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
		
		/*��������� �������� � � �������������
		   ����
			 ���������       +2
			 ��������        +3
		*/
		else if (command == 94) {
			unsigned short param = getParam() % 8;
			
			if (registers[0] == registers[param])
				incIP(2);
			else
				incIP(3);
		}

		/*��������� �������� B � �������������
			����
			 ���������       +2
			 ��������        +3
		*/
		else if (command == 95) {
			unsigned short param = getParam() % 8;

			if (registers[1] == registers[param])
				incIP(2);
			else
				incIP(3);
		}

		/*��������� �������� C � �������������
			����
			 ���������       +2
			 ��������        +3
		*/
		else if (command == 96) {
			unsigned short param = getParam() % 8;

			if (registers[2] == registers[param])
				incIP(2);
			else
				incIP(3);
		}

		/*��������� �������� D � �������������
			����
			 ���������       +2
			 ��������        +3
		*/
		else if (command == 97) {
			unsigned short param = getParam() % 8;

			if (registers[3] == registers[param])
				incIP(2);
			else
				incIP(3);
		}

		/*��������� �������� E � �������������
			����
			 ���������       +2
			 ��������        +3
		*/
		else if (command == 98) {
			unsigned short param = getParam() % 8;

			if (registers[4] == registers[param])
				incIP(2);
			else
				incIP(3);
		}

		/*��������� �������� F � �������������
			����
			 ���������       +2
			 ��������        +3
		*/
		else if (command == 99) {
			unsigned short param = getParam() % 8;

			if (registers[5] == registers[param])
				incIP(2);
			else
				incIP(3);
		}

		/*��������� �������� G � �������������
			����
			 ���������       +2
			 ��������        +3
		*/
		else if (command == 100) {
			unsigned short param = getParam() % 8;

			if (registers[6] == registers[param])
				incIP(2);
			else
				incIP(3);
		}

		/*��������� �������� H � �������������
			����
			 ���������       +2
			 ��������        +3
		*/
		else if (command == 101) {
			unsigned short param = getParam() % 8;

			if (registers[7] == registers[param])
				incIP(2);
			else
				incIP(3);
		}

		////////////////////////////////////

		/*����������� �������, ���� ������� � ����� 0
		   ���� �� �����     +2
		*/
		else if (command == 104) {
			if (registers[0] == 0)
				IP = getParam();
			else
				incIP(2);
		}

		/*����������� �������, ���� ������� B ����� 0
		   ���� �� �����     +2
		*/
		else if (command == 105) {
			if (registers[1] == 0)
				IP = getParam();
			else
				incIP(2);
		}

		/*����������� �������, ���� ������� C ����� 0
			���� �� �����     +2
		*/
		else if (command == 106) {
			if (registers[2] == 0)
				IP = getParam();
			else
				incIP(2);
		}

		/*����������� �������, ���� ������� D ����� 0
			���� �� �����     +2
		*/
		else if (command == 107) {
			if (registers[3] == 0)
				IP = getParam();
			else
				incIP(2);
		}

		/*����������� �������, ���� ������� E ����� 0
			���� �� �����     +2
		*/
		else if (command == 108) {
			if (registers[4] == 0)
				IP = getParam();
			else
				incIP(2);
		}

		/*����������� �������, ���� ������� F ����� 0
			���� �� �����     +2
		*/
		else if (command == 109) {
			if (registers[5] == 0)
				IP = getParam();
			else
				incIP(2);
		}

		/*����������� �������, ���� ������� G ����� 0
			���� �� �����     +2
		*/
		else if (command == 110) {
			if (registers[6] == 0)
				IP = getParam();
			else
				incIP(2);
		}

		/*����������� �������, ���� ������� H ����� 0
			���� �� �����     +2
		*/
		else if (command == 111) {
			if (registers[7] == 0)
				IP = getParam();
			else
				incIP(2);
		}
		
		////////////////////////////////////
		////////////////////////////////////

		/*������� ������� ��������� � ����
		   ����
			 ������           +2
			 ����������       +3
		*/
		else if (command == 112) {
			unsigned short param = getParam();
			if (heapPtr == 8)
				incIP(3);
				//energy -= 10; //��� ��������� ;)
			else {
				stack[heapPtr] = param;
				heapPtr++;
				incIP(2);
			}
		}

		/*�������� ������ �� ����� � ������������ �������
			����
			 ������          +2
			 ����            +3
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

		/*�������� �������� �����
		  ��������� �������� ����� � ��������� � ������� �
		   ����
			 ������          +2
			 ����            +3
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

		/*�������� �������� �����
		  ��������� �������� ����� � ��������� � ������� B
			����
			 ������          +2
			 ����            +3
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

		/*�������� �������� �����
		  ��������� �������� ����� � ��������� � ������� C
			����
			 ������          +2
			 ����            +3
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

		/*�������� �������� �����
		  ��������� �������� ����� � ��������� � ������� D
			����
			 ������          +2
			 ����            +3
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

		/*�������� �������� �����
	     ��������� �������� ����� � ��������� � ������� E
			����
			 ������          +2
			 ����            +3
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

		/*�������� �������� �����
		  ��������� �������� ����� � ��������� � ������� F
			����
			 ������          +2
			 ����            +3
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

		/*�������� �������� �����
		  ��������� �������� ����� � ��������� � ������� G
			����
			 ������          +2
			 ����            +3
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

		/*�������� �������� �����
		  ��������� �������� ����� � ��������� � ������� H
			����
			 ������          +2
			 ����            +3
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

		//����� �������� ����� � ������������� ��������
		else if (command == 122) {
			if (heapPtr > 0)
				std::swap(stack[heapPtr - 1], registers[(getParam() % 8)]);
		}

		////////////////////////////////////

		//������� �� ������ � �������� �����
		//(���� �� ����� -1, ������� �� ���������)
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

		//������ �������� ������ � ����	
		//��� �������       +2
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

		//���� �� �������� ��������, ��������� �������� �������
		else
			incIP(command);
	}
	else
		condition = alive;

	//
	//���������� ����
	//

	energy -= 4;

	
		//���� ��� � �������
		if (chainNext > -1 && chainPrev > -1) {
			long long size = static_cast<long long>(bots.size());
			assert(chainNext < size);
			assert(chainPrev < size);

			//����� ��������
			unsigned int min = minrNum + bots[chainNext].minrNum + bots[chainPrev].minrNum;
			minrNum = (min / 3) + (min % 3);
			bots[chainNext].minrNum = min;
			bots[chainPrev].minrNum = min;

			//����� �������
			//
			//��������, �������� �� ��������� � ���������� ���� � ������� ��������
			//���� ��� �� �������� ��������, �� ������������ ������� �������
			//������� ��� � ���, ��� � ������� ����� � ������� ������ ���� ������ �������
			//����� ��� ������� ����� ����� � �������� �������
			if ((bots[chainNext].chainNext > -1 || bots[chainNext].chainPrev > -1) && (bots[chainPrev].chainNext > -1 || bots[chainPrev].chainPrev > -1)) {
				unsigned int hlt = (energy + bots[chainNext].energy + bots[chainPrev].energy);
				energy = (min / 3) + (min % 3);
				bots[chainNext].energy = hlt;
				bots[chainPrev].energy = hlt;
			}
		}
		//���� ��� ����� ����������� � �������
		else if (chainPrev > -1) { 
			long long size = static_cast<long long>(bots.size());
			assert(chainPrev < size);

			//���� ���������� �� �������� ������� � �������
			if (bots[chainPrev].chainNext > -1 && bots[chainPrev].chainPrev > -1) {
				//�� ������������ ������� � ������ �������� ����
				unsigned int hlt = energy + bots[chainPrev].energy;
				bots[chainPrev].energy = (hlt / 4) + (hlt % 4);
				energy = hlt / 4 * 3;
			}
		}
		//���� ��� ����� ��������� � �������
		else if (chainNext > -1) { 
			long long size = static_cast<long long>(bots.size());
			assert(chainNext < size);

			//���� ���������� �� �������� ������� � �������
			if (bots[chainNext].chainNext > -1 && bots[chainNext].chainPrev > -1) {
				//�� ������������ ������� � ������ �������� ����
				unsigned int hlt = energy + bots[chainNext].energy;
				bots[chainNext].energy = (hlt / 4) + (hlt % 4);
				energy = hlt / 4 * 3;
			}
		}

		//���� ������� ������ ������������ ������, �� ������ ������ ����
		if (energy > born) {
			int a = -1; //���������� ���������� �����������
			for (unsigned short i = 0; i < 8; i++) {
				unsigned int x = getX(i);
				unsigned int y = getY(i);
				if (world[x][y] == empty && y > 0 && y < WORLD_HEIGHT - 1) {
					a = i;
					break;
				}
			}
			if (a == -1) //���� ��� ���������� �����
				energy = 0; //��� ��������
			else {
				if (b != nullptr)
					delete b;
				if (chainNext < -1 && chainPrev < -1)
					b = new bot(getX(a), getY(a), this, n + 1, FREE);
				else
					b = new bot(getX(a), getY(a), this, n + 1, CHAIN);
			}
		}
		try {	
			//���� ������� ����� ������ 1
			if (energy < 1) {
				condition = organic_sink; //�������� ��� ��������

				long long size = static_cast<long long>(bots.size());
				//���� ������� �� �������������� ������� - �������
				if (chainPrev > -1) {
					assert(chainPrev < size);
					bots[chainPrev].chainNext = -1;
				}
				if (chainNext > -1) {
					assert(chainNext < size);
					bots[chainNext].chainPrev = -1;
				}
			}

			assert(coorY > 0 && coorY <= WORLD_HEIGHT + 1);
			//���� ��� ������, ��� MAX_Y / 2, �� �� �������� ����������� ��������
			if (coorY > (MAX_Y / 2))
				minrNum++;
			else if (coorY > (MAX_Y / 2 + MAX_Y / 4))
				minrNum += 2;
			else if (coorY > (MAX_Y - MAX_Y / 10))
				minrNum += 3;

			//�������� �������
			if (minrNum > 999)
				minrNum = 999;
		}
		catch (...) {}

}

void bot::death() {
	//��������� �������� ����� �����
	//�++03 ����������� ����������� �������� ��������� � ������ �������
	//� ������������ ����������� �������� �� �����, �� ��� ������� �������� �����
	n = this - &bots[0];

	auto handler = std::signal(SIGABRT, signal_handler);
	if (handler == SIG_ERR)
		std::cerr << "Signal setup failed\n";
	else {
		assert(coorX >= 0 && coorX < WORLD_WIDTH);
		assert(coorY > 0 && coorY <= WORLD_HEIGHT + 1);
		world[coorX][coorY] = empty; //�������� ���� � �����
	}

	assert(n < bots.size());
	auto a = bots.begin() + n; //�� �������
	bots.erase(a);             //

	size_t i = n;
	if (i == 0)
		i = 0;
	else
		i--;
	//���������� ����������
	for (i; i < bots.size(); i++) {
		bots[i].n = i;
		int x = bots[i].coorX;
		int y = bots[i].coorY;
		assert(x >= 0 && x < WORLD_WIDTH);
		assert(y > 0 && y <= WORLD_HEIGHT + 1);
		world[bots[i].coorX][bots[i].coorY] = i;
	}

	if (chainPrev > -1) //���� �� �������������� ������� - ���� �������
		bots[chainPrev].chainNext = -1;
	if (chainNext > -1)
		bots[chainNext].chainPrev = -1;
}

//�������������� �������
//�������� ���� ��������� ��� � �����,
//����������� �� ����� ������� ������
void radiation() {
	for (size_t i = 0; i < WORLD_WIDTH; i++)
		if (world[i][1] != empty)
			bots[world[i][1]].DNA[getRandomNumber(0, 255)] = getRandomNumber(0, 255);
}