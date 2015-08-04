#ifndef _GAME_AI_H_
#define _GAME_AI_H_

#include "cocos2d.h"
#include "Constant.h"

USING_NS_CC;

class GameAI :public Node
{
public:
	CREATE_FUNC(GameAI);
	void selectLevel();
	void addNumber();
	int* randomNumber(int* arrayNum,int size);
	int* randomLessNumber(int* arrNum,int size);
	char* randomComputation(char *arr,int size,int level);
	int calculate(int a,int b,int c,char com1,char com2);
	int calculate2(int a, int b, char com);
	void printArray(int* arrayNum,int size);
	void printChar(char* arr,int size);
};

#endif
