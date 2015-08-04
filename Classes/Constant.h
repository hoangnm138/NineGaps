#ifndef _CONSTANT_H_
#define _CONSTANT_H_

//file contents here
enum GameState
{
	Welcome,
	Ready,
	Start,
	Playing,
	Setting,
	Pause,
	End
};

enum COMPUTATION
{
	plus = '+',
	minus = '-',
	multiply = '*',
	divide = '/'
};
const int INIT_FRAME = 2000;

const int SUM_COM = 12;
const int SUM_NUM = 9;
const int PADDING = 20;
const int TAG_LOGO = 100;
const int TAG_PLAYBTN = 101;
const int TAG_CONTINUED = 102;
const int TAG_SETTING = 103;
const int TAG_HIGHSCORE = 104;
const int TAG_HELP = 105;
const int TAG_QUIT = 106;

const int TAG_START_READY_POINT = 1001;
const int TAG_NUMBER = 2001;
const int TAG_FRAME_PLAY = 3001;
const int TAG_FRAME_EQUAL = 4001;
const int TAG_PAUSE_BTN = 5001;
const int TAG_LABEL_COUNT_TIME = 6000;

const int TAG_BTN_ON_SOUND = 111;
const int TAG_BTN_OFF_SOUND = 112;
const int TAG_BTN_ON_MUSIC = 113;
const int TAG_BTN_OFF_MUSIC = 114;
const int TAG_STICK_SOUND = 115;
const int TAG_STICK_MUSIC = 116;
#endif
