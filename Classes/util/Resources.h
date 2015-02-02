#ifndef __RESOURCES_H__
#define __RESOURCES_H__
#include "stdafx.h"

//资源管理单例
class Resources {
public:

	enum TextureResType
    {
        LOCAL = 0,//本地文件
        PLIST = 1//plist文件资源
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
