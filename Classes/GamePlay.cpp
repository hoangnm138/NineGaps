#include "GamePlay.h"
#include "GameScene.h"
#include "ui/CocosGUI.h"
#include "extensions/GUI/CCControlExtension/CCControlExtensions.h"
#include "platform/android/jni/JniHelper.h"
#include "AdmobHelper.h"
#include "NativeUtils.h"
#include "PlayGameConstants.h"
#include<string>

USING_NS_CC;
Scene* GamePlay::createScene() {
	// 'scene' is an autorelease object
	auto scene = Scene::create();

	// 'layer' is an autorelease object
	auto layer = GamePlay::create();

	// add layer as a child to scene
	scene->addChild(layer);

	// return the scene
	return scene;
}

GamePlay* GamePlay::inst = 0;
GamePlay* GamePlay::instance() {
	if (!inst)
		inst = new GamePlay();
	return inst;
}

GamePlay::GamePlay() {
	checkWin = false;
	timeplay = 0.0;
}

GamePlay::GamePlay(int _initialValueX) :
		levelGame(_initialValueX) {
}

bool GamePlay::init() {
	if (!Layer::init()) {
		return false;
	}

	gameState = Ready;
	visibleSize = Director::getInstance()->getVisibleSize();
	getSoundMusic();

	//load game AI
	//init array number and computation
	int ARRAY_NUMBER[9] = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };
	arrNum = gameAI->randomNumber(ARRAY_NUMBER, SUM_NUM);
	for (int i = 0; i < SUM_NUM; i++) {
		arrNUM[i] = arrNum[i];
	}
	char s[SUM_COM] = "";
	//TODO change level
//	arrCOM = gameAI->randomComputation(s, SUM_COM,
//			GamePlay::instance()->getLevel() >= 2 ? 2 : 3);
	arrCOM = gameAI->randomComputation(s, SUM_COM, 2);
	for (int i = 0; i < SUM_COM; i++) {
		arrCOMPUTATE[i] = arrCOM[i];
	}
	for (int i = 0; i < SUM_COM / 2; i++) {
		tempResult[i] = INIT_FRAME;
	}

	int arr[GamePlay::instance()->getLevel()];
	int* arrayLevel = gameAI->randomLessNumber(arr,
			GamePlay::instance()->getLevel());

	//load background
	auto bg = Sprite::create("background.png");
	bg->setAnchorPoint(Vec2(0, 0));
	bg->setPosition(Vec2(0, 0));
	this->addChild(bg, -1);
	float rX = visibleSize.width / bg->getContentSize().width;
	float rY = visibleSize.height / bg->getContentSize().height;
	bg->setScaleX(rX);
	bg->setScaleY(rY);

	//show logo game
	Sprite *logoGame = Sprite::create("smalllogo.png");
	logoGame->setAnchorPoint(Vec2(0.0, 1.0));
	logoGame->setPosition(Vec2(PADDING, visibleSize.height - PADDING));
	logoGame->setTag(TAG_LOGO);
	logoGame->setScale(0.6f);
	this->addChild(logoGame);

	//load frame ready
	float baseY = logoGame->getPositionY()
			- logoGame->getBoundingBox().size.height - PADDING;
	float baseX = PADDING / 2;
	for (int i = 0; i < SUM_NUM; i++) {
		frameReady[i] = Sprite::create("frame.png");
		float width = frameReady[i]->getContentSize().width;
		float height = frameReady[i]->getContentSize().height;
		frameReady[i]->setTag(TAG_START_READY_POINT + i);
		frameReady[i]->setPositionX(baseX + 5.0 / 8.0 * i * width + width / 2);
		if (i % 2 == 0) {
			frameReady[i]->setPositionY(baseY - height / 2);
		} else {
			frameReady[i]->setPositionY(baseY - height);
		}
		this->addChild(frameReady[i]);
	}

	//create frame play
	float paddingY = 0;
	float paddingX = 0;
	for (int i = 0; i < SUM_NUM; i++) {
		float baseX = PADDING;
		framePlay[i] = TouchSprite::create("frame.png");
		framePlay[i]->setTag(TAG_FRAME_PLAY + i);
		paddingX = (visibleSize.width - PADDING * 4
				- framePlay[i]->getContentSize().width) / 3.0;
		framePlay[i]->setPositionX(
				PADDING + framePlay[i]->getContentSize().width / 2
						+ paddingX * (i % 3));

		float baseY = frameReady[0]->getPositionY();
		if (i % 3 == 0)
			paddingY = paddingX * ((int) i / 3 + 1);
		framePlay[i]->setPositionY(baseY - paddingY);
		framePlay[i]->score = 0;
		framePlay[i]->isFixed = false;
		for (int j = 0; j < GamePlay::instance()->getLevel(); j++) {
			if (arrNum[i] == arrayLevel[j]) {
				framePlay[i]->score = arrNum[i];
				//TODO
				framePlay[i]->isFixed = true;
				framePlay[i]->setDisplayFrame(
						SpriteFrame::create(
								"frame" + std::to_string(arrNum[i]) + ".png",
								framePlay[i]->getTextureRect()));
			}
		}
//		CCLog("frameplay[%d] = %d", i, framePlay[i]->score);
		this->addChild(framePlay[i]);
	}

	//load number button
	//TODO
	std::string name = "number";
	MoveTo* actionBy;
	int k = 0;
	for (int i = 0; i < SUM_NUM; i++) {
		numberBtn[i] = TouchSprite::create(
				name + std::to_string(i + 1) + ".png");
		numberBtn[i]->setTag(TAG_NUMBER + i);
		numberBtn[i]->setPositionY(visibleSize.height + 3 * PADDING);
		numberBtn[i]->setPositionX(frameReady[i]->getPositionX());
		numberBtn[i]->setZOrder(10);
		numberBtn[i]->isHidden = false;
		this->addChild(numberBtn[i]);
		actionBy = MoveTo::create(0.4, frameReady[i]->getPosition());
		auto easeAction = EaseBounceInOut::create(actionBy);

		for (int j = 0; j < GamePlay::instance()->getLevel(); j++) {
			if ((i + 1) == arrayLevel[j]) {
				numberBtn[i]->isHidden = true;
				numberBtn[i]->setVisible(false);
			}
		}

		if (numberBtn[i]->isHidden == false) {
			auto delay = DelayTime::create(0.2 * (k + 1));
			auto sequence = Sequence::create(delay, easeAction, nullptr);
			numberBtn[i]->runAction(sequence);
			k++;
		}
	}

	if (frameReady != NULL) {
		for (int i = 0; i < SUM_NUM; i++)
			frameReady[i]->setVisible(false);
	}

	//create result frame and equal character
	Sprite* frameEqual[SUM_COM / 2];
	Sprite* characterEqual[SUM_COM / 2];
	for (int i = 0; i < SUM_COM / 2; i++) {
		characterEqual[i] = Sprite::create("icon_equal.png");
		frameEqual[i] = Sprite::createWithTexture(frameReady[i]->getTexture());
		frameEqual[i]->setTag(TAG_FRAME_EQUAL + i);
		if (i % 2 == 0) {
			//frame equal
			frameEqual[i]->setPositionY(
					framePlay[SUM_NUM - 1]->getPositionY()
							+ paddingX * (2 - (int) i / 2));
			frameEqual[i]->setPositionX(
					framePlay[SUM_NUM - 1]->getPositionX() + paddingX);
			//character equal
			characterEqual[i]->setPositionY(
					framePlay[SUM_NUM - 1]->getPositionY()
							+ paddingX * ((int) i / 2));
			characterEqual[i]->setPositionX(
					framePlay[SUM_NUM - 1]->getPositionX() + paddingX / 2);
		} else {
			//frame equal
			frameEqual[i]->setPositionY(
					framePlay[SUM_NUM - 1]->getPositionY() - paddingX);
			frameEqual[i]->setPositionX(
					framePlay[SUM_NUM - 1]->getPositionX()
							- paddingX * (2 - (int) i / 2));
			//character equal
			characterEqual[i]->setPositionY(
					framePlay[SUM_NUM - 1]->getPositionY() - paddingX / 2);
			characterEqual[i]->setPositionX(
					framePlay[SUM_NUM - 1]->getPositionX()
							- paddingX * (2 - (int) i / 2));
		}
		this->addChild(frameEqual[i]);
		this->addChild(characterEqual[i]);
	}

	//position of character computation
	Sprite* plusSprite = Sprite::create("icon_plus.png");
	Sprite* minusSrite = Sprite::create("icon_minus.png");
	Sprite* mutiplySprite = Sprite::create("icon_multiply.png");
	Sprite* divideSprite = Sprite::create("icon_divide.png");

	for (int i = 0; i < SUM_COM; i++) {
		if (arrCOM[i] == '+')
			comBtn[i] = Sprite::createWithTexture(plusSprite->getTexture());
		else if (arrCOM[i] == '-')
			comBtn[i] = Sprite::createWithTexture(minusSrite->getTexture());
		else if (arrCOM[i] == '*')
			comBtn[i] = Sprite::createWithTexture(mutiplySprite->getTexture());
		else if (arrCOM[i] == '/')
			comBtn[i] = Sprite::createWithTexture(divideSprite->getTexture());
		this->addChild(comBtn[i]);
	}
	setPosCharacterBtn(comBtn, framePlay, paddingX);

	//calculate result
	result[0] = gameAI->calculate(arrNum[0], arrNum[1], arrNum[2], arrCOM[0],
			arrCOM[1]);
	result[1] = gameAI->calculate(arrNum[0], arrNum[3], arrNum[6], arrCOM[2],
			arrCOM[7]);
	result[2] = gameAI->calculate(arrNum[3], arrNum[4], arrNum[5], arrCOM[5],
			arrCOM[6]);
	result[3] = gameAI->calculate(arrNum[1], arrNum[4], arrNum[7], arrCOM[3],
			arrCOM[8]);
	result[4] = gameAI->calculate(arrNum[6], arrNum[7], arrNum[8], arrCOM[10],
			arrCOM[11]);
	result[5] = gameAI->calculate(arrNum[2], arrNum[5], arrNum[8], arrCOM[4],
			arrCOM[9]);
	gameAI->printArray(result, SUM_COM / 2);

	//set result to frameEqual
	for (int i = 0; i < SUM_COM / 2; i++) {
		resultLabel[i] = LabelTTF::create("00:00", "fonts/hlvgchac.ttf", 48);
		resultLabel[i]->setColor(Color3B(Color3B::RED));
		resultLabel[i]->setString(std::to_string(result[i]));
		resultLabel[i]->setPosition(frameEqual[i]->getPosition());
		this->addChild(resultLabel[i]);
	}

	//button pause
	Sprite* pauseGame = Sprite::create("pauseBtn.png");
	Sprite* pauseGame_press = Sprite::create("pauseBtn_press.png");
	MenuItemSprite* pauseBtn = MenuItemSprite::create(pauseGame,
			pauseGame_press, NULL, CC_CALLBACK_0(GamePlay::gamePause, this));
	Menu *menuPause = Menu::create(pauseBtn, nullptr);
	menuPause->setPosition(
			Vec2(
					visibleSize.width - PADDING
							- pauseBtn->getContentSize().width / 2,
					visibleSize.height - PADDING
							- pauseBtn->getContentSize().height / 2));
	menuPause->setTag(TAG_PAUSE_BTN);
	this->addChild(menuPause);

	//time in pause button
	LabelTTF *currenttime = LabelTTF::create("00:00", "fonts/hlvgchac.ttf", 40);
	Point timePoint = menuPause->getPosition();
	currenttime->setPosition(
			Vec2(timePoint.x - PADDING * 2, timePoint.y + 7.0 / 4.0 * PADDING));
	currenttime->setZOrder(10);
	currenttime->setColor(Color3B(Color3B::RED));
	currenttime->setTag(TAG_LABEL_COUNT_TIME);
	this->addChild(currenttime);

	std::string miniModeSprite = getLevelString(
			GamePlay::instance()->getLevel());
	Sprite* miniMode = Sprite::create("mini" + miniModeSprite + ".png");
	miniMode->setScale(0.5);
	miniMode->setPosition(
			Vec2(currenttime->getPositionX(),
					currenttime->getPositionY()
							- currenttime->getContentSize().height
							- PADDING / 2));
	this->addChild(miniMode);

	//board menu pause
	boardMenuPause = Sprite::create("board.png");
	boardMenuPause->setPosition(
			Vec2(visibleSize.width / 2,
					visibleSize.height
							+ boardMenuPause->getContentSize().height / 2
							+ PADDING));
	boardMenuPause->setZOrder(30);
	this->addChild(boardMenuPause);

	//button help of menu
	Sprite* helpbtn = Sprite::create("help1.png");
	Sprite* helpbtn_press = Sprite::create("help2.png");
	MenuItemSprite* helpBtn = MenuItemSprite::create(helpbtn, helpbtn_press,
	NULL, CC_CALLBACK_0(GamePlay::gameHelp, this));
	Menu *menuHelp = Menu::create(helpBtn, nullptr);
	menuHelp->setPosition(
			Vec2(boardMenuPause->getContentSize().width / 2,
					boardMenuPause->getContentSize().height
							- helpbtn->getContentSize().height));
	boardMenuPause->addChild(menuHelp);

	//button setting of menu
	Sprite* setbtn = Sprite::create("settings1.png");
	Sprite* setbtn_press = Sprite::create("settings2.png");
	MenuItemSprite* setBtn = MenuItemSprite::create(setbtn, setbtn_press, NULL,
			CC_CALLBACK_0(GamePlay::gameSetting, this));
	Menu *menuSetting = Menu::create(setBtn, nullptr);
	menuSetting->setPosition(
			Vec2(boardMenuPause->getContentSize().width / 2,
					menuHelp->getPositionY() - setbtn->getContentSize().height
							- PADDING));
	menuSetting->setTag(TAG_SETTING);
	boardMenuPause->addChild(menuSetting);

	//button quit of menu
	Sprite* quitbtn = Sprite::create("quit1.png");
	Sprite* quitbtn_press = Sprite::create("quit2.png");
	MenuItemSprite* quitBtn = MenuItemSprite::create(quitbtn, quitbtn_press,
	NULL, CC_CALLBACK_0(GamePlay::gameQuit, this));
	Menu *menuQuit = Menu::create(quitBtn, nullptr);
	menuQuit->setPosition(
			Vec2(menuSetting->getPositionX(),
					menuSetting->getPositionY()
							- quitbtn->getContentSize().height - PADDING));
	menuQuit->setTag(TAG_QUIT);
	boardMenuPause->addChild(menuQuit);

	//button replay, resume, share
	MenuItemImage* resumeBtn = MenuItemImage::create("menu_resume.png",
			"menu_resume.png", CC_CALLBACK_0(GamePlay::gameResume, this));
	auto menuResume = Menu::create(resumeBtn, nullptr);
	menuResume->setPosition(
			Vec2(menuSetting->getPositionX(),
					menuQuit->getPositionY()
							- resumeBtn->getContentSize().height - PADDING));
	boardMenuPause->addChild(menuResume);

	MenuItemImage* replayBtn = MenuItemImage::create("menu_replay.png",
			"menu_replay.png", CC_CALLBACK_0(GamePlay::gameReplay, this));
	auto menuReplay = Menu::create(replayBtn, nullptr);
	menuReplay->setPosition(
			Vec2(
					menuResume->getPositionX()
							- resumeBtn->getContentSize().width - PADDING,
					menuResume->getPositionY()));
	boardMenuPause->addChild(menuReplay);

	MenuItemImage* shareBtn = MenuItemImage::create("menu_share.png",
			"menu_share.png",
			CC_CALLBACK_0(GamePlay::gameShareCloseMenuPause, this));
	auto menuShare = Menu::create(shareBtn, nullptr);
	menuShare->setPosition(
			Vec2(
					menuResume->getPositionX()
							+ resumeBtn->getContentSize().width + PADDING,
					menuResume->getPositionY()));
	boardMenuPause->addChild(menuShare);

	auto event_listener = EventListenerTouchOneByOne::create();
	auto dispatcher = Director::getInstance()->getEventDispatcher();
	event_listener->setSwallowTouches(true);
	event_listener->onTouchBegan = CC_CALLBACK_2(GamePlay::onTouchBegan, this);
	event_listener->onTouchMoved = CC_CALLBACK_2(GamePlay::onTouchMoved, this);
	event_listener->onTouchEnded = CC_CALLBACK_2(GamePlay::onTouchEnded, this);
	dispatcher->addEventListenerWithSceneGraphPriority(event_listener, this);
	this->setKeypadEnabled(true);
	this->scheduleUpdate();
//	this->schedule(schedule_selector(GamePlay::update), 1.0f);
	gameState = Playing;
	return true;
}

void GamePlay::gameResume() {
	if (onSound)
		CocosDenshion::SimpleAudioEngine::sharedEngine()->playEffect(
				"sound/button.mp3");
	auto actionBy = MoveTo::create(0.4,
			Vec2(visibleSize.width / 2,
					visibleSize.height
							+ boardMenuPause->getContentSize().height / 2
							+ PADDING));
	boardMenuPause->runAction(actionBy);
	gameState = Playing;
	this->scheduleUpdate();
}

void GamePlay::gameReplay() {
	if (onSound)
		CocosDenshion::SimpleAudioEngine::sharedEngine()->playEffect(
				"sound/button.mp3");
	auto gamePlay = GamePlay::createScene();
	auto transition = TransitionFade::create(1.0, gamePlay);
	Director::getInstance()->replaceScene(transition);
}

void GamePlay::gameShareCloseMenuPause() {
	if (onSound)
		CocosDenshion::SimpleAudioEngine::sharedEngine()->playEffect(
				"sound/button.mp3");
	boardMenuPause->setVisible(false);
	utils::captureScreen(CC_CALLBACK_2(GamePlay::afterCapturedShowMenu, this),
			"screenshot.png");
}

void GamePlay::afterCapturedShowMenu(bool succeed,
		const std::string& outputFile) {
	if (succeed) {
		JniMethodInfo t;
		if (JniHelper::getStaticMethodInfo(t, "org/cocos2dx/cpp/AppActivity",
				"transferData", "()V")) {
			t.env->CallStaticVoidMethod(t.classID, t.methodID);
			t.env->DeleteLocalRef(t.classID);
		}

		if (JniHelper::getStaticMethodInfo(t, "org/cocos2dx/cpp/AppActivity",
				"share", "()V")) {
			t.env->CallStaticIntMethod(t.classID, t.methodID);
		}

	} else {
		CCLog("Capture screen failed.");
	}
	boardMenuPause->setVisible(true);
}

void GamePlay::gameShare() {
	if (onSound)
		CocosDenshion::SimpleAudioEngine::sharedEngine()->playEffect(
				"sound/button.mp3");
	utils::captureScreen(CC_CALLBACK_2(GamePlay::afterCaptured, this),
			"screenshot.png");
}

void GamePlay::gameQuit() {
	if (onSound)
		CocosDenshion::SimpleAudioEngine::sharedEngine()->playEffect(
				"sound/button.mp3");
	setSoundMusic();
	auto gameScene = GameScene::createScene();
	auto transition = TransitionFade::create(1.0, gameScene);
	Director::getInstance()->replaceScene(transition);
}

void GamePlay::gameHelp() {
	if (onSound)
		CocosDenshion::SimpleAudioEngine::sharedEngine()->playEffect(
				"sound/button.mp3");
	boardMenuPause->setVisible(false);

	boardMenuHelp = Sprite::create("board.png");
	boardMenuHelp->setPosition(
			Vec2(visibleSize.width / 2, visibleSize.height / 2));
	boardMenuHelp->setZOrder(30);
	this->addChild(boardMenuHelp);

	float haflHeight = boardMenuHelp->getContentSize().height / 2;
	float haflWidth = boardMenuHelp->getContentSize().width / 2;

	Sprite* text = Sprite::create("howtoplaytext.png");
	text->setPosition(Vec2(haflWidth + PADDING / 2, haflHeight));
	text->setZOrder(40);
	boardMenuHelp->addChild(text);

	MenuItemImage* closeBtn = MenuItemImage::create("menu_closeBtn.png",
			"menu_closeBtn.png", CC_CALLBACK_0(GamePlay::gameCloseMenu, this));
	auto menuClose = Menu::create(closeBtn, nullptr);
	menuClose->setZOrder(40);
	menuClose->setPosition(
			Vec2(haflWidth * 2 - closeBtn->getContentSize().width - PADDING,
					haflHeight * 2 - closeBtn->getContentSize().height
							- PADDING));
	boardMenuHelp->addChild(menuClose);
}

void GamePlay::gameCloseMenu() {
	if (onSound)
		CocosDenshion::SimpleAudioEngine::sharedEngine()->playEffect(
				"sound/button.mp3");
	if (boardMenuHelp != NULL) {
		if (boardMenuHelp->isVisible() == true)
			boardMenuHelp->setVisible(false);
	}
	if (boardMenuPause != NULL) {
		if (boardMenuPause->isVisible() == false)
			boardMenuPause->setVisible(true);
	}

}

void GamePlay::gameCloseMenuSetting() {
	if (onSound)
		CocosDenshion::SimpleAudioEngine::sharedEngine()->playEffect(
				"sound/button.mp3");
	if (boardMenuSetting != NULL) {
		if (boardMenuSetting->isVisible() == true)
			boardMenuSetting->setVisible(false);
	}
	if (boardMenuPause != NULL) {
		if (boardMenuPause->isVisible() == false)
			boardMenuPause->setVisible(true);
	}
}

void GamePlay::gameSetting() {
	if (onSound)
		CocosDenshion::SimpleAudioEngine::sharedEngine()->playEffect(
				"sound/button.mp3");
	boardMenuPause->setVisible(false);

	boardMenuSetting = Sprite::create("board.png");
	boardMenuSetting->setPosition(
			Vec2(visibleSize.width / 2, visibleSize.height / 2));
	boardMenuSetting->setZOrder(30);
	this->addChild(boardMenuSetting);
	float haflHeight = boardMenuSetting->getContentSize().height / 2;
	float haflWidth = boardMenuSetting->getContentSize().width / 2;
	MenuItemImage* closeBtn = MenuItemImage::create("menu_closeBtn.png",
			"menu_closeBtn.png",
			CC_CALLBACK_0(GamePlay::gameCloseMenuSetting, this));
	auto menuClose = Menu::create(closeBtn, nullptr);
	menuClose->setZOrder(40);
	menuClose->setPosition(
			Vec2(haflWidth * 2 - closeBtn->getContentSize().width - PADDING,
					haflHeight * 2 - closeBtn->getContentSize().height
							- PADDING));
	boardMenuSetting->addChild(menuClose);

	Sprite* sound = Sprite::create("sound.png");
	sound->setPosition(
			Vec2(4 * PADDING + sound->getContentSize().width / 2,
					menuClose->getPositionY()
							- closeBtn->getContentSize().height - 2 * PADDING));
	sound->setZOrder(40);
	boardMenuSetting->addChild(sound);
	//button on/of sound
	MenuItemImage* onSoundBtn = MenuItemImage::create("onbutton.png",
			"onbutton.png", CC_CALLBACK_0(GamePlay::gameOnSound, this));
	auto menuOnSound = Menu::create(onSoundBtn, nullptr);
	menuOnSound->setZOrder(40);
	menuOnSound->setPosition(
			Vec2(haflWidth / 2 + PADDING,
					sound->getPositionY() - 2 * PADDING
							- onSoundBtn->getContentSize().height));
	boardMenuSetting->addChild(menuOnSound);

	MenuItemImage* offSoundBtn = MenuItemImage::create("offbutton.png",
			"offbutton.png", CC_CALLBACK_0(GamePlay::gameOffSound, this));
	auto menuOffSound = Menu::create(offSoundBtn, nullptr);
	menuOffSound->setZOrder(40);
	menuOffSound->setPosition(
			Vec2(haflWidth * 3 / 2 - PADDING, menuOnSound->getPositionY()));
	boardMenuSetting->addChild(menuOffSound);
	//stick sound
	stickSound = Sprite::create("stick.png");
	stickSound->setZOrder(50);
	stickSound->setTag(TAG_STICK_SOUND);
	stickSound->setPositionY(menuOnSound->getPositionY());
	boardMenuSetting->addChild(stickSound);
	this->posOnX = menuOnSound->getPositionX()
			- onSoundBtn->getContentSize().width / 4 - PADDING / 2;
	this->posOffX = menuOffSound->getPositionX()
			- offSoundBtn->getContentSize().width / 4 - PADDING / 2;
	if (onSound == true) {
		stickSound->setPositionX(this->posOnX);
	} else {
		stickSound->setPositionX(this->posOffX);
	}

	Sprite* music = Sprite::create("music.png");
	music->setZOrder(40);
	music->setPosition(
			Vec2(sound->getPositionX(),
					sound->getPositionY()
							- 3 * music->getContentSize().height));
	boardMenuSetting->addChild(music);

	//button on/of music
	MenuItemImage* onMusicBtn = MenuItemImage::create("onbutton.png",
			"onbutton.png", CC_CALLBACK_0(GamePlay::gameOnMusic, this));
	auto menuOnMusic = Menu::create(onMusicBtn, nullptr);
	menuOnMusic->setZOrder(40);
	menuOnMusic->setPosition(
			Vec2(menuOnSound->getPositionX(),
					music->getPositionY() - 2 * PADDING
							- onMusicBtn->getContentSize().height));
	boardMenuSetting->addChild(menuOnMusic);

	MenuItemImage* offMusicBtn = MenuItemImage::create("offbutton.png",
			"offbutton.png", CC_CALLBACK_0(GamePlay::gameOffMusic, this));
	auto menuOffMusic = Menu::create(offMusicBtn, nullptr);
	menuOffMusic->setZOrder(40);
	menuOffMusic->setPosition(
			Vec2(haflWidth * 3 / 2 - PADDING, menuOnMusic->getPositionY()));
	boardMenuSetting->addChild(menuOffMusic);

	//stick sound
	stickMusic = Sprite::create("stick.png");
	stickMusic->setZOrder(50);
	stickMusic->setTag(TAG_STICK_MUSIC);
	stickMusic->setPositionY(menuOnMusic->getPositionY());
	boardMenuSetting->addChild(stickMusic);
	if (onMusic == true) {
		stickMusic->setPositionX(this->posOnX);
	} else {
		stickMusic->setPositionX(this->posOffX);
	}
	//volumn setting
	/*cocos2d::extension::ControlSlider* slider =
	 cocos2d::extension::ControlSlider::create("sliderTrack2.png",
	 "sliderProgress2.png", "sliderThumb.png");
	 slider->setPosition(200, 200);
	 slider->setMinimumValue(0);
	 slider->setMaximumValue(100);
	 slider->setValue(50);
	 slider->addTargetWithActionForControlEvents(this, cccontrol_selector(IntroView::valueChangedCallback), cocos2d::extension::Control::EventType::VALUE_CHANGED);
	 addChild(slider);
	 void IntroView::valueChangedCallback(Ref* sender, cocos2d::extension::Control::EventType evnt)
	 {
	 float value = static_cast<cocos2d::extension::ControlSlider*>(sender)->getValue();
	 CCLOG(std::to_string(value).c_str());
	 }*/
}

void GamePlay::setSoundMusic() {
	UserDefault::sharedUserDefault()->setBoolForKey("onMusic", onMusic);
	UserDefault::sharedUserDefault()->flush();

	UserDefault::sharedUserDefault()->setBoolForKey("onSound", onSound);
	UserDefault::sharedUserDefault()->flush();
}

void GamePlay::getSoundMusic() {
	onMusic = UserDefault::sharedUserDefault()->getBoolForKey("onMusic");
	onSound = UserDefault::sharedUserDefault()->getBoolForKey("onSound");
}

void GamePlay::gameOnSound() {
	if (onSound == false) {
		onSound = true;
		stickSound->setPositionX(posOnX);
	}
}

void GamePlay::gameOffSound() {
	if (onSound == true) {
		onSound = false;
		stickSound->setPositionX(posOffX);
	}
}

void GamePlay::gameOnMusic() {
	if (onMusic == false) {
		onMusic = true;
		stickMusic->setPositionX(posOnX);
		if (CocosDenshion::SimpleAudioEngine::sharedEngine()->isBackgroundMusicPlaying()
				== false)
			CocosDenshion::SimpleAudioEngine::sharedEngine()->resumeBackgroundMusic();
	}
}

void GamePlay::gameOffMusic() {
	if (onMusic == true) {
		onMusic = false;
		stickMusic->setPositionX(posOffX);
		CocosDenshion::SimpleAudioEngine::sharedEngine()->pauseBackgroundMusic();
	}
}

void GamePlay::gamePause() {
	if (onSound)
		CocosDenshion::SimpleAudioEngine::sharedEngine()->playEffect(
				"sound/button.mp3");
	if (gameState == Playing) {
		gameState = Pause;
		auto actionBy = MoveTo::create(0.8,
				Vec2(visibleSize.width / 2, visibleSize.height / 2));
		auto easeAction = EaseElasticOut::create(actionBy);
		auto callback = CallFunc::create([this]() {
			this->unscheduleUpdate();
		});
		auto sequence = Sequence::create(easeAction, callback, nullptr);
		boardMenuPause->runAction(sequence);
	}
}

bool GamePlay::onTouchBegan(Touch *touch, Event *unused_event) {
//touch number button
	if (gameState == Playing) {
		for (int i = 0; i < SUM_NUM; i++) {
			TouchSprite* test = (TouchSprite*) this->getChildByTag(
					TAG_NUMBER + i);
			if (test->isTouchingSprite(touch) == true
					&& test->isHidden == false) {
				test->isTouch = true;
				test->setZOrder(20);
				return true;
			}
		}
	}
}

void GamePlay::onTouchMoved(Touch *touch, Event *unused_event) {
	if (gameState == Playing) {
		for (int i = 0; i < SUM_NUM; i++) {
			TouchSprite* test = (TouchSprite*) this->getChildByTag(
					TAG_NUMBER + i);
			if (test->isTouch == true)
				test->setPosition(touch->getLocation());
		}
	}
}

void GamePlay::onTouchEnded(Touch *touch, Event *unused_event) {
//touch end number button
	if (gameState == Playing) {
		for (int i = 0; i < SUM_NUM; i++) {
			TouchSprite* test = (TouchSprite*) this->getChildByTag(
					TAG_NUMBER + i);
			if (test->isTouch == true) {
				//in frame
				for (int j = 0; j < SUM_NUM; j++) {
					TouchSprite* frame = (TouchSprite*) this->getChildByTag(
							TAG_FRAME_PLAY + j);
					if (frame->isTouchingSprite(touch)
							&& frame->isFixed == false) {
						//di chuyen den o da co so
						if (frame->score != 0 && frame->score != (i + 1)) {
							TouchSprite* transform =
									(TouchSprite*) this->getChildByTag(
											TAG_NUMBER + (frame->score) - 1);
							if (test->posframe != NULL
									&& test->posframe->score != 0) {
								auto actionTo = MoveTo::create(0.2,
										test->posframe->getPosition());
								transform->runAction(actionTo);
								transform->posframe = test->posframe;
								test->posframe->score = frame->score;
							} else {
								auto actionTo =
										MoveTo::create(0.2,
												frameReady[(frame->score - 1)]->getPosition());
								transform->runAction(actionTo);
								transform->posframe = NULL;
							}
						} else {
							//set lai gia tri frame da chuyen
							if (test->posframe != NULL) {
								test->posframe->score = 0;
							}
						}

						//set gia tri frame moi
						frame->score = i + 1;
						auto actionTo = MoveTo::create(0.2,
								frame->getPosition());
						test->isTouch = false;
						test->posframe = frame;
						test->runAction(actionTo);
						test->setZOrder(10);
						if (onSound)
							CocosDenshion::SimpleAudioEngine::sharedEngine()->playEffect(
									"sound/tapNumber.mp3");
						return;
					}
				}

				//out frame
				test->setPosition(touch->getLocation());
				test->isTouch = false;
				test->setZOrder(10);

				//set frame score = 0
				if (test->posframe != NULL) {
					test->posframe->score = 0;
					test->posframe = NULL;
				}
			}
		}
	}
}

void GamePlay::showTextTime(LabelTTF *showtext, float time) {
	int seconds = (int) time % 60;
	int minutes = (int) (time / 60) % 60;
	int hours = (int) time / 3600;
	String* timeString = String::createWithFormat("%02d:%02d", minutes,
			seconds);
	showtext->setString(timeString->_string.c_str());
}

void GamePlay::showTime(float timeplay) {
	LabelTTF *currenttime = (LabelTTF*) this->getChildByTag(
			TAG_LABEL_COUNT_TIME);
	int seconds = (int) timeplay % 60;
	int minutes = (int) (timeplay / 60) % 60;
	int hours = (int) timeplay / 3600;
//	if (minutes < 60)
	String* timeString = String::createWithFormat("%02d:%02d", minutes,
			seconds);
//	else
//		timeString = String::createWithFormat("%02d:%02d:%02d", hours, minutes,
//				seconds);
	currenttime->setString(timeString->_string.c_str());
}

void GamePlay::gameCloseWinBoard() {
	if (onSound)
		CocosDenshion::SimpleAudioEngine::sharedEngine()->playEffect(
				"sound/button.mp3");
	AdmobHelper::displayInterstitial();
	if (boardWin != NULL) {
		//show mini board
		gameShowMiniBoard();

		auto actionBy = MoveTo::create(0.5,
				Vec2(-visibleSize.width / 2, visibleSize.height / 2));
		boardWin->runAction(actionBy);
	}
}

void GamePlay::gameShowMiniBoard() {

	Sprite* miniboard = Sprite::create("miniboard.png");
	miniboard->setPositionX(visibleSize.width / 2);
	miniboard->setPositionY(
			getChildByTag(TAG_LOGO)->getPositionY()
					- getChildByTag(TAG_LOGO)->getBoundingBox().size.height
					- miniboard->getContentSize().height / 2 - PADDING);
	this->addChild(miniboard);

//button replay, resume, share
	MenuItemImage* resumeBtn = MenuItemImage::create("menu_resume.png",
			"menu_resume.png", CC_CALLBACK_0(GamePlay::gameQuit, this));
	auto menuResume = Menu::create(resumeBtn, nullptr);
	menuResume->setPositionY(miniboard->getContentSize().height / 2);
	menuResume->setPositionX(miniboard->getContentSize().width / 2);
	miniboard->addChild(menuResume);

	MenuItemImage* replayBtn = MenuItemImage::create("menu_replay.png",
			"menu_replay.png", CC_CALLBACK_0(GamePlay::gameReplay, this));
	auto menuReplay = Menu::create(replayBtn, nullptr);
	menuReplay->setPositionY(miniboard->getContentSize().height / 2);
	menuReplay->setPositionX(miniboard->getContentSize().width / 4);
	miniboard->addChild(menuReplay);

	MenuItemImage* sharebtn = MenuItemImage::create("menu_share.png",
			"menu_share.png", CC_CALLBACK_0(GamePlay::gameShare, this));
	auto menuShare = Menu::create(sharebtn, nullptr);
	menuShare->setPositionY(miniboard->getContentSize().height / 2);
	menuShare->setPositionX(miniboard->getContentSize().width * 3 / 4);
	miniboard->addChild(menuShare);
}

void GamePlay::afterCaptured(bool succeed, const std::string& outputFile) {
	if (succeed) {
		JniMethodInfo t;
		if (JniHelper::getStaticMethodInfo(t, "org/cocos2dx/cpp/AppActivity",
				"transferData", "()V")) {
			t.env->CallStaticVoidMethod(t.classID, t.methodID);
			t.env->DeleteLocalRef(t.classID);
		}

		if (JniHelper::getStaticMethodInfo(t, "org/cocos2dx/cpp/AppActivity",
				"share", "()V")) {
			t.env->CallStaticIntMethod(t.classID, t.methodID);
		}

	} else {
		CCLog("Capture screen failed.");
	}
}

void GamePlay::gameShareWin() {
	if (onSound)
		CocosDenshion::SimpleAudioEngine::sharedEngine()->playEffect(
				"sound/button.mp3");
	utils::captureScreen(CC_CALLBACK_2(GamePlay::afterCaptured, this),
			"screenshot.png");

//	CCSize size = CCDirector::sharedDirector()->getWinSize();
//
//	CCRenderTexture* texture = CCRenderTexture::create((int) size.width,
//			(int) size.height, kCCTexture2DPixelFormat_RGBA8888);
//	texture->setPosition(ccp(size.width / 2, size.height / 2));
//	texture->begin();
//	CCDirector::sharedDirector()->getRunningScene()->visit();
//	texture->end();
//
//	bool result = texture->saveToFile("scrshot.png", kCCImageFormatPNG);
}

char* GamePlay::getLevelString(int level) {
	char* levelName = "";
	switch (level) {
	case 4:
		levelName = "Novice";
		break;
	case 3:
		levelName = "Medium";
		break;
	case 2:
		levelName = "Advanced";
		break;
	case 1:
		levelName = "Expert";
		break;
	case 0:
		levelName = "Master";
		break;
	}
	return levelName;
}

char* GamePlay::getLeaderBoard(int level) {
	char* leaderboard = "";
	switch (level) {
	case 4:
		leaderboard = LEADERBOARD_NOVICE;
		break;
	case 3:
		leaderboard = LEADERBOARD_MEDIUM;
		break;
	case 2:
		leaderboard = LEADERBOARD_ADVANCED;
		break;
	case 1:
		leaderboard = LEADERBOARD_EXPERT;
		break;
	case 0:
		leaderboard = LEADERBOARD_MASTER;
		break;
	}
	return leaderboard;
}

void GamePlay::updatehightestRecord() {
	float beforescore = UserDefault::sharedUserDefault()->getFloatForKey(
			getLevelString(GamePlay::instance()->getLevel()), 7200.0);
	if (beforescore > timeplay) {
		UserDefault::sharedUserDefault()->setFloatForKey(
				getLevelString(GamePlay::instance()->getLevel()), timeplay);
		UserDefault::sharedUserDefault()->flush();
		this->highScore = timeplay;
	} else {
		this->highScore = beforescore;
	}
}

void GamePlay::gameOver(float dt) {
//	Director::getInstance()->pause();
	gameState = End;
//invisible frame ready
	if (frameReady != NULL) {
		for (int i = 0; i < SUM_NUM; i++)
			frameReady[i]->setVisible(false);
	}

	boardWin = Sprite::create("board.png");
	boardWin->setPosition(Vec2(-visibleSize.width / 4, visibleSize.height / 2));
	boardWin->setZOrder(30);
	this->addChild(boardWin);
	float haflHeight = boardWin->getContentSize().height / 2;
	float haflWidth = boardWin->getContentSize().width / 2;

	auto jumpto = JumpTo::create(0.5,
			Vec2(visibleSize.width / 2, visibleSize.height / 2 - 3 * PADDING),
			PADDING * 3, 4);
	boardWin->runAction(jumpto);

	updatehightestRecord();

	if (onSound)
		CocosDenshion::SimpleAudioEngine::sharedEngine()->playEffect(
				"sound/music_win.ogg");
//congratulation
	Sprite* congratulation = Sprite::create("congratulation.png");
	congratulation->setPosition(
			Vec2(haflWidth,
					haflHeight * 2 - 3 * PADDING
							- congratulation->getContentSize().height));
	congratulation->setZOrder(40);
	boardWin->addChild(congratulation);

//highscore
	Sprite* newhighscore = Sprite::create("newhighscore.png");
	newhighscore->setPosition(
			Vec2(haflWidth,
					congratulation->getPositionY()
							- congratulation->getContentSize().height / 2
							- newhighscore->getContentSize().height / 2
							- 2 * PADDING));
	newhighscore->setZOrder(40);
	boardWin->addChild(newhighscore);

	//TODO
	if (timeplay > 0)
		NativeUtils::submitScore(
				getLeaderBoard(GamePlay::instance()->getLevel()),
				timeplay * 1000);
	if (timeplay > highScore) {
		newhighscore->setVisible(false);
	} else {
		newhighscore->setVisible(true);
	}

//gameover text
	Sprite* gameovertext = Sprite::create("gameovertext.png");
	gameovertext->setPosition(
			Vec2(4 * PADDING + gameovertext->getContentSize().width / 2,
					newhighscore->getPositionY()
							- newhighscore->getContentSize().height / 2
							- gameovertext->getContentSize().height / 2
							- 2 * PADDING));
	gameovertext->setZOrder(40);
	boardWin->addChild(gameovertext);

	float sizewidth = gameovertext->getContentSize().width;
	float sizeheight = gameovertext->getContentSize().height;
//show score
	LabelTTF *currentscore = LabelTTF::create("00:00", "fonts/hlvgchac.ttf",
			40);
	currentscore->setZOrder(50);
	currentscore->setColor(Color3B(Color3B::BLUE));
	showTextTime(currentscore, timeplay);
	currentscore->setPositionX(
			sizewidth + PADDING + currentscore->getContentSize().width / 2);
	currentscore->setPositionY(sizeheight / 2);
	gameovertext->addChild(currentscore);

	LabelTTF *highscore = LabelTTF::create("00:00", "fonts/hlvgchac.ttf", 40);
	highscore->setZOrder(50);
	highscore->setColor(Color3B(Color3B::BLUE));
	showTextTime(highscore, highScore);
	highscore->setPositionX(
			sizewidth + PADDING + currentscore->getContentSize().width / 2);
	highscore->setPositionY(highscore->getContentSize().height / 2);
	gameovertext->addChild(highscore);

	std::string miniModeSprite = getLevelString(
			GamePlay::instance()->getLevel());
	Sprite* miniMode = Sprite::create("mini" + miniModeSprite + ".png");
	miniMode->setPosition(
			Vec2(
					haflWidth * 2 - miniMode->getContentSize().width / 2
							- 3 * PADDING,
					gameovertext->getPositionY()
							+ miniMode->getContentSize().height / 2
							+ 2 * PADDING));
	boardWin->addChild(miniMode);
//button ok
	MenuItemImage* okBtn = MenuItemImage::create("okBtn.png", "okBtn.png",
			CC_CALLBACK_0(GamePlay::gameCloseWinBoard, this));
	auto okMenu = Menu::create(okBtn, nullptr);
	okMenu->setZOrder(40);
	okMenu->setPosition(
			Vec2(haflWidth / 2 + PADDING,
					gameovertext->getPositionY()
							- gameovertext->getContentSize().height / 2
							- okBtn->getContentSize().height / 2
							- 2 * PADDING));
	boardWin->addChild(okMenu);
//button share
	MenuItemImage* shareBtn = MenuItemImage::create("shareBtn.png",
			"shareBtn.png", CC_CALLBACK_0(GamePlay::gameShareWin, this));
	auto shareMenu = Menu::create(shareBtn, nullptr);
	shareMenu->setZOrder(40);
	shareMenu->setPosition(
			Vec2(haflWidth * 3 / 2 - PADDING, okMenu->getPositionY()));
	boardWin->addChild(shareMenu);
}

void GamePlay::update(float delta) {
	if (gameState == Playing) {
		timeplay += delta;
		showTime(timeplay);
	}
	setAllScore(framePlay, tempResult, arrCOMPUTATE);
	for (int i = 0; i < SUM_COM / 2; i++) {
		if (tempResult[i] == result[i]) {
			resultLabel[i]->setColor(Color3B(Color3B::BLUE));
		} else {
			resultLabel[i]->setColor(Color3B(Color3B::RED));
		}
	}

	//TODO
	if (
//			checkAllInFramePlay() == true&&
	checkCalculate(result, tempResult) == true) {
		checkWin = true;
		// show game over
		this->unscheduleUpdate();
		this->scheduleOnce(schedule_selector(GamePlay::gameOver), 1.0);
	}
}

bool GamePlay::checkAllInFramePlay() {
	bool check = false;
	for (int i = 0; i < SUM_NUM; i++) {
		TouchSprite* test = (TouchSprite*) this->getChildByTag(TAG_NUMBER + i);
		if (test->posframe != NULL && test->isTouch == false)
			check = true;
		else {
			check = false;
			break;
		}
	}
	return check;
}

bool GamePlay::checkCalculate(int result[], int tempResult[]) {
	bool check;
	for (int i = 0; i < SUM_COM / 2; i++) {
		if (tempResult[i] == result[i]) {
			check = true;
		} else {
			check = false;
			break;
		}
	}
	return check;
}

void GamePlay::setAllScore(TouchSprite* framePlay[], int tempResult[],
		char* arrCOM) {
	// 0 1 2
	if (framePlay[0]->score == 0 || framePlay[1]->score == 0
			|| framePlay[2]->score == 0)
		tempResult[0] = INIT_FRAME;
	else
		tempResult[0] = gameAI->calculate(framePlay[0]->score,
				framePlay[1]->score, framePlay[2]->score, arrCOM[0], arrCOM[1]);

	// 0 3 6
	if (framePlay[0]->score == 0 || framePlay[3]->score == 0
			|| framePlay[6]->score == 0)
		tempResult[1] = INIT_FRAME;
	else
		tempResult[1] = gameAI->calculate(framePlay[0]->score,
				framePlay[3]->score, framePlay[6]->score, arrCOM[2], arrCOM[7]);

	// 3 4 5
	if (framePlay[3]->score == 0 || framePlay[4]->score == 0
			|| framePlay[5]->score == 0)
		tempResult[2] = INIT_FRAME;
	else
		tempResult[2] = gameAI->calculate(framePlay[3]->score,
				framePlay[4]->score, framePlay[5]->score, arrCOM[5], arrCOM[6]);

	//1 4 7
	if (framePlay[1]->score == 0 || framePlay[4]->score == 0
			|| framePlay[7]->score == 0)
		tempResult[3] = INIT_FRAME;
	else
		tempResult[3] = gameAI->calculate(framePlay[1]->score,
				framePlay[4]->score, framePlay[7]->score, arrCOM[3], arrCOM[8]);

	//6 7 8
	if (framePlay[6]->score == 0 || framePlay[7]->score == 0
			|| framePlay[8]->score == 0)
		tempResult[4] = INIT_FRAME;
	else
		tempResult[4] = gameAI->calculate(framePlay[6]->score,
				framePlay[7]->score, framePlay[8]->score, arrCOM[10],
				arrCOM[11]);

	//2 5 8
	if (framePlay[2]->score == 0 || framePlay[5]->score == 0
			|| framePlay[8]->score == 0)
		tempResult[5] = INIT_FRAME;
	else
		tempResult[5] = gameAI->calculate(framePlay[2]->score,
				framePlay[5]->score, framePlay[8]->score, arrCOM[4], arrCOM[9]);
}

void GamePlay::setPosCharacterBtn(Sprite* comBtn[], TouchSprite* framePlay[],
		float paddingX) {
	comBtn[0]->setPositionX(framePlay[0]->getPositionX() + paddingX / 2);
	comBtn[1]->setPositionX(framePlay[1]->getPositionX() + paddingX / 2);
	comBtn[0]->setPositionY(framePlay[0]->getPositionY());
	comBtn[1]->setPositionY(framePlay[0]->getPositionY());

	comBtn[2]->setPositionY(framePlay[0]->getPositionY() - paddingX / 2);
	comBtn[3]->setPositionY(framePlay[0]->getPositionY() - paddingX / 2);
	comBtn[4]->setPositionY(framePlay[0]->getPositionY() - paddingX / 2);
	comBtn[2]->setPositionX(framePlay[0]->getPositionX());
	comBtn[3]->setPositionX(framePlay[1]->getPositionX());
	comBtn[4]->setPositionX(framePlay[2]->getPositionX());

	comBtn[5]->setPositionX(framePlay[0]->getPositionX() + paddingX / 2);
	comBtn[6]->setPositionX(framePlay[1]->getPositionX() + paddingX / 2);
	comBtn[5]->setPositionY(framePlay[3]->getPositionY());
	comBtn[6]->setPositionY(framePlay[3]->getPositionY());

	comBtn[7]->setPositionY(framePlay[3]->getPositionY() - paddingX / 2);
	comBtn[8]->setPositionY(framePlay[3]->getPositionY() - paddingX / 2);
	comBtn[9]->setPositionY(framePlay[3]->getPositionY() - paddingX / 2);
	comBtn[7]->setPositionX(framePlay[0]->getPositionX());
	comBtn[8]->setPositionX(framePlay[1]->getPositionX());
	comBtn[9]->setPositionX(framePlay[2]->getPositionX());

	comBtn[10]->setPositionX(framePlay[0]->getPositionX() + paddingX / 2);
	comBtn[11]->setPositionX(framePlay[1]->getPositionX() + paddingX / 2);
	comBtn[10]->setPositionY(framePlay[SUM_NUM - 1]->getPositionY());
	comBtn[11]->setPositionY(framePlay[SUM_NUM - 1]->getPositionY());
}

void GamePlay::onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event) {
	if (keyCode == EventKeyboard::KeyCode::KEY_ESCAPE) {
		setSoundMusic();
		auto gameScene = GameScene::createScene();
		auto transition = TransitionFade::create(1.0, gameScene);
		Director::getInstance()->replaceScene(transition);
	}
}
