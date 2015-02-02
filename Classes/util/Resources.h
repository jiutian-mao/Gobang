#ifndef __RESOURCES_H__
#define __RESOURCES_H__
#include "stdafx.h"

//��Դ������
class Resources {
public:

	enum TextureResType
    {
        LOCAL = 0,//�����ļ�
        PLIST = 1//plist�ļ���Դ
    };

	const string stringFileName;

	string getString(string key);

	static Resources* getInstance();
	static void destroyInstance();

private:

	static Resources* _instance;

	ValueMap stringMap;

	Resources();

	virtual ~Resources();
};

#endif // __STRING_RESOURCES_H__
