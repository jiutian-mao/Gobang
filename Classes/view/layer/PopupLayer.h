#ifndef __POPUP_LAYER_H__
#define __POPUP_LAYER_H__
#include "stdafx.h"

class PopupLayer : public Layer{
public:
    PopupLayer();
    ~PopupLayer();
    
    CREATE_FUNC(PopupLayer);
    static PopupLayer* create(const char* backgroundImage,Resources::TextureResType texType,Size dialogSize);
 
    //touch�¼����� �������´���
    bool onTouchBegan(Touch* touch, Event* event) override;
    void onTouchEnded(Touch* touch, Event* event) override;
 
    //����
    void setTitle(const char* title, int fontsize = 20);
    //�ı�
    void setContentText(const char* text, int fontsize = 20, int padding = 50, int paddintTop = 100);
    //���button
    bool addButton(const char* normalImage, const char* selectedImage, const char* title, int tag = 0);
    //����button�ص��¼�
    void setCallbackFunc(Ref* target, SEL_CallFuncN callfun);

    //����Զ�����ͼ
	void addView(Node* node);
	void addView(Node* node,const Point& margin);

    //�رյ�����
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

	//�������������
	Node* _panel;
	EventListenerTouchOneByOne* listener;

    // �����������ߵĿհ���
    int _contentPadding;
    int _contentPaddingTop;
    Size _dialogContentSize;

    Ref* _callbackListener;
    SEL_CallFuncN _callback;
    
};
#endif //__POPUP_LAYER_H__
