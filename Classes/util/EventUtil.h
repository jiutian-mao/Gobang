#ifndef __EVENT_UTIL__
#define __EVENT_UTIL__
#include "cocos2d.h"
USING_NS_CC;

class EventUtil
{

public:
	//����Ƿ�����Ŀ�����
	static bool touchHitRef(Touch* touch,Event* event);

private:
	EventUtil(){};
	~EventUtil(){};

};

#endif //__EVENT_UTIL__
