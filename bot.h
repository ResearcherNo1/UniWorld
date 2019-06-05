//���� ���������� �����
//
#include "constants.h"
#include <ctime>
#include <vector>
#include <algorithm> 

#ifndef BOT_H
#define BOT_H

class bot {
public:
	         int    energy; //���-�� �������
	unsigned short  DNA[DNA_SIZE]; //�����
	unsigned int    IP; //��������� ������� ����������

	unsigned short  stack[SUB_SIZE]; //����
	unsigned short  heapPtr;  //��������� �������� �����

	unsigned short  registers[SUB_SIZE]; //��������

	unsigned int    coorX;     //���������� X
	unsigned int    coorY;     //���������� Y
	         int    minrNum;   //���-�� ����������� ���������
	         cond   condition; //���������

	         short  red;       //       ��������
	         short  green;     //���-�� �������
	         short  blue;      //       ������
	         drct   direct;    //������� �����������
				size_t chainPrev; //C����� �� ����������� ���� � �������
				size_t chainNext; //C����� �� ����������  ���� � �������
	         size_t n;         //�������� � ������� bots 

	bot(unsigned int X, unsigned int Y, bot* parent = nullptr, size_t N = 0, bool free = true);

	void incIP(unsigned int num = 1); //������� ��e������� ��������� ������� �������
	unsigned short getParam(); //������� ��������� ���������

	unsigned int getX(unsigned short n); //��������� �-���������� �� �����������
	unsigned int getY(unsigned short n); //��������� Y-���������� �� �����������
	bool isRelative(bot _bot); //����������, ����������� �� _bot

	void goRed  (short n);
	void goGreen(short n);
	void goBlue (short n);
	 
	void step(); //������� ������� �����������������
	~bot();
};

#endif //BOT_H