#include "util/EventUtil.h"

bool EventUtil::touchHitRef(Touch* touch,Event* event)
{
	Node* target = event->getCurrentTarget();
	const Size& s = target->getContentSize();
	Rect rect(0,0,s.width,s.height);
	const Point& p = target->convertTouchToNodeSpace(touch);
	if(rect.containsPoint(p)){
		return true;
	}
	return false;
}
