#include "AppDelegate.h"
#include "GameScene.h"

USING_NS_CC;

typedef struct tagResource {
	cocos2d::CCSize size;
	char directory[100];
} Resolution;
static cocos2d::CCSize designResolutionSize = cocos2d::CCSizeMake(720, 1280);
AppDelegate::AppDelegate() {

}

AppDelegate::~AppDelegate() {
}

bool AppDelegate::applicationDidFinishLaunching() {
	// initialize director
	auto director = Director::getInstance();
	auto glview = director->getOpenGLView();
	if (!glview) {
		glview = GLView::create("Nine Gaps");
		director->setOpenGLView(glview);
	}

	// set frame for window
	//glview->setFrameSize(smallResource.size.width, smallResource.size.height);
	//set display
	glview->setDesignResolutionSize(designResolutionSize.width,
			designResolutionSize.height, ResolutionPolicy::EXACT_FIT);
	// turn on display FPS
//	director->setDisplayStats(true);

	// set FPS. the default value is 1.0/60 if you don't call this
	director->setAnimationInterval(1.0 / 60);

	// create a scene. it's an autorelease object
	Scene* scene = GameScene::createScene();

	// run
	director->runWithScene(scene);

	return true;
}

// This function will be called when the app is inactive. When comes a phone call,it's be invoked too
void AppDelegate::applicationDidEnterBackground() {
	if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
		Director::getInstance()->pause();
	else {
		Director::getInstance()->pause();
		Director::getInstance()->stopAnimation();
	}
	// if you use SimpleAudioEngine, it must be pause
	// SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground() {
	if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
		Director::getInstance()->resume();
	else {
		Director::getInstance()->resume();
		Director::getInstance()->startAnimation();
	}

	// if you use SimpleAudioEngine, it must resume here
	// SimpleAudioEngine::getInstance()->resumeBackgroundMusic();
}
