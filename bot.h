//���� ���������� �����
//
#include "constants.h"
#include <ctime>
#include <vector>
#include <string>

#ifndef BOT_H
#define BOT_H

class bot {
public:
	         int    energy;    //���-�� �������
				int    minrNum;   //���-�� ����������� ���������
				short  decompose; //������� ����������
				cond   condition; //���������

				short  DNA[DNA_SIZE];   //�����
	unsigned int    IP;              //��������� ������� ����������

	         short  stack[SUB_SIZE]; //����
	unsigned short  heapPtr;         //��������� �������� �����

	         short  registers[SUB_SIZE]; //��������

	unsigned int    coorX;     //���������� X
	unsigned int    coorY;     //���������� Y
				drct   direct;    //������� �����������

	 	 long long   chainPrev; //C����� �� ����������� ���� � �������
		 long long   chainNext; //C����� �� ����������  ���� � �������
				size_t n;         //�������� � ������� bots 

	         short  red;       //       ��������
	         short  green;     //���-�� �������
	         short  blue;      //       ������

				int    born = 0;      //����������� ����� ������� ��� �������� �������

	bot(unsigned int X, unsigned int Y, bot* parent = nullptr, size_t N = 0, bool free = true);
	void death(); //��������� ������

	void incIP(unsigned int num = 1); //������� ��e������� ��������� ������� �������
	unsigned short getParam();        //������� ��������� ���������

	unsigned int getX(unsigned short n); //��������� �-���������� �� �����������
	unsigned int getY(unsigned short n); //��������� Y-���������� �� �����������
	bool isRelative(bot _bot);           //����������, ����������� �� _bot

	void goRed  (short n);
	void goGreen(short n);
	void goBlue (short n);
	void print(std::string a);

	void step(); //������� ������� ����e������������
};

void radiation();

#endif //BOT_H