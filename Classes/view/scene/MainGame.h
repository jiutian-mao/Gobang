#ifndef __MAINGAME_H__
#define __MAINGAME_H__

#include "stdafx.h"
#include "control/GameControl.h"
#include "view/ui/Chess.h"
#include "view/widget/BaseWidget.h"
#include <vector>

enum GameMode{
	LOCAL = 1,//单机模式
	FIELD = 2//好友对战
};

class MainGame:public Layer
{
public:
	static const int COL = 15;
	static const int ROW = 15;
	static const int WIDTH = 41;
	static const int HEIGHT = 41;
	
	static Scene* createScene();

	CREATE_FUNC(MainGame);

protected:

	virtual bool init() override;
	MainGame();
	virtual ~MainGame();

private:
	Size visibleSize;
	Vec2 origin;
	Type mySide;
	Type otherSide;
	GameMode mode;
	int step;
	bool isMoved;
	Layer* container;
	Layer* chessboard;
	Sprite* dragChess;
	Sprite* blackLocal;
	Sprite* whiteLocal;
	vector<Chess*>* grid;
	Sprite* lastStepChess;
	GameControl* control;
	EventListenerTouchOneByOne* listener;

	void initChessboard();
	void initPlayerInfo();
	void initGameMode();

	void startGame(Node* pSender,GameMode mode);
	void againGame(Node* pSender);

	bool onTouchBegan(Touch* touch,Event* event) override;
	void onTouchMoved(Touch* touch,Event* event) override;
	void onTouchEnded(Touch* touch,Event* event) override;
	void onTouchEvent(Ref* ref,TouchEventType type);
	void onKeyReleased(EventKeyboard::KeyCode keyCode,Event* event);

	void popButtonCallback(Node *pNode);

	void addChess(const Vec2& point,int crtType);

	void doWin(const Vec2& point,Type curType);

	void showResult(Type curType,Node* pSender=nullptr);

};
#endif //__MAINGAME_H__
