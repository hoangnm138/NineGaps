#include "GameScene.h"
#include "GamePlay.h"
#include "AdmobHelper.h"
#include "NativeUtils.h"
#include "PlayGameConstants.h"

USING_NS_CC;

Scene* GameScene::createScene() {
	// 'scene' is an autorelease object
	auto scene = Scene::create();

	// 'layer' is an autorelease object
	auto layer = GameScene::create();

	// add layer as a child to scene
	scene->addChild(layer);

	// return the scene
	return scene;
}

// on "init" you need to initialize your instance
bool GameScene::init() {
	//////////////////////////////
	// 1. super init first
	if (!Layer::init()) {
		return false;
	}

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	// create background
	auto bg = Sprite::create("background.png");
	bg->setAnchorPoint(Vec2(0, 0));
	bg->setPosition(Vec2(0, 0));
	this->addChild(bg, -1);
	float rX = visibleSize.width / bg->getContentSize().width;
	float rY = visibleSize.height / bg->getContentSize().height;
	bg->setScaleX(rX);
	bg->setScaleY(rY);

	//show logo game
	Sprite *logoGame = Sprite::create("gamelogo.png");
	logoGame->setPosition(
			Vec2(visibleSize.width / 2,
					visibleSize.height - logoGame->getContentSize().height / 2
							- PADDING));
	logoGame->setTag(TAG_LOGO);
	this->addChild(logoGame);

	homeButton = Node::create();
	this->addChild(homeButton);

	//create button
	//button play
	Sprite* playbtn = Sprite::create("play1.png");
	Sprite* playbtn_press = Sprite::create("play2.png");
	MenuItemSprite* playBtn = MenuItemSprite::create(playbtn, playbtn_press,
	NULL, CC_CALLBACK_0(GameScene::gameSelectLevel, this));
	Menu *menuStart = Menu::create(playBtn, nullptr);
	menuStart->setPosition(
			Vec2(visibleSize.width / 2,
					logoGame->getPosition().y
							- logoGame->getContentSize().height - PADDING));
	menuStart->setTag(TAG_PLAYBTN);
	homeButton->addChild(menuStart);

	//button setting
	Sprite* setbtn = Sprite::create("settings1.png");
	Sprite* setbtn_press = Sprite::create("settings2.png");
	MenuItemSprite* setBtn = MenuItemSprite::create(setbtn, setbtn_press, NULL,
			CC_CALLBACK_1(GameScene::gameSetting, this));
	Menu *menuSetting = Menu::create(setBtn, nullptr);
	menuSetting->setPosition(
			Vec2(visibleSize.width / 2,
					menuStart->getPosition().y
							- playbtn->getContentSize().height - PADDING));
	homeButton->addChild(menuSetting);

	//button highscore
	Sprite* highscorebtn = Sprite::create("highscore1.png");
	Sprite* highscorebtn_press = Sprite::create("highscore2.png");
	MenuItemSprite* highscoreBtn = MenuItemSprite::create(highscorebtn,
			highscorebtn_press, NULL,
			CC_CALLBACK_1(GameScene::gameHighScore, this));
	Menu *menuHS = Menu::create(highscoreBtn, nullptr);
	menuHS->setPosition(
			Vec2(visibleSize.width / 2,
					menuSetting->getPosition().y
							- playbtn->getContentSize().height - PADDING));
	menuHS->setTag(TAG_HIGHSCORE);
	homeButton->addChild(menuHS);

	//button help
	Sprite* helpbtn = Sprite::create("help1.png");
	Sprite* helpbtn_press = Sprite::create("help2.png");
	MenuItemSprite* helpBtn = MenuItemSprite::create(helpbtn, helpbtn_press,
	NULL, CC_CALLBACK_1(GameScene::gameHelp, this));
	Menu *menuHelp = Menu::create(helpBtn, nullptr);
	menuHelp->setPosition(
			Vec2(visibleSize.width / 2,
					menuHS->getPosition().y - playbtn->getContentSize().height
							- PADDING));
	homeButton->addChild(menuHelp);

	MenuItemImage* hallBtn = MenuItemImage::create("hallofframe1.png",
			"hallofframe2.png",
			CC_CALLBACK_0(GameScene::gameHallOfFrame, this));
	Menu *menuHall = Menu::create(hallBtn, nullptr);
	menuHall->setPosition(
			Vec2(visibleSize.width / 2,
					menuHelp->getPosition().y - playbtn->getContentSize().height
							- PADDING));
	homeButton->addChild(menuHall);

	this->setKeypadEnabled(true);
	//preload music
	getSoundMusic();
	preloadMusic();
	if (CocosDenshion::SimpleAudioEngine::getInstance()->isBackgroundMusicPlaying()
			== false)
		CocosDenshion::SimpleAudioEngine::getInstance()->playBackgroundMusic(
				"sound/music.mp3", true);
	else
		CocosDenshion::SimpleAudioEngine::getInstance()->resumeBackgroundMusic();
	if (onMusic == false)
		CocosDenshion::SimpleAudioEngine::getInstance()->pauseBackgroundMusic();

	//board menu
	boardMenu = Sprite::create("board.png");
	boardMenu->setPosition(
			Vec2(visibleSize.width / 2, visibleSize.height / 2 - 4 * PADDING));
	boardMenu->setZOrder(30);
	boardMenu->setVisible(false);
	this->addChild(boardMenu);

	AdmobHelper::showAd();
	return true;
}

void GameScene::gameHallOfFrame() {
	if (onSound)
			CocosDenshion::SimpleAudioEngine::getInstance()->playEffect(
					"sound/button.mp3");
	NativeUtils::showLeaderboards();
}

void GameScene::gameStart(Object *pSender, int level) {
	if (onSound)
		CocosDenshion::SimpleAudioEngine::getInstance()->playEffect(
				"sound/chooseLevel.mp3");
	setSoundMusic();
	GamePlay::instance()->setLevel(level);

	auto gamePlay = GamePlay::createScene();
	auto transition = TransitionFade::create(1.0, gamePlay);
	Director::getInstance()->replaceScene(transition);
}

void GameScene::gameSelectLevel() {
	if (onSound)
		CocosDenshion::SimpleAudioEngine::getInstance()->playEffect(
				"sound/button.mp3");
	homeButton->setVisible(false);
	boardMenu->setVisible(true);
	boardMenu->removeAllChildrenWithCleanup(true);

	Node* selectLevelNode = Node::create();

	float haflHeight = boardMenu->getContentSize().height / 2;
	float haflWidth = boardMenu->getContentSize().width / 2;

	//button close
	MenuItemImage* closeBtn = MenuItemImage::create("menu_closeBtn.png",
			"menu_closeBtn.png", CC_CALLBACK_0(GameScene::gameCloseMenu, this));
	auto menuClose = Menu::create(closeBtn, nullptr);
	menuClose->setZOrder(40);
	menuClose->setPosition(
			Vec2(haflWidth * 2 - closeBtn->getContentSize().width - PADDING,
					haflHeight * 2 - closeBtn->getContentSize().height
							- PADDING));
	selectLevelNode->addChild(menuClose);

	MenuItemImage* noviceBtn = MenuItemImage::create("Novice.png",
			"Novice_press.png", CC_CALLBACK_1(GameScene::gameStart, this, 4));
	auto menuNovice = Menu::create(noviceBtn, nullptr);
	menuNovice->setZOrder(40);
	menuNovice->setPosition(
			Vec2(haflWidth,
					menuClose->getPositionY()
							- closeBtn->getContentSize().height - PADDING));
	selectLevelNode->addChild(menuNovice);

	MenuItemImage* mediumBtn = MenuItemImage::create("Medium.png",
			"Medium_press.png", CC_CALLBACK_1(GameScene::gameStart, this, 3));
	auto menuMedium = Menu::create(mediumBtn, nullptr);
	menuMedium->setZOrder(40);
	menuMedium->setPosition(
			Vec2(haflWidth,
					menuNovice->getPositionY()
							- noviceBtn->getContentSize().height - PADDING));
	selectLevelNode->addChild(menuMedium);

	MenuItemImage* advancedBtn = MenuItemImage::create("Advanced.png",
			"Advanced_press.png", CC_CALLBACK_1(GameScene::gameStart, this, 2));
	auto menuAdvanced = Menu::create(advancedBtn, nullptr);
	menuAdvanced->setZOrder(40);
	menuAdvanced->setPosition(
			Vec2(haflWidth,
					menuMedium->getPositionY()
							- advancedBtn->getContentSize().height - PADDING));
	selectLevelNode->addChild(menuAdvanced);

	MenuItemImage* expertBtn = MenuItemImage::create("Expert.png",
			"Expert_press.png", CC_CALLBACK_1(GameScene::gameStart, this, 1));
	auto menuExpert = Menu::create(expertBtn, nullptr);
	menuExpert->setZOrder(40);
	menuExpert->setPosition(
			Vec2(haflWidth,
					menuAdvanced->getPositionY()
							- expertBtn->getContentSize().height - PADDING));
	selectLevelNode->addChild(menuExpert);

	MenuItemImage* masterBtn = MenuItemImage::create("Master.png",
			"Master_press.png", CC_CALLBACK_1(GameScene::gameStart, this, 0));
	auto menuMaster = Menu::create(masterBtn, nullptr);
	menuMaster->setZOrder(40);
	menuMaster->setPosition(
			Vec2(haflWidth,
					menuExpert->getPositionY()
							- masterBtn->getContentSize().height - PADDING));
	selectLevelNode->addChild(menuMaster);

	boardMenu->addChild(selectLevelNode);
}

void GameScene::gameSetting(Object *pSender) {
	if (onSound)
		CocosDenshion::SimpleAudioEngine::getInstance()->playEffect(
				"sound/button.mp3");
	homeButton->setVisible(false);
	boardMenu->setVisible(true);
	boardMenu->removeAllChildrenWithCleanup(true);

	Node* settingNode = Node::create();
	settingNode->setTag(TAG_SETTING);

	float haflHeight = boardMenu->getContentSize().height / 2;
	float haflWidth = boardMenu->getContentSize().width / 2;
	MenuItemImage* closeBtn = MenuItemImage::create("menu_closeBtn.png",
			"menu_closeBtn.png", CC_CALLBACK_0(GameScene::gameCloseMenu, this));
	auto menuClose = Menu::create(closeBtn, nullptr);
	menuClose->setZOrder(40);
	menuClose->setPosition(
			Vec2(haflWidth * 2 - closeBtn->getContentSize().width - PADDING,
					haflHeight * 2 - closeBtn->getContentSize().height
							- PADDING));
	settingNode->addChild(menuClose);

	//image sound
	Sprite* sound = Sprite::create("sound.png");
	sound->setPosition(
			Vec2(4 * PADDING + sound->getContentSize().width / 2,
					menuClose->getPositionY()
							- closeBtn->getContentSize().height - 2 * PADDING));
	sound->setZOrder(40);
	settingNode->addChild(sound);

	//button on/of sound
	MenuItemImage* onSoundBtn = MenuItemImage::create("onbutton.png",
			"onbutton.png", CC_CALLBACK_0(GameScene::gameOnSound, this));
	auto menuOnSound = Menu::create(onSoundBtn, nullptr);
	menuOnSound->setZOrder(40);
	menuOnSound->setPosition(
			Vec2(haflWidth / 2 + PADDING,
					sound->getPositionY() - 2 * PADDING
							- onSoundBtn->getContentSize().height));
	settingNode->addChild(menuOnSound);

	MenuItemImage* offSoundBtn = MenuItemImage::create("offbutton.png",
			"offbutton.png", CC_CALLBACK_0(GameScene::gameOffSound, this));
	auto menuOffSound = Menu::create(offSoundBtn, nullptr);
	menuOffSound->setZOrder(40);
	menuOffSound->setPosition(
			Vec2(haflWidth * 3 / 2 - PADDING, menuOnSound->getPositionY()));
	settingNode->addChild(menuOffSound);

	//stick sound
	stickSound = Sprite::create("stick.png");
	stickSound->setZOrder(50);
	stickSound->setTag(TAG_STICK_SOUND);
	stickSound->setPositionY(menuOnSound->getPositionY());
	settingNode->addChild(stickSound);
	this->posOnX = menuOnSound->getPositionX()
			- onSoundBtn->getContentSize().width / 4 - PADDING / 2;
	this->posOffX = menuOffSound->getPositionX()
			- offSoundBtn->getContentSize().width / 4 - PADDING / 2;
	if (onSound == true) {
		stickSound->setPositionX(this->posOnX);
	} else {
		stickSound->setPositionX(this->posOffX);
	}

	//image music
	Sprite* music = Sprite::create("music.png");
	music->setZOrder(40);
	music->setPosition(
			Vec2(sound->getPositionX(),
					sound->getPositionY()
							- 3 * music->getContentSize().height));
	settingNode->addChild(music);

	//button on/of music
	MenuItemImage* onMusicBtn = MenuItemImage::create("onbutton.png",
			"onbutton.png", CC_CALLBACK_0(GameScene::gameOnMusic, this));
	auto menuOnMusic = Menu::create(onMusicBtn, nullptr);
	menuOnMusic->setZOrder(40);
	menuOnMusic->setPosition(
			Vec2(menuOnSound->getPositionX(),
					music->getPositionY() - 2 * PADDING
							- onMusicBtn->getContentSize().height));
	settingNode->addChild(menuOnMusic);

	MenuItemImage* offMusicBtn = MenuItemImage::create("offbutton.png",
			"offbutton.png", CC_CALLBACK_0(GameScene::gameOffMusic, this));
	auto menuOffMusic = Menu::create(offMusicBtn, nullptr);
	menuOffMusic->setZOrder(40);
	menuOffMusic->setPosition(
			Vec2(haflWidth * 3 / 2 - PADDING, menuOnMusic->getPositionY()));
	settingNode->addChild(menuOffMusic);

	//stick music
	stickMusic = Sprite::create("stick.png");
	stickMusic->setZOrder(50);
	stickMusic->setTag(TAG_STICK_MUSIC);
	stickMusic->setPositionY(menuOnMusic->getPositionY());
	settingNode->addChild(stickMusic);
	if (onMusic == true) {
		stickMusic->setPositionX(this->posOnX);
	} else {
		stickMusic->setPositionX(this->posOffX);
	}

	boardMenu->addChild(settingNode);
}

void GameScene::showTextTime(LabelTTF *showtext, float time) {
	int seconds = (int) time % 60;
	int minutes = (int) (time / 60) % 60;
	int hours = (int) time / 3600;
	String* timeString = String::createWithFormat("%02d:%02d", minutes,
			seconds);
	showtext->setString(timeString->_string.c_str());
}

void GameScene::gameHighScore(Object *pSender) {
	if (onSound)
		CocosDenshion::SimpleAudioEngine::getInstance()->playEffect(
				"sound/button.mp3");
	homeButton->setVisible(false);
	boardMenu->setVisible(true);
	boardMenu->removeAllChildrenWithCleanup(true);

	Node* highScoreNode = Node::create();
	float haflHeight = boardMenu->getContentSize().height / 2;
	float haflWidth = boardMenu->getContentSize().width / 2;

	MenuItemImage* closeBtn = MenuItemImage::create("menu_closeBtn.png",
			"menu_closeBtn.png", CC_CALLBACK_0(GameScene::gameCloseMenu, this));
	auto menuClose = Menu::create(closeBtn, nullptr);
	menuClose->setZOrder(40);
	menuClose->setPosition(
			Vec2(haflWidth * 2 - closeBtn->getContentSize().width - PADDING,
					haflHeight * 2 - closeBtn->getContentSize().height
							- PADDING));
	highScoreNode->addChild(menuClose);

	Sprite* highscoreSprite = Sprite::create("highscore.png");
	highscoreSprite->setPosition(
			Vec2(PADDING * 4 + highscoreSprite->getContentSize().width / 2,
					menuClose->getPositionY()));
	highScoreNode->addChild(highscoreSprite);

	float tempX = highscoreSprite->getPositionX()
			- highscoreSprite->getContentSize().width / 2;

	Sprite* noviceImg = Sprite::create("miniNovice.png");
	noviceImg->setPositionX(tempX + noviceImg->getContentSize().width / 2);
	noviceImg->setPositionY(
			highscoreSprite->getPositionY()
					- highscoreSprite->getContentSize().height - 2 * PADDING);
	highScoreNode->addChild(noviceImg);

	Sprite* mediumImg = Sprite::create("miniMedium.png");
	mediumImg->setPositionX(tempX + mediumImg->getContentSize().width / 2);
	mediumImg->setPositionY(
			noviceImg->getPositionY() - noviceImg->getContentSize().height
					- PADDING);
	highScoreNode->addChild(mediumImg);

	Sprite* advancedImg = Sprite::create("miniAdvanced.png");
	advancedImg->setPositionX(tempX + advancedImg->getContentSize().width / 2);
	advancedImg->setPositionY(
			mediumImg->getPositionY() - mediumImg->getContentSize().height
					- PADDING);
	highScoreNode->addChild(advancedImg);

	Sprite* expertImg = Sprite::create("miniExpert.png");
	expertImg->setPositionX(tempX + expertImg->getContentSize().width / 2);
	expertImg->setPositionY(
			advancedImg->getPositionY() - advancedImg->getContentSize().height
					- PADDING);
	highScoreNode->addChild(expertImg);

	Sprite* masterImg = Sprite::create("miniMaster.png");
	masterImg->setPositionX(tempX + masterImg->getContentSize().width / 2);
	masterImg->setPositionY(
			expertImg->getPositionY() - expertImg->getContentSize().height
					- PADDING);
	highScoreNode->addChild(masterImg);

	//show score
	LabelTTF *novicescore = LabelTTF::create("00:00", "fonts/hlvgchac.ttf", 40);
	novicescore->setZOrder(50);
	novicescore->setColor(Color3B(Color3B::BLUE));
	showTextTime(novicescore,
			UserDefault::sharedUserDefault()->getFloatForKey("Novice"));
	novicescore->setPositionX(
			haflWidth * 2 - PADDING - novicescore->getContentSize().width);
	novicescore->setPositionY(noviceImg->getPositionY());
	highScoreNode->addChild(novicescore);

	LabelTTF *mediumscore = LabelTTF::create("00:00", "fonts/hlvgchac.ttf", 40);
	mediumscore->setZOrder(50);
	mediumscore->setColor(Color3B(Color3B::BLUE));
	showTextTime(mediumscore,
			UserDefault::sharedUserDefault()->getFloatForKey("Medium"));
	mediumscore->setPositionX(novicescore->getPositionX());
	mediumscore->setPositionY(mediumImg->getPositionY());
	highScoreNode->addChild(mediumscore);

	LabelTTF *advancedscore = LabelTTF::create("00:00", "fonts/hlvgchac.ttf",
			40);
	advancedscore->setZOrder(50);
	advancedscore->setColor(Color3B(Color3B::BLUE));
	showTextTime(advancedscore,
			UserDefault::sharedUserDefault()->getFloatForKey("Advanced"));
	advancedscore->setPositionX(novicescore->getPositionX());
	advancedscore->setPositionY(advancedImg->getPositionY());
	highScoreNode->addChild(advancedscore);

	LabelTTF *expertscore = LabelTTF::create("00:00", "fonts/hlvgchac.ttf", 40);
	expertscore->setZOrder(50);
	expertscore->setColor(Color3B(Color3B::BLUE));
	showTextTime(expertscore,
			UserDefault::sharedUserDefault()->getFloatForKey("Expert"));
	expertscore->setPositionX(novicescore->getPositionX());
	expertscore->setPositionY(expertImg->getPositionY());
	highScoreNode->addChild(expertscore);

	LabelTTF *masterscore = LabelTTF::create("00:00", "fonts/hlvgchac.ttf", 40);
	masterscore->setZOrder(50);
	masterscore->setColor(Color3B(Color3B::BLUE));
	showTextTime(masterscore,
			UserDefault::sharedUserDefault()->getFloatForKey("Master"));
	masterscore->setPositionX(novicescore->getPositionX());
	masterscore->setPositionY(masterImg->getPositionY());
	highScoreNode->addChild(masterscore);

	boardMenu->addChild(highScoreNode);
}

void GameScene::gameHelp(Object *pSender) {
	if (onSound)
		CocosDenshion::SimpleAudioEngine::getInstance()->playEffect(
				"sound/button.mp3");
	homeButton->setVisible(false);
	boardMenu->setVisible(true);
	boardMenu->removeAllChildrenWithCleanup(true);

	Node* helpNode = Node::create();
	helpNode->setTag(TAG_HELP);

	float haflHeight = boardMenu->getContentSize().height / 2;
	float haflWidth = boardMenu->getContentSize().width / 2;

	Sprite* text = Sprite::create("howtoplaytext.png");
	text->setPosition(Vec2(haflWidth + PADDING / 2, haflHeight));
	text->setZOrder(40);
	helpNode->addChild(text);

	MenuItemImage* closeBtn = MenuItemImage::create("menu_closeBtn.png",
			"menu_closeBtn.png", CC_CALLBACK_0(GameScene::gameCloseMenu, this));
	auto menuClose = Menu::create(closeBtn, nullptr);
	menuClose->setZOrder(40);
	menuClose->setPosition(
			Vec2(haflWidth * 2 - closeBtn->getContentSize().width - PADDING,
					haflHeight * 2 - closeBtn->getContentSize().height
							- PADDING));
	helpNode->addChild(menuClose);

	boardMenu->addChild(helpNode);
}

void GameScene::gameCloseMenu() {
	if (onSound)
		CocosDenshion::SimpleAudioEngine::getInstance()->playEffect(
				"sound/button.mp3");
	boardMenu->removeAllChildrenWithCleanup(true);
	boardMenu->setVisible(false);
	homeButton->setVisible(true);
}

//load music game
void GameScene::preloadMusic() {
	CocosDenshion::SimpleAudioEngine::getInstance()->preloadEffect(
			"sound/music_win.ogg");
	CocosDenshion::SimpleAudioEngine::getInstance()->preloadEffect(
			"sound/tapNumber.mp3");
	CocosDenshion::SimpleAudioEngine::getInstance()->preloadEffect(
			"sound/button.mp3");
	CocosDenshion::SimpleAudioEngine::getInstance()->preloadBackgroundMusic(
			"sound/music.mp3");
	CocosDenshion::SimpleAudioEngine::getInstance()->preloadEffect(
			"sound/chooseLevel.mp3");
}

void GameScene::setSoundMusic() {
	UserDefault::sharedUserDefault()->setBoolForKey("onMusic", onMusic);
	UserDefault::sharedUserDefault()->flush();

	UserDefault::sharedUserDefault()->setBoolForKey("onSound", onSound);
	UserDefault::sharedUserDefault()->flush();
}

void GameScene::getSoundMusic() {
	onMusic = UserDefault::sharedUserDefault()->getBoolForKey("onMusic", true);
	onSound = UserDefault::sharedUserDefault()->getBoolForKey("onSound", true);
}

void GameScene::gameOnSound() {
	if (onSound == false) {
		onSound = true;
		stickSound->setPositionX(posOnX);
	}
}

void GameScene::gameOffSound() {
	if (onSound == true) {
		onSound = false;
		stickSound->setPositionX(posOffX);
	}
}

void GameScene::gameOnMusic() {
	if (onMusic == false) {
		onMusic = true;
		stickMusic->setPositionX(posOnX);
		if (CocosDenshion::SimpleAudioEngine::getInstance()->isBackgroundMusicPlaying()
				== false)
			CocosDenshion::SimpleAudioEngine::getInstance()->resumeBackgroundMusic();
	}
}

void GameScene::gameOffMusic() {
	if (onMusic == true) {
		onMusic = false;
		stickMusic->setPositionX(posOffX);
		CocosDenshion::SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
	}
}

//quit app
void GameScene::onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event) {
	if (keyCode == EventKeyboard::KeyCode::KEY_ESCAPE) {
		setSoundMusic();
		Director::getInstance()->end();
	}
}
