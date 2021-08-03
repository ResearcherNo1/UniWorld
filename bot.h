//���� ���������� �����
//
#include "constants.h"
#include <ctime>
#include <vector>
#include <string>
#undef NDEBUG
#include <cassert>
#include <csignal>
#define NDEBUG

#ifndef BOT_H
#define BOT_H

class bot {
public:	
	         int    energy = 0;              //���-�� �������
			 int    minrNum = 0;             //���-�� ����������� ���������
			 short  decompose = 0;           //������� ����������
	         cond   condition = cond::alive; //���������
	         	    
	         short  DNA[DNA_SIZE];           //�����
	unsigned int    IP = 0;                  //��������� ������� ����������

	         short  stack[SUB_SIZE];         //����
	unsigned short  heapPtr = 0;             //��������� �������� �����

	         short  registers[SUB_SIZE];     //��������

	         int    coorX = 0;               //���������� X
	         int    coorY = 1;               //���������� Y
	         drct   direct = drct::up;       //������� �����������

	    long long   chainPrev = -1;          //C����� �� ����������� ���� � �������
	    long long   chainNext = -1;          //C����� �� ����������  ���� � �������
	         size_t n = 0;                   //�������� � ������� bots 

	         short  red = 0;                 //       ��������
	         short  green = 0;               //���-�� �������
	         short  blue = 0;                //       ������

	         int    born = 0;      //����������� ����� ������� ��� �������� �������

	bot(const unsigned int X, const unsigned int Y, bot* parent = nullptr, size_t N = 0, const bool free = true);
	void death(); //��������� ������

	void incIP(unsigned int num = 1); //������� ��e������� ��������� ������� �������
	unsigned short getParam();        //������� ��������� ���������

	unsigned int getX(unsigned short direct); //��������� �-���������� �� �����������
	unsigned int getY(unsigned short direct); //��������� Y-���������� �� �����������

	bool isRelative(bot _bot);                //����������, ����������� �� _bot

	void goRed  (short power);
	void goGreen(short power);
	void goBlue (short power);
	void print(std::string a);

	void step(); //������� ������� ����e������������
};

void radiation();

#endif //BOT_H