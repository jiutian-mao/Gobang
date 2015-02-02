#include "control/GameControl.h"
#include <algorithm>
#include <iostream>
//获取触摸所在点
Vec2 GameControl::touchPointTocell(Touch* touch,Layer* grid,int width,int height)
{
	Point p = grid->convertTouchToNodeSpace(touch);
	int x = floor(p.x + row * width / 2) / width;
	int y = floor((p.y + col * height / 2) / height);
	return Vec2(x,y);
}

void GameControl::setStartPoint(const Vec2& point)
{
	range.left = point.x;
	range.right = point.x;
	range.bottom = point.y;
	range.top = point.y;
	refreshRange(point);
}

void GameControl::refreshRange(const Vec2& point)
{
	range.left = max(min((int)point.x-3,(int)this->range.left),0);
	range.right = min(max((int)point.x+3,(int)this->range.right),col-1);
	range.bottom = max(min((int)point.y-3,(int)this->range.bottom),0);
	range.top = min(max((int)point.y+3,(int)this->range.top),row-1);
}

bool sortScore(GridElement& m1,GridElement& m2)
{
	//sort内部是不能进行随机排序的(比较次数是固定的)
	//if(m1.curScore + m1.opponentScore == m2.curScore + m2.opponentScore)
	if(m1.curScore == m2.curScore)
	{
		return m1.opponentScore < m2.opponentScore;
	}
	//return m1.curScore + m1.opponentScore < m2.curScore + m2.opponentScore;
	return m1.curScore < m2.curScore;
}

bool sortScore2(GridElement& m1,GridElement& m2)
{
	//sort内部是不能进行随机排序的(比较次数是固定的)
	if(m1.curScore + m1.opponentScore == m2.curScore + m2.opponentScore)
	{
		return m1.curScore < m2.curScore;
	}
	return m1.curScore + m1.opponentScore < m2.curScore + m2.opponentScore;
}

//获取最大同分值中随机数,增加游戏随机性
int getRandomMaxScore(vector<GridElement>& vec)
{
	GridElement& lastElement = *vec.rbegin();
	int last = vec.size() - 1;
	for(vector<GridElement>::reverse_iterator iter = vec.rbegin() + 1;iter != vec.rend();++iter)
	{
		if((*iter).curScore == lastElement.curScore && (*iter).opponentScore == lastElement.opponentScore)
		{
			last--;
		}
	}
	if(last == vec.size() - 1)
	{
		return last;
	}else
	{
		return rand() % (vec.size() - 1 - last) + last;
	}
}

Vec2 GameControl::calculateState(Type curType,int step)
{
	Type otherSide = getOtherType(curType);
	vector<GridElement> computeState;
	vector<GridElement> playerState;
//	log("calculateState%d,%d,%d",this->otherSide,curType,step);
	//提前分配空间，避免容器push扩容产生的复制、移动等操作，提高性能
	int capacity = (range.right - range.left + 1) * (range.top - range.bottom + 1) - step;
	computeState.reserve(capacity);
	playerState.reserve(capacity);
	for(int x = range.left;x <= range.right;x++)
	{
		for(int y = range.bottom;y <= range.top;y++)
		{
			if((*grid)[x * row + y] == nullptr)
			{
				//获取电脑得分与玩家得分
				int computeScore = getScore(x,y,curType);
				int playerScore = getScore(x,y,otherSide);

				GridElement compute(x,y,computeScore,playerScore);
				computeState.push_back(compute);

				GridElement player(x,y,playerScore,computeScore);
				playerState.push_back(player);
			}else
			{
				Chess* chess = (*grid)[x * row + y];
//				string str = "addChess(Vec2("+StringUtils::toString(x) + "," + StringUtils::toString(y) + ")," + StringUtils::toString(chess->type)+");";
//				log("%s",str.c_str());
			}
		}
	}
	sort(computeState.begin(),computeState.end(),sortScore);
	/*for(auto iter = computeState.begin();iter != computeState.end();++iter)
	{
		log("myC1:~%d,%d,%d,%d",(*iter).x,(*iter).y,(*iter).curScore,(*iter).opponentScore);
	}*/
	GridElement maxC = *computeState.rbegin();
	//排序并获得最大分值的随机数,增加下棋的随机性
	sort(playerState.begin(),playerState.end(),sortScore);
	/*for(auto iter = playerState.begin();iter != playerState.end();++iter)
	{
		log("myP2 :~%d,%d,%d,%d",(*iter).x,(*iter).y,(*iter).curScore,(*iter).opponentScore);
	}*/
	//GridElement maxP = playerState[getRandomMaxScore(playerState)];
	GridElement maxP = *playerState.rbegin();
//	log("POINT0: %d,%d,%d,%d,%d,%d,%d,%d",maxC.curScore,maxP.curScore,maxP.x,maxP.y,maxC.x,maxC.y,maxC.curScore,maxC.opponentScore);
	//log("%s","~~~~~~~~~");
	Vec2 pos;
	int maxScoreC = getMaxScore(maxC.x,maxC.y,curType);
	int maxScoreP = getMaxScore(maxP.x,maxP.y,otherSide);
	if(maxC.curScore  < maxP.curScore)
	{
		GridElement maxP2 = *computeState.rbegin();

		//取最大得分值点位另一端
		Vec2 diag = getDiagonalPoint(maxP.x,maxP.y,otherSide);
		for(auto iter = playerState.begin();iter != playerState.end();++iter)
		{
			if((*iter).x == diag.x && (*iter).y == diag.y)
			{
				maxP2 = (*iter);
			}
		}

		if(maxScoreP == FOUR_TWO)
		{

			//计算附近两个网格点以内得分值
			GridElement nearbyP = maxP;
			for(auto iter = playerState.rbegin();iter != playerState.rend();++iter)
			{
				GridElement endP = *iter;
				nearbyP = (*iter);
				if((*iter).curScore == maxScoreP)
				{
					for(auto iter = playerState.rbegin();iter != playerState.rend();++iter)
					{
						if(abs((*iter).x - endP.x) <= 2 && abs((*iter).y - endP.y) <= 2)
						{
							nearbyP.curScore += (*iter).curScore;
						}
					}
					if(nearbyP.curScore > maxP.curScore)
					{
						maxP = nearbyP;
					}

				}else
				{
					break;
				}
			}
			pos.x = maxP.x;
			pos.y = maxP.y;
		}else if(maxP.curScore + maxP.opponentScore < maxP2.curScore + maxP2.opponentScore
				&& maxP.curScore <= FOUR)
		{
			pos.x = maxP2.x;
			pos.y = maxP2.y;
		}else if(maxScoreC >= maxScoreP && maxP.curScore - maxScoreC < SFOUR2 * 2)
		{
			pos.x = maxC.x;
			pos.y = maxC.y;
		}else
		{
			pos.x = maxP.x;
			pos.y = maxP.y;
		}

	}else{

		if(maxScoreC == SFOUR1)
		{
			Vec2 SFour = getSFourResultPoint(maxC.x,maxC.y,curType);
			GridElement maxP4;
			for(auto iter = playerState.begin();iter != playerState.end();++iter)
			{
				if((*iter).x == SFour.x && (*iter).y == SFour.y)
				{
					maxP4 = (*iter);
				}
			}
			if(maxP4.curScore < SFOUR2)
			{
				pos.x = maxC.x;
				pos.y = maxC.y;
			}else
			{
				pos.x = maxP.x;
				pos.y = maxP.y;
			}
		}else
		{
			pos.x = maxC.x;
			pos.y = maxC.y;
		}

	}

	log("POINT2: %f,%f",pos.x,pos.y);

	return pos;
}

int GameControl::getScore(int x,int y,Type side)
{
	int s0 = calculateScore(getXline(x,y,side),side);
	int s1 = calculateScore(getYline(x,y,side),side);
	int s2 = calculateScore(getXYline(x,y,side),side);
	int s3 = calculateScore(getYXline(x,y,side),side);

	int score = s0 + s1 + s2 + s3;
	//棋谱分析
	if(score>=THREE + TWO)
	{
		int count = 0;
		int total = 0 ;
		//冲四棋谱
		if(s0 == SFOUR || s0 == SFOUR2){count++;};
		if(s1 == SFOUR || s1 == SFOUR2){count++;};
		if(s2 == SFOUR || s2 == SFOUR2){count++;};
		if(s3 == SFOUR || s3 == SFOUR2){count++;};
		if(count >=2)
		{
			return FOUR_FOUR;
		}else if(count>=1)
		{
			if( s0 >= HFOUR1 || s1 >= HFOUR1 || s2 >= HFOUR1 || s3 >= HFOUR1)
			{
				return FOUR_THREE;
			}/*else{
				if(s0 < SFOUR){total+=s0;}
				if(s1 < SFOUR){total+=s1;}
				if(s2 < SFOUR){total+=s2;}
				if(s3 < SFOUR){total+=s3;}
				return FOUR_TWO + total;
			}*/
		}

		//双活三
		count = 0;
		if(s0 == THREE || s0 == THREE1){count++;};
		if(s1 == THREE || s1 == THREE1){count++;};
		if(s2 == THREE || s2 == THREE1){count++;};
		if(s3 == THREE || s3 == THREE1){count++;};
		if(count >=2)
		{
			return THREE_THREE;
		}
	}
	return score;
}

int GameControl::getMaxScore(int x,int y,Type side)
{
	int s0 = calculateScore(getXline(x,y,side),side);
	int s1 = calculateScore(getYline(x,y,side),side);
	int s2 = calculateScore(getXYline(x,y,side),side);
	int s3 = calculateScore(getYXline(x,y,side),side);
	int manual = getchessManual(s0,s1,s2,s3);
	if(manual!=-1)return manual;
	return max(max(max(s0,s1),s2),s3);
}

int GameControl::getchessManual(int s0,int s1,int s2,int s3)
{
	int score = s0 + s1 + s2 + s3;
	//棋谱分析
	if(score>=SFOUR2 + TWO)
	{
		int count = 0;

		//冲四棋谱
		if(s0 == SFOUR || s0 == SFOUR2){count++;};
		if(s1 == SFOUR || s1 == SFOUR2){count++;};
		if(s2 == SFOUR || s2 == SFOUR2){count++;};
		if(s3 == SFOUR || s3 == SFOUR2){count++;};
		if(count >=2)
		{
			return FOUR_FOUR;
		}else if(count>=1)
		{
			if( s0 >= HFOUR1 || s1 >= HFOUR1 || s2 >= HFOUR1 || s3 >= HFOUR1)
			{
				return FOUR_THREE;
			}else{
				if(s0 == TWO){count++;}
				if(s1 == TWO){count++;}
				if(s2 == TWO){count++;}
				if(s3 == TWO){count++;}
				return FOUR_TWO;
			}
		}

		//双活三
		count = 0;
		if(s0 == THREE || s0 == THREE1){count++;};
		if(s1 == THREE || s1 == THREE1){count++;};
		if(s2 == THREE || s2 == THREE1){count++;};
		if(s3 == THREE || s3 == THREE1){count++;};
		if(count >=2)
		{
			return THREE_THREE;
		}
	}
	return -1;
}

Vec2 GameControl::getDiagonalPoint(int x,int y,Type side)
{
	int s0 = calculateScore(getXline(x,y,side),side);
	int s1 = calculateScore(getYline(x,y,side),side);
	int s2 = calculateScore(getXYline(x,y,side),side);
	int s3 = calculateScore(getYXline(x,y,side),side);
	int maxS = max(max(max(s0,s1),s2),s3);
	int i;
	int j;
	int count;
	Vec2 point(x,y);

	//X
	if(maxS == s0)
	{
		i = x;
		count = 0;
		while(--i >= range.left && getChessType(i * row + y) == side){count++;}
		if(count>1)
		{
			point.x = i;
		}
		i = x;
		count = 0;
		while(++i <= this->range.right && getChessType(i * row + y) == side){count++;}
		if(count>1)
		{
			point.x = i;
		}
	}

	//Y
	if(maxS == s1)
	{
		i = y;
		count = 0;
		while(--i >= 0 && getChessType(x * row + i) == side){count++;}
		if(count > 1)
		{
			point.y = i;
		}
		i = y;
		count = 0;
		while(++i >= 0 && getChessType(x * row + i) == side){count++;}
		if(count > 1)
		{
			point.y = i;
		}

	}else if(maxS == s2)
	{
		//XY
		//左下角:/
		i = x;
		j = y;
		count = 0;
		while(--i>=range.left&&--j>=range.bottom&&getChessType(i * row + j)==side){count++;}
		if(count > 1)
		{
			point.x = i;
			point.y = j;
		}

		//右上角:/
		i = x;
		j = y;
		count = 0;
		while(++i<=range.right&&++j<=range.top&&getChessType(i * row + j)==side){count++;}
		if(count > 1)
		{
			point.x = i;
			point.y = j;
		}

	}else if(maxS == s3)
	{
		//YX
		i = x;
		j = y;
		count = 0;
		while(--i >=range.left && ++j<=range.top && getChessType(i * row + j) == side){count++;}
		if(count > 1)
		{
			point.x = i;
			point.y = j;
		}

		i = x;
		j = y;
		count = 0;
		while(++i<=range.right && --j>=range.bottom && getChessType(i * row + j) == side){count++;};
		if(count > 1)
		{
			point.x = i;
			point.y = j;
		}

	}

	return point;
}

Vec2 GameControl::getSFourResultPoint(int x,int y,Type side)
{
	string result = getXline(x,y,side);
	int s0 = calculateScore(getXline(x,y,side),side);
	int s1 = calculateScore(getYline(x,y,side),side);
	int s2 = calculateScore(getXYline(x,y,side),side);
	int s3 = calculateScore(getYXline(x,y,side),side);
	int maxS = max(max(max(s0,s1),s2),s3);
	int i = x;
	int j = y;
	int count;
	Vec2 point(x,y);
	//X
	if(maxS == s0)
	{
		i = x;
		count = 0;
		while(--i >= range.left && getChessType(i * row + y) == side){count++;}
		if(count > 1)
		{
			point.x = x + 1;
		}
		i = x;
		count = 0;
		while(++i <= this->range.right && getChessType(i * row + y) == side){count++;}
		if(count > 1)
		{
			point.x = x - 1;
		}
	}

	//Y
	if(maxS == s1)
	{
		i = y;
		count = 0;
		while(--i >= 0 && getChessType(x * row + i) == side){count++;}
		if(count > 1)
		{
			point.y = y + 1;
		}
		i = y;
		count = 0;
		while(++i >= 0 && getChessType(x * row + i) == side){count++;}
		if(count > 1)
		{
			point.y = i - 1;
		}
	}else if(maxS == s2)
	{
		//XY
		//左下角:/
		i = x;
		j = y;
		count = 0;
		while(--i >= range.left && --j >= range.bottom && getChessType(i * row + j) == side){count++;}
		if(count>1)
		{
			point.x = x+1;
			point.y = y+1;
		}

		//右上角:/
		i = x;
		j = y;
		count = 0;
		while(++i <= range.right && ++j <= range.top && getChessType(i * row + j) == side){count++;}
		if(count>1)
		{
			point.x = x-1;
			point.y = y-1;
		}

	}else if(maxS == s3)
	{
		//YX
		//左上角
		i = x;
		j = y;
		count = 0;
		while(--i >=range.left && ++j<=range.top && getChessType(i * row + j) == side){count++;}
		if(count>1)
		{
			point.x = x+1;
			point.y = y-1;
		}

		//右下角
		i = x;
		j = y;
		count = 0;
		while(++i<=range.right && --j>=range.bottom && getChessType(i * row + j) == side){count++;};
		if(count>1)
		{
			point.x = x-1;
			point.y = y+1;
		}
	}

	return point;
}

Type GameControl::getChessType(int xy)
{
	Chess* chess = (*grid)[xy];
	return chess ? chess->type : NORNAL;
}

//检查目标类型
bool GameControl::checkType(Type curType, Type targetType,Type otherType, int& count) {
	if(curType != targetType)
	{
		if(curType == otherType || ++count >= 2)
		{
			return false;
		}
	}else
	{
		count = 0;
	}
	return true;
}

string GameControl::getXline(int x,int y,Type curType)
{
	string value = "";
	Type otherSide = getOtherType(curType);
	int i = x;
	int k = 0;
	while(--i >= range.left)
	{
		Type type = getChessType(i * row + y);
		value += StringUtils::toString(type);
		if(!checkType(type,curType,otherSide,k))break;
	}
	reverse(value.begin(),value.end());
	value += StringUtils::toString(curType);

	i = x;
	k = 0;
	while(++i <= this->range.right)
	{
		Type type = getChessType(i * row + y);
		value += StringUtils::toString(type);
		if(!checkType(type,curType,otherSide,k))break;
	};

	return value;
}

string GameControl::getYline(int x,int y,Type curType)
{
	string value = "";
	Type otherSide = getOtherType(curType);
	int i = y;
	int k = 0;
	while(--i >= range.bottom)
	{
		Type type = getChessType(x * row + i);
		value += StringUtils::toString(type);
		if(!checkType(type,curType,otherSide,k))break;
	}
	reverse(value.begin(),value.end());
	value += StringUtils::toString(curType);

	i = y;
	k = 0;
	while(++i<=this->range.top)
	{
		Type type = getChessType(x * row + i);
		value += StringUtils::toString(type);
		if(!checkType(type,curType,otherSide,k))break;
	};
	return value;
}

string GameControl::getXYline(int x,int y,Type curType)
{
	string value = "";
	Type otherSide = getOtherType(curType);

	//左下角:/
	int i = x;
	int j = y;
	int k = 0;
	while(--i >= range.left && --j >= range.bottom)
	{
		Type type = getChessType(i * row + j);
		value += StringUtils::toString(type);
		if(!checkType(type,curType,otherSide,k))break;
	}

	reverse(value.begin(),value.end());
	value += StringUtils::toString(curType);

	//右上角:/
	i = x;
	j = y;
	k = 0;
	while(++i <= range.right && ++j <= range.top)
	{
		Type type = getChessType(i * row + j);
		value += StringUtils::toString(type);
		if(!checkType(type,curType,otherSide,k))break;
	}
	return value;
}

string GameControl::getYXline(int x,int y,Type curType)
{
	string value = "";
	Type otherSide = getOtherType(curType);

	//左上角:
	int i = x;
	int j = y;
	int k = 0;
	while(--i >= range.left && ++j <= range.top)
	{
		Type type = getChessType(i * row + j);
		value += StringUtils::toString(type);
		if(!checkType(type,curType,otherSide,k))break;
	}

	reverse(value.begin(),value.end());
	value += StringUtils::toString(curType);

	//右下角:
	i = x;
	j = y;
	k = 0;
	while(++i <= range.right && --j >= range.bottom)
	{
		Type type = getChessType(i * row + j);
		value += StringUtils::toString(type);
		if(!checkType(type,curType,otherSide,k))break;
	}
	return value;
}


int GameControl::calculateScore(string score,Type side)
{
	string otherside = StringUtils::toString(getOtherType(side));

	//以下注释中 * 为本方棋子，o 为对方棋子，_ 为空格
	//五连: *****
	string five = StringUtils::toString(side * 11111);
	//活四: _****_
	string four = "0" + StringUtils::toString(side * 1111) + "0";
	//跳四: _*_***_
	string h_lfour = "0" + StringUtils::toString(side * 10111) + "0";
	//跳四: _**_**_
	string h_cfour = "0" + StringUtils::toString(side * 11011) + "0";
	//跳四: _***_*_
	string h_rfour = "0" + StringUtils::toString(side * 11101) + "0";
	//活三: _***_
	string three = "0" + StringUtils::toString(side * 111) + "0";
	//跳三: _**_*_
	string h_lthree = "0" + StringUtils::toString(side * 1101) + "0";
	//跳三: _*_**_
	string h_rthree = "0" + StringUtils::toString(side * 1011) + "0";
	//活二: _**_
	string two = "0" + StringUtils::toString(side * 11) + "0";
	//跳活二: _*_*_
	string jtwo = "0" + StringUtils::toString(side * 101) + "0";
	//跳活二: _*__*_
	string ctwo = "0" + StringUtils::toString(side * 1001) + "0";
	//冲四: ****_
	string lfour = otherside + StringUtils::toString(side * 1111) + "0";
	//冲四: _****
	string rfour = "0" + StringUtils::toString(side * 1111) + otherside;
	//冲四: *_***
	string l_four = StringUtils::toString(side * 10111);
	//冲四: ***_*
	string r_four = StringUtils::toString(side * 11101);
	//眠三: o***_
	string lthree = otherside + StringUtils::toString(side * 111) + "0";
	//眠三: _***o
	string rthree = "0" + StringUtils::toString(side * 111) + otherside;
	//眠二: o**_
	string ltwo = otherside + StringUtils::toString(side * 11) + "0";
	//眠二: _**o
	string rtwo = "0" + StringUtils::toString(side * 11) + otherside;
	//假活三: ***_o
	string rfthree = StringUtils::toString(side * 111) + "0" + otherside;
	//假活三: o_***
	string lfthree = otherside + "0" + StringUtils::toString(side * 111);

	string str = score;
	int res = 0;
	int index = str.find(five);
	if(str.find(five) != -1)
	{
		res = FIVE;
		if(side == this->otherSide)
		{
			res *= 2;
		}
	}else if(str.find(four) != -1)
		res = side == this->otherSide ? FOUR1 : FOUR;
	else if(str.find(h_lfour) != -1 || str.find(h_cfour) != -1 || str.find(h_rfour) != -1)
		res = side == this->otherSide ? HFOUR : HFOUR1;
	else if(str.find(h_lthree) != -1 || str.find(h_rthree) != -1)
		res = side == this->otherSide ? THREE1 : THREE2;
	else if(str.find(three) != -1)
		res = THREE ;
	else if(str.find(jtwo) != -1)
		res = side == this->otherSide ? TWO1 : TWO;
	else if(str.find(two)!=-1) 
		res = TWO;
	else if(str.find(ctwo)!=-1) 
		res = TWO2;
	else if(str.find(lfour) != -1 || str.find(rfour) != -1)
		res = side == this->otherSide ? SFOUR1 : SFOUR;
	else if(str.find(l_four) != -1 || str.find(r_four) != -1)
		res = SFOUR2;
	else if(str.find(lfthree) != -1 || str.find(rfthree) != -1)
		res = FTHREE;
	else if(str.find(lthree) != -1 || str.find(rthree) != -1)
		res = STHREE;
	else if(str.find(ltwo) != -1 || str.find(rtwo) != -1)
		res = STWO;
	else
		res = 0;
	return res;
}

bool GameControl::checkWinner(int x,int y,Type crtType)
{
	Chess* chess = nullptr;
	int i = x;
	int j;
	int len = 5;
	int count = -1;
	
	do
	{
		//X线:--
		count = 1;
		i = x;
		while(--i >= 0 && getChessType(i * row + y) == crtType){count++;}
		i = x;
		while(++i < col && getChessType(i * row + y) == crtType){count++;}
		if(count >= len)
			break;

		//Y线:|
		count = 1;
		i = y;
		while(--i >= 0 && getChessType(x * row + i) == crtType){count++;}
		i = y;
		while(++i < row && getChessType(x * row + i) == crtType){count++;}
		if(count >= len)
			break;

		//正斜:/
		count = 1;
		i = x;
		j = y;
		while (--i>=0 && --j>=0 && getChessType(i * row + j) == crtType){count++;}
		i = x;
		j = y;
		while(++i<row && ++j <col && getChessType(i * row + j) == crtType){count++;}
		if(count>=len)
			break;

		//反斜
		count = 1;
		i = x;
		j = y;
		while(--i>=0 && ++j<row && getChessType(i * row + j) == crtType){count++;}
		i = x;
		j = y;
		while(++i<row && --j>=0 && getChessType(i * row + j) == crtType){count++;}
		if(count >= len)break;

	}while(false);

	return count >= len;
}

list<Chess*> GameControl::checkWinnerList(int x,int y,Type crtType)
{
	list<Chess*> cList;
	int i = x;
	int j;
	int len = 4;

	do
	{
		//X线:--
		i = x;
		while(--i >= 0 && getChessType(i * row + y) == crtType)
		{
			cList.push_back((*grid)[i * row + y]);
		}
		i = x;
		while(++i < col && getChessType(i * row + y) == crtType)
		{
			cList.push_back((*grid)[i * row + y]);
		}
		if(cList.size() >= len)break;

		//Y线:|
		cList.clear();
		i = y;
		while(--i >= 0 && getChessType(x * row + i) == crtType)
		{
			cList.push_back((*grid)[x * row + i]);
		}
		i = y;
		while(++i < row && getChessType(x * row + i) == crtType)
		{
			cList.push_back((*grid)[x * row + i]);
		}
		if(cList.size() >= len)break;


		//正斜:/
		cList.clear();
		i = x;
		j = y;
		while(--i >= 0 && --j >= 0 && getChessType(i * row + j) == crtType)
		{
			cList.push_back((*grid)[i * row + j]);
		}
		i = x;
		j = y;
		while(++i < row && ++j < col && getChessType(i * row + j) == crtType)
		{
			cList.push_back((*grid)[i * row + j]);
		}
		if(cList.size() >= len)break;

		//反斜
		cList.clear();
		i = x;
		j = y;
		while(--i >= 0 && ++j < row && getChessType(i * row + j) == crtType)
		{
			cList.push_back((*grid)[i * row + j]);
		}
		i = x;
		j = y;
		while(++i<row && --j>=0 && getChessType(i * row + j) == crtType)
		{
			cList.push_back((*grid)[i * row + j]);
		}
		if(cList.size() >= len)break;

	}while(false);

	cList.push_back((*grid)[x * row + y]);//len+1
	return cList;
}

GameControl::GameControl(vector<Chess*>* grid,int row,int col) :
		grid(grid),row(row),col(col)
{
	setStartPoint(Vec2(7,7));
	srand((unsigned)time(nullptr));
};

GameControl::~GameControl()
{
};
