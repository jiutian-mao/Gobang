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
		STWO = 2,//眠二
		FTWO = 4,//假活二
		STHREE = 5,//眠三
		TWO = 8,//活二
		FTHREE = 15,//假活三
		SFOUR = 38,//冲四(玩家)
		SFOUR1 = 390,//冲四(电脑)
		SFOUR2 = 37,//冲四(跳冲四)
		SFOUR3 = 388,//冲四(跳冲四,电脑)
		THREE = 40,//活三
		THREE1 = 42,//跳三
		HFOUR = 365,//跳四(电脑)
		HFOUR1 = 39,//跳四(玩家)
		FOUR_TWO = 372,//冲四加活二
		THREE_THREE = 375,//双活三
		FOUR_THREE = 390,//冲四加活三
		FOUR_FOUR = 395,//双四
		FOUR = 380,//活四
		FOUR1 = 400,//活四(电脑)
		FIVE = 1000,//五连
	};
};


#endif//__GAME_CONTROL__
