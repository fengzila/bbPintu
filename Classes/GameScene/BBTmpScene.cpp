//
//  BBTmpScene.cpp
//  BBFlycups
//
//  Created by FengZi on 14-4-10.
//
//

#include "BBTmpScene.h"
#include "BBGameDataManager.h"
#include "BBGameScene.h"
#include "BBWelcomeScene.h"

BBTmpScene::BBTmpScene()
{
    
}

BBTmpScene::~BBTmpScene()
{
    
}

Scene* BBTmpScene::createScene()
{
    auto scene = Scene::create();
    auto layer = BBTmpScene::create();
    scene->addChild(layer);
    return scene;
}

bool BBTmpScene::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    initData();
    return true;
}

#pragma mark - initData
void BBTmpScene::initData()
{
    Size visibleSize = Director::getInstance()->getVisibleSize();
    
    auto bgSpt = Sprite::create("bg_main1.png");
    bgSpt->setPosition(Point(visibleSize.width/2, visibleSize.height/2));
    bgSpt->setScale(visibleSize.width/bgSpt->getContentSize().width, visibleSize.height/bgSpt->getContentSize().height);
    addChild(bgSpt);
    
    auto *loadingLayer = Sprite::create("bg_main1.png");
    loadingLayer->setPosition(Point(visibleSize.width/2, visibleSize.height/2));
//    loadingLayer->setScale(visibleSize.width/loadingLayer->getContentSize().width, visibleSize.height/loadingLayer->getContentSize().height);
    addChild(loadingLayer);
    
    auto bgLogoSpt = Sprite::create("bg_logo.png");
    bgLogoSpt->setPosition(Point(visibleSize.width/2, visibleSize.height/2 + 100));
    addChild(bgLogoSpt);

    auto loadingSpt = Sprite::create("loading.png");
    loadingSpt->setScale(2.0);
    loadingSpt->setPosition(Point(visibleSize.width/2, visibleSize.height/2 - 50));
    loadingSpt->runAction(RepeatForever::create(RotateBy::create(1, -360)));
    loadingLayer->addChild(loadingSpt);
    
    schedule(schedule_selector(BBTmpScene::goGmaeScene), 1);
}

void BBTmpScene::goGmaeScene(float dt)
{
    int themeId = BBGameDataManager::getInstance()->getThemeId();
    int levelId = BBGameDataManager::getInstance()->getLevelId() + 1;
    
    unschedule(schedule_selector(BBTmpScene::goGmaeScene));
    
    if (levelId > 9) {
        themeId++;
    }
    
    if (themeId > 5) {
        themeId = 5;
        levelId = 9;
    }
    BBGameDataManager::getInstance()->setThemeIdAndLevelId(themeId, levelId);
        Director::getInstance()->replaceScene(BBGameScene::createScene());
}