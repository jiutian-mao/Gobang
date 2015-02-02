#ifndef __EVENT_UTIL__
#define __EVENT_UTIL__
#include "cocos2d.h"
USING_NS_CC;

class EventUtil
{

public:
	//检测是否触摸到目标对象
	static bool touchHitRef(Touch* touch,Event* event);

private:
	EventUtil(){};
	~EventUtil(){};

};

#endif //__EVENT_UTIL__
