#ifndef __GAME_PLAY_H__
#define __GAME_PLAY_H__

#include "cocos2d.h"
#include "SimpleAudioEngine.h"
#include "GameAI.h"
#include "Constant.h"
#include "TouchSprite.h"
#include "GameScene.h"

USING_NS_CC;

class GamePlay: public cocos2d::Layer {

public:
	static GamePlay *inst;
	static GamePlay* instance();
	inline  void setLevel(int level) {
		levelGame = level;
	};
	inline int getLevel() {
		return levelGame;
	};
	GamePlay();
	GamePlay(int);
	// there's no 'id' in cpp, so we recommend returning the class instance pointer
	static cocos2d::Scene* createScene();

	// Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
	virtual bool init();

	// implement the "static create()" method manually
	CREATE_FUNC(GamePlay);

	//catch key
	virtual void onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event);

	void loadGameAI(int* arrNum, char* arrCOM);
	void setPosCharacterBtn(Sprite* comBtn[], TouchSprite* framePlay[],
			float paddingX);

	//using touch functions
	bool onTouchBegan(Touch *touch, Event *unused_event);
	virtual void onTouchMoved(Touch *touch, Event *unused_event);
	virtual void onTouchEnded(Touch *touch, Event *unused_event);

	// update
	virtual void update(float delta);

	bool checkAllInFramePlay();
	void setAllScore(TouchSprite* framePlay[], int tempResult[], char* arrCOM);
	bool checkCalculate(int result[], int tempResult[]);

	//show time
	void showTime(float time);
	void gameOver(float dt);
	void gamePause();
	void gameSetting();
	void gameHelp();
	void gameQuit();

	void gameReplay();
	void gameResume();
	void gameShare();
	void gameShareCloseMenuPause();
	void gameCloseMenu();
	void gameCloseMenuSetting();

	void gameCloseWinBoard();
	void gameShareWin();
	void gameShowMiniBoard();
	void updatehightestRecord();
	void showTextTime(LabelTTF *showtext, float time);

	void gameOnSound();
	void gameOffSound();
	void gameOnMusic();
	void gameOffMusic();
	void getSoundMusic();
	void setSoundMusic();

	char* getLevelString(int level);
	char* getLeaderBoard(int level);
	void afterCaptured(bool succeed, const std::string& outputFile);
	void afterCapturedShowMenu(bool succeed, const std::string& outputFile);
private:
	int levelGame;
	GameState gameState;
	Size visibleSize;
	int* arrNum;
	int arrNUM[SUM_NUM];
	char* arrCOM;
	char arrCOMPUTATE[SUM_COM];
	GameAI* gameAI;
	Sprite* frameReady[SUM_NUM];
	TouchSprite* numberBtn[SUM_NUM];
	Sprite* comBtn[SUM_COM];
	int result[SUM_COM / 2];
	LabelTTF* resultLabel[SUM_COM / 2];
	bool checkWin;
	int tempResult[SUM_COM / 2];
	TouchSprite* framePlay[SUM_NUM];
	float timeplay;
	float highScore;
	Sprite* boardMenuPause;
	Sprite* boardMenuHelp;
	Sprite* boardMenuSetting;
	Sprite* boardWin;

	bool onSound;
	bool onMusic;
	float posOnX;
	float posOffX;
	Sprite* stickMusic;
	Sprite* stickSound;
};

#endif // __GAME_PLAY_H__
