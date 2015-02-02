#include "PopupLayer.h"
 
PopupLayer::PopupLayer():
    _pMenu(NULL)
    , _contentPadding(0)
    , _contentPaddingTop(0)
    , _callbackListener(NULL)
    , _callback(NULL)
    , _BG(NULL)
    , _contentText(NULL)
    , _titlie(NULL)
	, _panel(NULL)
{
 
}
 
PopupLayer::~PopupLayer()
{
	CC_SAFE_RELEASE(_pMenu);
	CC_SAFE_RELEASE(_BG);
	CC_SAFE_RELEASE(_contentText);
	CC_SAFE_RELEASE(_titlie);
}
 
bool PopupLayer::init()
{
	if(!Layer::init())
	{
		return false;
	}

	//触摸事件
	listener = EventListenerTouchOneByOne::create();
	listener->setSwallowTouches(true);
	listener->onTouchBegan = CC_CALLBACK_2(PopupLayer::onTouchBegan, this);
	listener->onTouchEnded = CC_CALLBACK_2(PopupLayer::onTouchEnded, this);

	//返回退出键监听
	EventListenerKeyboard* back = EventListenerKeyboard::create();
	back->onKeyReleased = CC_CALLBACK_2(PopupLayer::onKeyReleased,this);
	Director::getInstance()->getEventDispatcher()->setPriority(back,-1);
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(
			back,this);

	//背景灰色层
	LayerColor* color = LayerColor::create();
	color->setColor(Color3B(0,0,0));
	color->setOpacity(128);
	color->setScale(1.5);
	this->addChild(color);

	_panel = Node::create();
	this->addChild(_panel);

	return true;
}
 

bool PopupLayer::onTouchBegan(Touch *touch,Event *event)
{
	return true;
}

void PopupLayer::onTouchEnded(Touch* touch,Event* event)
{
	if((!EventUtil::touchHitRef(touch,event)))
	{
		//点击内容区域以外，隐藏显示
		this->setTouchEnabled(false);
		hide();
	}
}

PopupLayer* PopupLayer::create(const char* backgroundImage,Resources::TextureResType texType,Size dialogSize)
{
	PopupLayer* layer = PopupLayer::create();
	layer->_dialogContentSize = dialogSize;
	switch(texType)
	{
	case Resources::TextureResType::LOCAL:
		layer->setSpriteBackGround(Sprite::create(backgroundImage));
		break;
	case Resources::TextureResType::PLIST:
		layer->setSpriteBackGround(Sprite::createWithSpriteFrameName(backgroundImage));
		break;
	default:
		break;
	}
	return layer;
}

void PopupLayer::setTitle(const char* title,int fontsize /* = 20 */)
{
	LabelTTF* label = LabelTTF::create(title,"",fontsize);
	setLabelTitle(label);
}
 

void PopupLayer::setContentText(const char *text,int fontsize,int padding,int paddingTop)
{
	Label* ltf = Label::create(text,"",fontsize);
	setLabelContentText(ltf);
	_contentPadding = padding;
	_contentPaddingTop = paddingTop;
}
 
void PopupLayer::setCallbackFunc(Ref* target, SEL_CallFuncN callfun)
{
    _callbackListener = target;
    _callback = callfun;    
}
 

bool PopupLayer::addButton(const char* normalImage,const char* selectedImage,
		const char* title,int tag /* = 0 */)
{

	// 初始化需要的 Menu
	if(!getMenuButton())
	{
		Menu* menu = Menu::create();
		menu->setPosition(CCPointZero);
		setMenuButton(menu);
	}

	auto size = Director::getInstance()->getWinSize();
	auto center = Point(size.width / 2,size.height / 2);

	// 创建图片菜单按钮
	auto item = MenuItemImage::create(normalImage,selectedImage,
			CC_CALLBACK_1(PopupLayer::buttonCallBack,this));
	item->setTag(tag);
	item->setPosition(center);

	// 添加文字说明并设置位置
	Size itemSize = item->getContentSize();
	LabelTTF* ttf = LabelTTF::create(title,"",20);
	ttf->setColor(Color3B(0,0,0));
	ttf->setPosition(Point(itemSize.width / 2,itemSize.height / 2));
	item->addChild(ttf);

	getMenuButton()->addChild(item);

	return true;
}

void PopupLayer::addView(Node* node)
{
	_panel->addChild(node);
	if(getSpriteBackGround() == nullptr)
	{
		setSpriteBackGround(node);
	}
}

//@param margin 居中位置偏移量
void PopupLayer::addView(Node* node,const Point& margin){
	node->setPosition(
			-((1-0.5-node->getAnchorPoint().x)*node->getContentSize().width*node->getScaleX())+margin.x,
			-((1-0.5-node->getAnchorPoint().y)*node->getContentSize().height*node->getScaleY())+margin.y);
	if(getSpriteBackGround() == nullptr)
	{
		setSpriteBackGround(node);
	}
	_panel->addChild(node);
}
 
void PopupLayer::buttonCallBack(Ref* pSender)
{
	Node* node = dynamic_cast<Node*>(pSender);
	if(_callback && _callbackListener)
	{
		(_callbackListener->*_callback)(node);
	}
	this->removeFromParent();
}
 
void PopupLayer::onEnter()
{
	Layer::onEnter();
 
    Size winSize = Director::getInstance()->getWinSize();
    Point pCenter = Point(winSize.width / 2, winSize.height / 2);
    _panel->setPosition(ccp(winSize.width / 2, winSize.height / 2));

    //设置背景
	if(getSpriteBackGround() && getContentSize().equals(this->getParent()->getContentSize()))
	{
		//getSpriteBackGround()->setPosition(ccp(winSize.width / 2, winSize.height / 2));
		_panel->addChild(getSpriteBackGround(),-1,0);
		Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(
				listener,getSpriteBackGround());
	}

	//添加按钮
	if(getMenuButton())
	{
		_panel->addChild(getMenuButton());
		float btnWidth = _dialogContentSize.width / (getMenuButton()->getChildrenCount() + 1);
		Vector<Node*> vector = getMenuButton()->getChildren();
		Ref* pObj = NULL;
		int i = 0;
		for(Node* pObj : vector)
		{
			Node* node = dynamic_cast<Node*>(pObj);
			node->setPosition(
					Point(-_dialogContentSize.width / 2 + btnWidth * (i + 1),
							-_dialogContentSize.height / 4));
			i++;
		}
	}
	
	// 显示对话框标题
	if(getLabelTitle())
	{
		getLabelTitle()->setPosition(ccpAdd(pCenter,ccp(0,_dialogContentSize.height / 2 - 35.0f)));
		_panel->addChild(getLabelTitle());
	}

	//显示文本内容
	if(getLabelContentText())
	{
		Label* ltf = getLabelContentText();
//		ltf->setPosition(ccp(winSize.width / 2, winSize.height / 2));
		ltf->setDimensions(_dialogContentSize.width - _contentPadding * 2,
				_dialogContentSize.height - _contentPaddingTop);
		ltf->setHorizontalAlignment(kCCTextAlignmentCenter);
		_panel->addChild(ltf);
	}

    //内容区域抖动效果
	Action* popupLayer = Sequence::create(
			DelayTime::create(0.3),
			ScaleTo::create(0.15, 0.98),
			ScaleTo::create(0.15, 1.01),
			ScaleTo::create(0.15, 0.99),
			ScaleTo::create(0.15, 1),
			NULL
	);
	_panel->runAction(popupLayer);
}

void PopupLayer::destroy()
{
	Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(this);
	this->removeFromParent();
}

bool PopupLayer::hide()
{
	//设定tag防止连续调用
	int tag = 1;
	if(_panel->getActionByTag(tag) == nullptr)
	{
		_panel->stopAllActions();
		Sequence* seq = Sequence::create(ScaleTo::create(0.15,0.0),
				CallFunc::create(CC_CALLBACK_0(PopupLayer::destroy,this)),NULL);
		seq->setTag(tag);
		_panel->runAction(seq);
		return true;
	}
	return false;
}

void PopupLayer::onKeyReleased(EventKeyboard::KeyCode keyCode,Event* event)
{
	if(keyCode == EventKeyboard::KeyCode::KEY_BACKSPACE
			|| keyCode == EventKeyboard::KeyCode::KEY_ESCAPE
			|| keyCode == EventKeyboard::KeyCode::KEY_RETURN)
	{
		//屏蔽底层按键
		event->stopPropagation();
		hide();
	}
}
 
void PopupLayer::onExit()
{
	CCLog("popup on exit.");
	Layer::onExit();
}
