#ifndef __CHESS_H__
#define __CHESS_H__
#include "cocos2d.h"
USING_NS_CC;

typedef enum ChessType
{
	NORNAL = 0,
	BLACK = 1,
	WHITE = 2
}Type;

static Type getOtherType(Type curType)
{return (Type)(BLACK + WHITE - curType);};

static Type getOtherType(int curType)
{return (Type)(BLACK + WHITE - curType);};

class Chess : public Ref
{
public:

	Type type;

	CREATE_FUNC(Chess);
	//get and set
	CC_SYNTHESIZE(Sprite*,sprite,Sprite);

protected:
	Chess(void){};
	virtual ~Chess(){};
	virtual bool init(){return true;};
};
#endif
