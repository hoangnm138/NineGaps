#ifndef __TOUCHSPRITE_H__
#define __TOUCHSPRITE_H__
#include "cocos2d.h" 

USING_NS_CC;

class TouchSprite: public cocos2d::Sprite
{
public:
	 TouchSprite(void);
    ~TouchSprite(void);
	static TouchSprite* create(const std::string& filename);
	bool initWithFile(const std::string& filename);
	TouchSprite* createWithTexture(Texture2D *texture);
	bool isTouchingSprite(Touch* touch);
	bool isTouch;
	int score;
	TouchSprite* posframe;
	bool isHidden;
	bool isFixed;
};

#endif
