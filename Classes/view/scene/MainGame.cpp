#include "MainGame.h"
#include "control/GameControl.h"
#include "view/layer/PopupLayer.h"
#include "view/widget/PlayerInfoPanel.h"
#include "util/Resources.h"
#include "util/EventUtil.h"

#if(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#include <jni.h> 
#include "platform\android\jni\JniHelper.h"
#endif

Scene* MainGame::createScene()
{
	Scene* scene = Scene::create();
	Layer* layer = MainGame::create();
	scene->addChild(layer);
	return scene;
}

MainGame::MainGame():
		step(0),
		isMoved(false),
		visibleSize(Size::ZERO),
		origin(Vec2::ZERO),
		mySide(Type::BLACK),
		otherSide(Type::WHITE),
		mode(GameMode::LOCAL),
		container(nullptr),
		chessboard(nullptr),
		dragChess(nullptr),
		blackLocal(nullptr),
		whiteLocal(nullptr),
		grid(nullptr),
		lastStepChess(nullptr),
		control(nullptr),
		listener(nullptr)
{

}

MainGame::~MainGame()
{
}

bool MainGame::init()
{
	if ( !Layer::init() )
	{
		return false;
	}

	this->grid = new vector<Chess*>(COL*ROW);
	this->control = new GameControl(grid,ROW,COL);

	//触摸事件
	listener = EventListenerTouchOneByOne::create();
	listener->onTouchBegan = CC_CALLBACK_2(MainGame::onTouchBegan,this);
	listener->onTouchMoved = CC_CALLBACK_2(MainGame::onTouchMoved,this);
	listener->onTouchEnded = CC_CALLBACK_2(MainGame::onTouchEnded,this);

	//返回退出键监听
	EventListenerKeyboard* back = EventListenerKeyboard::create();
	back->onKeyReleased = CC_CALLBACK_2(MainGame::onKeyReleased,this);
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(back,this);

	//屏幕适配
	this->visibleSize = Director::getInstance()->getVisibleSize();
	this->origin = Director::getInstance()->getVisibleOrigin();
	const Size& design = Director::getInstance()->getOpenGLView()->getDesignResolutionSize();
	float scaleX = visibleSize.width / design.width;
	float scaleH = visibleSize.height / design.height;
	float scale = scaleX < scaleH ? scaleX : scaleH;

	SpriteFrameCache::getInstance()->addSpriteFramesWithFile("GameLayer.plist");
	Sprite* gameBG = Sprite::create("game_bg.png");
	gameBG->setPosition((visibleSize.width + origin.x*2)/2,(visibleSize.height + origin.y*2)/2);
	this->addChild(gameBG);

	this->container = Layer::create();
	container->setScale(scale);
	this->addChild(container);
	
	//最后一步棋子，用于标记提示
	this->lastStepChess = Sprite::createWithSpriteFrameName("game_last_step_mark.png");
	this->lastStepChess->retain();
	this->lastStepChess->setPosition(WIDTH/2,HEIGHT/2+3);

	initChessboard();
	initPlayerInfo();
	initGameMode();

	return true;
}

void MainGame::initPlayerInfo()
{
	//对手头像(小九天)
	Widget* playerInfo = GUIReader::getInstance()->widgetFromJsonFile("playerInfo.ExportJson");
	Widget* panel = dynamic_cast<Widget*>(Helper::seekWidgetByName(playerInfo,"playerPanelLeft"));
	ImageView* playerHead = dynamic_cast<ImageView*>(Helper::seekWidgetByName(playerInfo,"playerHead"));
	ImageView* playerTitle = dynamic_cast<ImageView*>(Helper::seekWidgetByName(playerInfo,"playerTitle"));
	Text* playerName = dynamic_cast<Text*>(Helper::seekWidgetByName(playerInfo,"playerName"));

	playerHead->loadTexture("maomao.jpg");
	playerHead->addTouchEventListener(this,toucheventselector(MainGame::onTouchEvent));
	playerTitle->loadTexture("headicon_local_level_3.png",TextureResType::PLIST);//崭露头角
	playerName->setString(Resources::getInstance()->getString("xiaojiutian"));

	panel->setPosition(Vec2(origin.x + panel->getContentSize().width/2 + 10,
			origin.y + visibleSize.height - 10));
	panel->removeFromParent();
	//localZOrder为0时,配合popupLayer使用时有BUG会导致panel面板不可见
	this->addChild(panel,1,1);

	//我的头像
	panel = dynamic_cast<Widget*>(Helper::seekWidgetByName(playerInfo,"playerPanelRight"));
	playerHead = dynamic_cast<ImageView*>(Helper::seekWidgetByName(playerInfo,"playerHead"));
	playerTitle = dynamic_cast<ImageView*>(Helper::seekWidgetByName(playerInfo,"playerTitle"));
	playerName = dynamic_cast<Text*>(Helper::seekWidgetByName(playerInfo,"playerName"));

	playerHead->loadTexture("local_robot_head_4.png");
	playerTitle->loadTexture("headicon_local_level_5.png",TextureResType::PLIST);//威震棋坛
	playerName->setString(Resources::getInstance()->getString("wo"));

	panel->setPosition(Vec2(origin.x + visibleSize.width + panel->getContentSize().width/2 - 10,
			origin.y + panel->getContentSize().height + 10));
	panel->removeFromParent();
	this->addChild(panel,1,2);
}

void MainGame::initGameMode()
{
	listener->setEnabled(false);

	Layer* menuLayer = Layer::create();
	menuLayer->setScale(0.6);
	this->addChild(menuLayer);

	//本地单机模式
	auto localItem = MenuItemImage::create("button_local_game_up.png","button_local_game_down.png",
		CC_CALLBACK_0(MainGame::startGame,this,menuLayer,GameMode::LOCAL));
	auto localBtn = Menu::create(localItem,nullptr);
	localBtn->setPositionY((visibleSize.height + origin.y*2)/2+50);
	menuLayer->addChild(localBtn);
	
	//好友对战
	auto fieldItem = MenuItemImage::create("button_with_friend_up.png","button_with_friend_down.png",
		CC_CALLBACK_0(MainGame::startGame,this,menuLayer,GameMode::FIELD));
	auto fieldBtn = Menu::create(fieldItem,nullptr);
	fieldBtn->setPositionY(localBtn->getPositionY()-localItem->getContentSize().height-10);
	menuLayer->addChild(fieldBtn);

	//根据游戏对局次数决定先后手顺序(轮序)
	int playGameCount = UserDefault::getInstance()->getIntegerForKey("playGameCount");
	this->mySide = (Type)(playGameCount%2+1);
	this->otherSide = getOtherType(this->mySide);
	this->control->setSide(this->otherSide);

	//头像上下两边黑白棋
	if(blackLocal == nullptr || whiteLocal == nullptr)
	{
		blackLocal = Sprite::createWithSpriteFrameName("game_black_local.png");
		whiteLocal = Sprite::createWithSpriteFrameName("game_white_local.png");
		this->addChild(blackLocal);
		this->addChild(whiteLocal);
	}

	//根据对战双方黑白棋设置显示坐标
	Vec2 topP(80,origin.y + visibleSize.height -
			this->getChildByTag(1)->getContentSize().height-50);
	Vec2 bottomP(origin.x + visibleSize.width - 50,
			this->getChildByTag(2)->getContentSize().height+50);
	if(this->mySide == WHITE){
		whiteLocal->setPosition(bottomP);
		blackLocal->setPosition(topP);
	}else{
		whiteLocal->setPosition(topP);
		blackLocal->setPosition(bottomP);
	}
}

void MainGame::startGame(Node* pSender,GameMode mode)
{
	pSender->removeFromParent();
	
	this->step = 0;
	this->mode = mode;
	listener->setEnabled(true);

	if(mode == LOCAL)
	{
		//玩家白棋，电脑黑棋，默认先下中心点
		if(this->mySide==WHITE)
		{
			addChess(Vec2(7,7),BLACK);
			this->step++;
		}
	}
}

bool MainGame::onTouchBegan(Touch* touch,Event* event)
{
//	log("onTouchBegan-name:%s",event->getCurrentTarget()->getName().c_str());
	if(EventUtil::touchHitRef(touch,event))
	{
		if(event->getCurrentTarget()->getName() == "chessboardBG")
		{
			return true;
		}
	}
	return false;
}

void MainGame::onTouchMoved(Touch* touch,Event* event)
{
	if(!isMoved)
	{
		isMoved = true;
		if(this->step % 2 + 1 == BLACK)
		{
			dragChess = Sprite::createWithSpriteFrameName("game_black_chess_node_picked_tiny_up.png");
		}else
		{
			dragChess = Sprite::createWithSpriteFrameName("game_white_chess_node_picked_tiny_up.png");
		}
		chessboard->addChild(dragChess);
	}
	if(dragChess != nullptr)
	{
		Vec2 point = this->control->touchPointTocell(touch,chessboard,WIDTH,HEIGHT);
		point.y += 2;
		dragChess->setPosition(point.x * WIDTH - ROW * WIDTH / 2 + WIDTH / 2,
				point.y * HEIGHT - ROW * HEIGHT / 2 + HEIGHT / 2);
		if(point.x < 0 || point.y < 0 || point.x >= ROW || point.y >= COL)
		{
			dragChess->setVisible(false);
		}else
		{
			dragChess->setVisible(true);
		}
	}
}

void MainGame::onTouchEnded(Touch* touch,Event* event)
{
	//log("onTouchEnded-name:%s",event->getCurrentTarget()->getName().c_str());
	if(!EventUtil::touchHitRef(touch,event))
	{
		return;
	}

	//棋盘操作
	if(event->getCurrentTarget()->getName() != "chessboardBG")
	{
		return;
	}

	Vec2 point = this->control->touchPointTocell(touch,chessboard,WIDTH,HEIGHT);
	if(isMoved)
	{
		isMoved = false;
		point.y += 2;
		if(dragChess != nullptr)
		{
			dragChess->removeFromParent();
			dragChess = nullptr;
		}
	}

	//判断是在棋盘范围内操作
	if(point.x < 0 || point.y < 0 || point.x >= ROW || point.y >= COL)
	{
		/*Type type = (Type)(this->step % 2 + 1);
		 Vec2 pos = this->control->calculateState(type,this->step);
		 log("postion: %f,%f",pos.x,pos.y);
		 addChess(pos,type);
		 step++;
		 if(this->control->checkWinner(pos.x,pos.y,type))
		 {
		 doWin(pos,otherSide);
		 return;
		 }*/
		return;
	}

	if((*grid)[point.x * ROW + point.y] == nullptr)
	{
		//玩家下
		Type curType = (Type)(step % 2 + 1);
		addChess(point,curType);
		step++;

		if(this->control->checkWinner(point.x,point.y,curType))
		{
			doWin(point,curType);
			return;
		}else if(step == ROW * COL)
		{
			doWin(point,NORNAL);//和棋
			return;
		}

		if(mode == LOCAL)
		{
			//计算机下
			Type otherType = getOtherType(curType);
			Vec2 pos = this->control->calculateState(otherType,this->step);
			log("POINT: %f,%f",pos.x,pos.y);
			addChess(pos,otherType);
			step++;
			if(this->control->checkWinner(pos.x,pos.y,otherType))
			{
				doWin(pos,otherSide);
			}else if(step == ROW * COL)
			{
				doWin(pos,NORNAL);//和棋
			}
		}
	}

	return;
}

void MainGame::onTouchEvent(Ref* ref,TouchEventType type)
{
	Widget* widget = static_cast<Widget*>(ref);
	log("onTouchEvent:%s,%d",widget->getName().c_str(),(int)type);
	if(type == TouchEventType::TOUCH_EVENT_ENDED)
	{
		if(widget->getName() == "playerHead")
		{
			//创建玩家信息面板弹窗层
			UserDefault* shared = UserDefault::getInstance();
			string name = Resources::getInstance()->getString("xiaojiutian");
			int playGameCount = shared->getIntegerForKey("playGameCount");
			int winCount = shared->getIntegerForKey("winCount");
			int winRate = playGameCount != 0 ? winCount * 100 / playGameCount : 0;
			PlayerInfoPanel* panel = PlayerInfoPanel::create("maomao.jpg",name,"headicon_local_level_3.png",
					"759702330",playGameCount,StringUtils::toString(winRate) + "%");
			auto widget = panel->getWidget();
			widget->setScale(container->getScale());
			PopupLayer* popup = PopupLayer::create();
			popup->addView(widget,Point(0,0));
			this->addChild(popup,9999);
			return;
		}
	}
}

void MainGame::addChess(const Point& point,int crtType)
{
	//log("addChess:,%f,%f",point.x,point.y);
	Chess* chess = Chess::create();
	chess->retain();
	Sprite* sprite = nullptr;
	if(crtType == BLACK)
	{
		sprite = Sprite::createWithSpriteFrameName("game_black_chess_node.png");
	}else
	{
		sprite = Sprite::createWithSpriteFrameName("game_white_chess_node.png");
	}
	chess->type = (Type)crtType;
	chess->setSprite(sprite);

	sprite->setPosition(point.x * WIDTH - ROW * WIDTH / 2 + WIDTH / 2,
			point.y * HEIGHT - ROW * HEIGHT / 2 + HEIGHT / 2);
	chessboard->addChild(sprite);
	(*grid)[point.x * ROW + point.y] = chess;
	control->refreshRange(point);

	lastStepChess->removeFromParent();
	sprite->addChild(lastStepChess);
}

void MainGame::popButtonCallback(Node *pNode)
{
	log("popButtonCallback-tag: %d",pNode->getTag());
	if(pNode->getTag() == 0)
	{
		//退出游戏
		#if (CC_TARGET_PLATFORM == CC_PLATFORM_WP8) || (CC_TARGET_PLATFORM == CC_PLATFORM_WINRT)
				MessageBox("You pressed the close button. Windows Store Apps do not implement a close button.","Alert");
				return;
		#endif
		Director::getInstance()->end();
		#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
				exit(0);
		#endif
	}else
	{

	}
}

void MainGame::againGame(Node* pSender)
{
	pSender->removeFromParent();
	for(int i = 0;i < grid->size();i++)
	{
		if((*grid)[i] != nullptr)
		{
			(*grid)[i]->getSprite()->removeFromParent();
			(*grid)[i]->release();
			(*grid)[i] = nullptr;
		}
	}
	this->step = 0;
	initGameMode();
}

void MainGame::showResult(Type curType,Node* pSender/* = nullptr */)
{
	if(pSender != nullptr)
	{
		pSender->removeFromParent();
	}
	
	Layer* layer = Layer::create();
	layer->setScale(container->getScale());
	this->addChild(layer);

	LayerColor* bg = LayerColor::create(Color4B(0,0,0,100));
	bg->setScale(1.5);
	layer->addChild(bg);
	Sprite* panel = Sprite::create("bg_gameresult.png");
	panel->setPosition((visibleSize.width + origin.x * 2) / 2,(visibleSize.height + origin.y * 2) / 2);
	layer->addChild(panel);

	//结果文字:胜、败、和
	SpriteFrameCache::getInstance()->addSpriteFramesWithFile("GameResultLayer.plist");
	Sprite* result = nullptr;
	if(curType == this->mySide)	{
		result = Sprite::createWithSpriteFrameName("gameresult_win.png");
	}else if(curType == this->otherSide){
		result = Sprite::createWithSpriteFrameName("gameresult_lose.png");
	}else{
		result = Sprite::createWithSpriteFrameName("gameresult_draw.png");
	}
	result->setPosition(panel->getContentSize().width / 2,panel->getContentSize().height - 60);
	panel->addChild(result);

	//再来一局按钮
	MenuItemSprite* againItem = MenuItemSprite::create(
			Sprite::createWithSpriteFrameName("gameresult_button_again_up.png"),
			Sprite::createWithSpriteFrameName("gameresult_button_again_down.png"),
			CC_CALLBACK_0(MainGame::againGame,this,layer));
	Menu* againBtn = Menu::create(againItem,nullptr);
	againBtn->setPosition(panel->getContentSize().width / 2,120);
	panel->addChild(againBtn);

	Action* popupEffect = Sequence::create(
			ScaleTo::create(0.0, 0.0),
			ScaleTo::create(0.22, 1.05),
			ScaleTo::create(0.22, 0.95),
			ScaleTo::create(0.1, 1.0),
			nullptr
			);
	panel->runAction(popupEffect);

	//根据zOrder显示顺序，屏蔽底层事件
	auto touch = EventListenerTouchOneByOne::create();
	touch->setSwallowTouches(true);
	touch->onTouchBegan = [](Touch* touch,Event* event)
	{
		return true;
	};
	this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(touch,layer);
}

void MainGame::doWin(const Vec2& point,Type curType)
{
	listener->setEnabled(false);

	//保存游戏对局次数以及胜利次数，胜率
	UserDefault* shared = UserDefault::getInstance();
	int playGameCount = shared->getIntegerForKey("playGameCount");
	shared->setIntegerForKey("playGameCount",playGameCount + 1);
	int winCount = shared->getIntegerForKey("winCount");
	if(curType == this->mySide)
	{
		shared->setIntegerForKey("winCount",winCount + 1);
	}
	shared->flush();

	//和棋
	if(curType == NORNAL)
	{
		showResult(curType);
		return;
	}

	//显示五连黄圈
	list<Chess*> cList = control->checkWinnerList(point.x,point.y,curType);
	for(list<Chess*>::iterator iter = cList.begin();iter != cList.end();iter++)
	{
		Sprite* blur = Sprite::createWithSpriteFrameName("game_chess_blur_mark.png");
		blur->setPosition(WIDTH / 2,HEIGHT / 2 + 2);
		(*iter)->getSprite()->addChild(blur);
		ActionInterval *blinkAction = CCBlink::create(15,15);
		blur->runAction(blinkAction);
	}
	
	//显示输赢结果
	SpriteFrameCache::getInstance()->addSpriteFramesWithFile("GameLayerAnimation.plist");
	Sprite* reason = Sprite::createWithSpriteFrameName("bg_game_reason.png");
	reason->setPosition((visibleSize.width + origin.x * 2) / 2,
			chessboard->getContentSize().height / 2 + HEIGHT * 6);
	container->addChild(reason);

	//五子连珠字体
	Sprite* wu = Sprite::createWithSpriteFrameName("game_reason_wu.png");
	Sprite* zi = Sprite::createWithSpriteFrameName("game_reason_zi.png");
	Sprite* lian = Sprite::createWithSpriteFrameName("game_reason_lian.png");
	Sprite* zhu = Sprite::createWithSpriteFrameName("game_reason_zhu.png");
	vector<Node*> childList;
	childList.push_back(wu);
	childList.push_back(zi);
	childList.push_back(lian);
	childList.push_back(zhu);
	int startX = (visibleSize.width + origin.x * 2) / 2 - 150;
	int w = 60;
	int h = 45;
	int i = 0;
	for(auto node : childList)
	{
		node->setPosition(startX + w * (++i),h);
		reason->addChild(node);
	}

	DelayTime* delay = DelayTime::create(3);
	CallFunc* func = CallFunc::create(CC_CALLBACK_0(MainGame::showResult,this,curType,reason));
	Sequence* sequ = Sequence::create(delay,func,nullptr);
	this->runAction(sequ);
}
 

void MainGame::initChessboard()
{
	//创建棋盘
	this->chessboard = Layer::create();
	Sprite* chessboardBG = Sprite::create("game_chess_board.png");
	container->addChild(chessboard);
	chessboardBG->setName("chessboardBG");
	this->chessboard->addChild(chessboardBG);
	this->chessboard->setPosition(visibleSize.width / 2 + origin.x,
			(visibleSize.height / 2 + origin.y * 2));
	this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener,chessboardBG);

	//绘制棋盘
	SpriteFrameCache::getInstance()->addSpriteFramesWithFile("game_ui.plist");
	for(int row=0;row<ROW;row++)
	{
		for(int col=0;col<COL;col++)
		{
			//创建网格线
			Sprite* line = nullptr;
			if(row == 0 && col==0){
				line = Sprite::createWithSpriteFrameName("cell_left_buttom.png");
			}else if(row == 0 && col==COL-1){
				line = Sprite::createWithSpriteFrameName("cell_left_top.png");
			}else if(col==0 && row == ROW-1){
				line = Sprite::createWithSpriteFrameName("cell_right_buttom.png");
			}else if(row == ROW-1 && col==COL-1){
				line = Sprite::createWithSpriteFrameName("cell_top_right.png");
			}else if(row==0){
				line = Sprite::createWithSpriteFrameName("cell_left.png");
			}else if(col==0){
				line = Sprite::createWithSpriteFrameName("cell_buttom.png");
			}else if(row==ROW-1){
				line = Sprite::createWithSpriteFrameName("cell_right.png");
			}else if(col==COL-1){
				line = Sprite::createWithSpriteFrameName("cell_top.png");
			}else if((row == ROW/2 && col == COL/2) || (row == 3 && col==3) ||
				(row == 3 && col==11) || (row == 11 && col==3) || (col == 11 && row==11) ){
					line = Sprite::createWithSpriteFrameName("cell_point.png");
			}else{
				line = Sprite::createWithSpriteFrameName("cell_normal.png");
			}
			line->setPosition(Vec2((row-ROW/2)*WIDTH,(col-ROW/2)*HEIGHT));
			chessboard->addChild(line);
		}
	}
}


void MainGame::onKeyReleased(EventKeyboard::KeyCode keyCode,Event* event)
{
	//退出游戏
	if(keyCode == EventKeyboard::KeyCode::KEY_BACKSPACE
			|| keyCode == EventKeyboard::KeyCode::KEY_ESCAPE
			|| keyCode == EventKeyboard::KeyCode::KEY_RETURN)
	{
		//弹出对话框
		PopupLayer* pl = PopupLayer::create("bg_playerinfo_local.png",Resources::TextureResType::PLIST,Size(590,448));
		pl->setScale(container->getScale());
		pl->setContentText(Resources::getInstance()->getString("exitGame").c_str(), 42, 10, 300);
		pl->getLabelContentText()->setColor(Color3B(71,44,31));
		pl->setCallbackFunc(this, callfuncN_selector(MainGame::popButtonCallback));//设置按钮回调
		pl->addButton("dialog_button_Yes_up.png", "dialog_button_Yes_down.png", "", 0);
		pl->addButton("dialog_button_No_up.png", "dialog_button_No_down.png", "", 1);
		this->addChild(pl,9999);
	}
}
