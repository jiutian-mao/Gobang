#ifndef __POPUP_LAYER_H__
#define __POPUP_LAYER_H__
#include "stdafx.h"

class PopupLayer : public Layer{
public:
    PopupLayer();
    ~PopupLayer();
    
    CREATE_FUNC(PopupLayer);
    static PopupLayer* create(const char* backgroundImage,Resources::TextureResType texType,Size dialogSize);
 
    //touch事件监听 屏蔽向下触摸
    bool onTouchBegan(Touch* touch, Event* event) override;
    void onTouchEnded(Touch* touch, Event* event) override;
 
    //标题
    void setTitle(const char* title, int fontsize = 20);
    //文本
    void setContentText(const char* text, int fontsize = 20, int padding = 50, int paddintTop = 100);
    //添加button
    bool addButton(const char* normalImage, const char* selectedImage, const char* title, int tag = 0);
    //设置button回调事件
    void setCallbackFunc(Ref* target, SEL_CallFuncN callfun);

    //添加自定义视图
	void addView(Node* node);
	void addView(Node* node,const Point& margin);

    //关闭弹出层
    bool hide();

    virtual void onEnter() override;
    virtual void onExit() override;

	CC_SYNTHESIZE_RETAIN(Menu*, _pMenu, MenuButton);
	CC_SYNTHESIZE_RETAIN(Node*, _BG, SpriteBackGround);
	CC_SYNTHESIZE_RETAIN(LabelTTF*, _titlie, LabelTitle);
	CC_SYNTHESIZE_RETAIN(Label*, _contentText, LabelContentText);

protected:

	virtual bool init() override;

    void buttonCallBack(Ref* pSender);

    void onKeyReleased(EventKeyboard::KeyCode keyCode,Event* event);

    void destroy();

	//主内容区域面板
	Node* _panel;
	EventListenerTouchOneByOne* listener;

    // 文字内容两边的空白区
    int _contentPadding;
    int _contentPaddingTop;
    Size _dialogContentSize;

    Ref* _callbackListener;
    SEL_CallFuncN _callback;
    
};
#endif //__POPUP_LAYER_H__
