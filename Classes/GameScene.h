#ifndef __GAME_SCENE_H__
#define __GAME_SCENE_H__

#include "cocos2d.h"
#include "SimpleAudioEngine.h"
#include "GameAI.h"
#include "Constant.h"

USING_NS_CC;

class GameScene: public cocos2d::Layer {
public:
	// there's no 'id' in cpp, so we recommend returning the class instance pointer
	static cocos2d::Scene* createScene();

	// Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
	virtual bool init();

	// implement the "static create()" method manually
	CREATE_FUNC(GameScene)
	;
	//catch key
	virtual void onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event);

	void preloadMusic();
	void gameStart(Object *pSender,int level);
	void gameSelectLevel();
	void gameSetting(Object* pSender);
	void gameHighScore(Object* pSender);
	void gameHelp(Object* pSender);
	void gameHallOfFrame();

	void getSoundMusic();
	void setSoundMusic();
	void gameCloseMenu();
	void gameOnSound();
	void gameOffSound();
	void gameOnMusic();
	void gameOffMusic();
	void showTextTime(LabelTTF *showtext, float time);
private:
	GameState gameState;
	Sprite* boardMenu;
	Node* homeButton;
	bool onSound;
	bool onMusic;
	Sprite* stickMusic;
	Sprite* stickSound;
	float posOnX;
	float posOffX;
};

#endif // __GAME_SCENE_H__
