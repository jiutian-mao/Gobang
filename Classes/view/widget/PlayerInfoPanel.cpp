#include "view/widget/PlayerInfoPanel.h"

PlayerInfoPanel::PlayerInfoPanel()
{
	
}

PlayerInfoPanel::~PlayerInfoPanel() {
	
}

bool PlayerInfoPanel::init(const string& head,const string& name,
		const string& title,const string& qq,int playGameCount,const string& winRate)
{
	auto wedget = GUIReader::getInstance()->widgetFromJsonFile("playerInfoPanel.ExportJson");
	setWidget(wedget);

	ui::ImageView* headPortrait = static_cast<ImageView*>(Helper::seekWidgetByName(wedget,"headPortrait"));
	headPortrait->loadTexture(head);

	Text* playerName = static_cast<Text*>(Helper::seekWidgetByName(wedget,"playerName"));
	playerName->setString(name);

	Text* q = static_cast<Text*>(Helper::seekWidgetByName(wedget,"qq"));
	q->setString(qq);

	ui::ImageView* t = static_cast<ImageView*>(Helper::seekWidgetByName(wedget,"title"));
	t->loadTexture(title,TextureResType::PLIST);

	Text* pGC = static_cast<Text*>(Helper::seekWidgetByName(wedget,"playGameCount"));
	pGC->setString(StringUtils::toString(playGameCount));

	Text* winR = static_cast<Text*>(Helper::seekWidgetByName(wedget,"winRate"));
	winR->setString(winRate);

	return true;
}

PlayerInfoPanel* PlayerInfoPanel::create(const string& head,const string& name,
		const string& title,const string& qq,int playGameCount,const string& winRate)
{
	PlayerInfoPanel* user = new PlayerInfoPanel();
	if(user && user->init(head,name,title,qq,playGameCount,winRate)){
		user->autorelease();
		return user;
	}else{
		CC_SAFE_DELETE(user);
        return nullptr;
    }
}



