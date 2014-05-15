#include "AppDelegate.h"
#include "BBGameScene.h"
#include "BBGameDataManager.h"
#include "GameCenterApi.h"
#include "MobClickCpp.h"
#include "BBWelcomeScene.h"

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
    }
    
    auto frameSize = glview->getFrameSize();
    std::vector<std::string> searchPaths;
    if ((int)frameSize.width == 1536) {
        log("hdr");
//        searchPaths.push_back("hdr");
        BBGameDataManager::getInstance()->setScreenScale(2);
    }else{
        log("hd");
//        searchPaths.push_back("hd");
    }
    
    FileUtils::getInstance()->setSearchPaths(searchPaths);
    
//    glview->setDesignResolutionSize(640, 1136, kResolutionShowAll);
    
    // turn on display FPS
    director->setDisplayStats(false);
    
    // set FPS. the default value is 1.0/60 if you don't call this
    director->setAnimationInterval(1.0 / 60);
    
//    GameCenterApi::getInstance()->authenticateLocalUser();
    
    // Umeng
    MobClickCpp::startWithAppkey("533f4d2f56240b5a23090104");
//    MobClickCpp::setLogEnabled(true);
    
    // create a scene. it's an autorelease object
    director->runWithScene(BBWelcomeScene::createScene());
    
    return true;
}

// This function will be called when the app is inactive. When comes a phone call,it's be invoked too
void AppDelegate::applicationDidEnterBackground() {
    Director::getInstance()->stopAnimation();
    
    // if you use SimpleAudioEngine, it must be pause
    // SimpleAudioEngine::sharedEngine()->pauseBackgroundMusic();
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground() {
    Director::getInstance()->startAnimation();
    
    // if you use SimpleAudioEngine, it must resume here
    // SimpleAudioEngine::sharedEngine()->resumeBackgroundMusic();
}
