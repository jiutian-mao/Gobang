#ifndef __PLAYER_INFO_PANEL_H__
#define __PLAYER_INFO_PANEL_H__

#include "BaseWidget.h"
class PlayerInfoPanel: public BaseWidget {
public:
	
	static PlayerInfoPanel* create(const string& head,const string& name,
			const string& title,const string& qq,int playGameCount,const string& winRate);

protected:

	PlayerInfoPanel();

	virtual ~PlayerInfoPanel();

	virtual bool init(const string& head,const string& name,
			const string& title,const string& qq,int playGameCount,const string& winRate);

};

#endif /* __PLAYER_INFO_PANEL_H__ */
