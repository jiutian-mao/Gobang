#ifndef __BASEWIDGET_H_
#define __BASEWIDGET_H_

#include "stdafx.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CCSGUIReader.h"
using namespace cocostudio;
using namespace ui;

class BaseWidget : public Ref{
public:
	//get and set
	CC_SYNTHESIZE(Widget*,widget,Widget);
public:
	BaseWidget();
	virtual ~BaseWidget();
};

#endif /* __BASEWIDGET_H_ */
