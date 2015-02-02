#include "AppDelegate.h"
#include "view/scene/MainGame.h"
USING_NS_CC;

AppDelegate::AppDelegate() {

}

AppDelegate::~AppDelegate() 
{
}

bool AppDelegate::applicationDidFinishLaunching() {
    // initialize director
    auto director = Director::getInstance();
    auto glview = director->getOpenGLView();
    if(!glview) {
        glview = GLView::create("My Game");
        director->setOpenGLView(glview);
		if(Application::getInstance()->getTargetPlatform() == Platform::OS_WINDOWS){
			glview->setFrameSize(640,700);
		}else{
			glview->setFrameSize(640,1024);
		}
    }
	//ÉèÖÃÆÁÄ»´óÐ¡
	if(Application::getInstance()->getTargetPlatform() == Platform::OS_WINDOWS){
		glview->setDesignResolutionSize(640,700,ResolutionPolicy::NO_BORDER);
	}else{
		glview->setDesignResolutionSize(640,1024,ResolutionPolicy::NO_BORDER);
	}

    // turn on display FPS
    //director->setDisplayStats(true);

    // set FPS. the default value is 1.0/60 if you don't call this
    director->setAnimationInterval(1.0 / 60);

    // create a scene. it's an autorelease object
	auto scene = MainGame::createScene();
    // run
    director->runWithScene(scene);

    return true;
}

// This function will be called when the app is inactive. When comes a phone call,it's be invoked too
void AppDelegate::applicationDidEnterBackground() {
    Director::getInstance()->stopAnimation();

    // if you use SimpleAudioEngine, it must be pause
    // SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground() {
    Director::getInstance()->startAnimation();

    // if you use SimpleAudioEngine, it must resume here
    // SimpleAudioEngine::getInstance()->resumeBackgroundMusic();
}
