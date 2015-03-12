#ifndef __GAME_CONTROL__
#define __GAME_CONTROL__

#include <string>
#include <vector>
#include <list>
#include "cocos2d.h"
#include "view/ui/Chess.h"

USING_NS_CC;
using namespace std;

struct Rects
{
    int left;
    int top;
    int right;
    int bottom;
};

class GridElement
{
public:
	GridElement(){};
	GridElement(int x,int y,int curScore,int opponentScore):
		x(x),y(y),curScore(curScore),opponentScore(opponentScore){};
	~GridElement(){};
	int x;
	int y;
	int curScore;
	int opponentScore;
};

class GameControl
{
public:

	GameControl(vector<Chess*>* grid,int row,int col);

	virtual ~GameControl();

	bool onTouchBegan(Touch* touch,Event* event);

	Vec2 touchPointTocell(Touch* touch,Layer* grid,int width,int height);

	void refreshRange(const Vec2& point);

	void setStartPoint(const Vec2& point);

	Vec2 calculateState(Type curType,int step);

	int calculateScore(string score,Type side);

	int getScore(int x,int y,Type side);

	int getMaxScore(int x,int y,Type side);

	int getchessManual(int s0,int s1,int s2,int s3);

	Vec2 getDiagonalPoint(int x,int y,Type side);

	Vec2 getSFourResultPoint(int x,int y,Type side);

	Type getChessType(int xy);

	string getXline(int x,int y,Type curType);

	string getYline(int x,int y,Type curType);

	string getXYline(int x,int y,Type curType);

	string getYXline(int x,int y,Type curType);
	
	bool checkType(Type curType,Type targetType,Type otherType,int& count);

	bool checkWinner(int x,int y,Type crtType);

	list<Chess*> checkWinnerList(int x,int y,Type crtType);
	//get and set
	CC_SYNTHESIZE(Type,otherSide,Side);

private:
	int mySide;
	int row;
	int col;
	Rects range;
	vector<Chess*>* grid;
	enum state
	{
		STWO = 2,//�߶�
		FTWO = 4,//�ٻ��
		STHREE = 5,//����
		TWO = 8,//���
		FTHREE = 15,//�ٻ���
		SFOUR = 38,//����(���)
		SFOUR1 = 390,//����(����)
		SFOUR2 = 37,//����(������)
		SFOUR3 = 388,//����(������,����)
		THREE = 40,//����
		THREE1 = 42,//����
		HFOUR = 365,//����(����)
		HFOUR1 = 39,//����(���)
		FOUR_TWO = 372,//���ļӻ��
		THREE_THREE = 375,//˫����
		FOUR_THREE = 390,//���ļӻ���
		FOUR_FOUR = 395,//˫��
		FOUR = 380,//����
		FOUR1 = 400,//����(����)
		FIVE = 1000,//����
	};
};


#endif//__GAME_CONTROL__
