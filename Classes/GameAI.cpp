#include "GameAI.h"

int GameAI::calculate2(int a, int b, char com) {
	int result = 0;
	switch (com) {
	case '+':
		result = a + b;
		break;
	case '-':
		result = a - b;
		break;
	case '*':
		result = a * b;
		break;
	case '/':
		result = a / b;
		break;
	}
	return result;
}

int GameAI::calculate(int a, int b, int c, char com1, char com2) {
	int result = 0;

	if (com2 == '*' || com2 == '/') {
		int temp = 0;
		temp = calculate2(b, c, com2);
		result = calculate2(a, temp, com1);
	} else {
		int temp = 0;
		temp = calculate2(a, b, com1);
		result = calculate2(temp, c, com2);
	}
	//CCLog("%d%c%d%c%d=%d",a,com1,b,com2,c,result);
	return result;
}

int* GameAI::randomNumber(int* arrayNum, int size) {
	srand((unsigned) time(NULL));
	for (int i = 0; i < size; i++) {
		int index = rand() % size;
		std::swap(arrayNum[index], arrayNum[i]);
	}
//	printArray(arrayNum, size);
	return arrayNum;
}

int* GameAI::randomLessNumber(int* arrNum, int size) {
	srand((unsigned) time(NULL));
	int i = 0, number = 0;
	bool kt;
	do {
		kt = true;
		number = rand() % SUM_NUM + 1;
		for (int i = 0; i < size; i++) {
			if (arrNum[i] == number) {
				kt = false;
				break;
			}
		}
		if (kt == true) {
			arrNum[i] = number;
			i++;
		}
	} while (i < size);
//	printArray(arrNum,size);
	return arrNum;
}

char* GameAI::randomComputation(char *arr, int size, int level) {
	srand((unsigned) time(NULL));
	int k = 0;
	for (int i = 0; i < size; i++) {
		int index = rand() % level;
		switch (index) {
		case 0:
			arr[i] = '+';
			break;
		case 1:
			arr[i] = '-';
			break;
		case 2:
			if (k < 5 - level) {
				arr[i] = '*';
				k++;
			}else{
				arr[i] = (level % 2)>0 ? '-' : '+';
			}
			break;
		case 3:
			arr[i] = '/';
			break;
		}
	}
	printChar(arr, size);
	return arr;
}

void GameAI::printArray(int* arrayNum, int size) {
	CCLog("Print Array:\n");
	for (int i = 0; i < size; i++) {
		CCLog("%d ", arrayNum[i]);
	}
	CCLog("\n");
}

void GameAI::printChar(char* arr, int size) {
	CCLog("Print string:\n");
	for (int i = 0; i < size; i++) {
		CCLog("%c ", arr[i]);
	}
	CCLog("\n");
}
