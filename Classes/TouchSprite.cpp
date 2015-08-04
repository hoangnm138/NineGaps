#include "TouchSprite.h"
USING_NS_CC;

TouchSprite::TouchSprite(void)
{
	isTouch = false;
	score = 0;
	posframe = NULL;
}

TouchSprite::~TouchSprite(void)
{
}

bool TouchSprite::initWithFile(const std::string& filename)
{
    CCASSERT(filename.size()>0, "Invalid filename for sprite");

    Texture2D *texture = Director::getInstance()->getTextureCache()->addImage(filename);
    if (texture)
    {
        Rect rect = Rect::ZERO;
        rect.size = texture->getContentSize();
        return initWithTexture(texture, rect);
    }
    // don't release here.
    // when load texture failed, it's better to get a "transparent" sprite then a crashed program
    // this->release();
    return false;
}

TouchSprite* TouchSprite::createWithTexture(Texture2D *texture)
{
    TouchSprite *sprite = new TouchSprite();
    if (sprite && sprite->initWithTexture(texture))
    {
        sprite->autorelease();
        return sprite;
    }
    CC_SAFE_DELETE(sprite);
    return nullptr;
}

TouchSprite* TouchSprite::create(const std::string& filename)
{
    TouchSprite *sprite = new TouchSprite();
    if (sprite && sprite->initWithFile(filename))
    {
        sprite->autorelease();
        return sprite;
    }
    CC_SAFE_DELETE(sprite);
    return nullptr;
}

bool TouchSprite::isTouchingSprite(Touch* touch)
{
	if((this->getPositionX()+this->getContentSize().width/2)>=touch->getLocation().x && 
		touch->getLocation().x>=(this->getPositionX()-this->getContentSize().width/2) &&
		(this->getPositionY()+this->getContentSize().height/2)>=touch->getLocation().y && 
		touch->getLocation().y>=(this->getPositionY()-this->getContentSize().height/2))
		return true;
	else return false;
}
