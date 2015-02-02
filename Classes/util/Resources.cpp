#include "Resources.h"

Resources::Resources():
	stringFileName("string.plist")
{
	string file = FileUtils::getInstance()->fullPathForFilename(stringFileName);
	stringMap = FileUtils::getInstance()->getValueMapFromFile(file);
}
Resources::~Resources() {
	
}

string Resources::getString(string key){
	return stringMap.find(key)->second.asString();
}

Resources* Resources::_instance = nullptr;
Resources* Resources::getInstance(){
	if(_instance == nullptr){
		_instance = new Resources();
	}
	return _instance;
}

void Resources::destroyInstance()
{
	CC_SAFE_DELETE(_instance);
}
