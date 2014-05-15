//
//  BBWelcomeScene.cpp
//  puzzle
//
//  Created by FengZi on 14-1-22.
//
//

#include "BBWelcomeScene.h"
#include "BBGameScene.h"
#include "BBThemeLayer.h"
#include "BBLevelLayer.h"
#include "BBGameDataManager.h"
#include "BBAudio.h"
#include "GameCenterApi.h"

BBWelcomeScene::BBWelcomeScene()
{
    
}

BBWelcomeScene::~BBWelcomeScene()
{
    
}

Scene* BBWelcomeScene::createScene()
{
    auto scene = Scene::create();
    auto layer = BBWelcomeScene::create();
    scene->addChild(layer);
    return scene;
}

bool BBWelcomeScene::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    initData();
    initSprite();
    return true;
}

#pragma mark - initData
void BBWelcomeScene::initData()
{
}
#pragma mark - initSprite
void BBWelcomeScene::initSprite()
{
    BBAudio::preloadMusic("main_song.mp3");
    BBAudio::playMusic("main_song.mp3");
    BBAudio::preloadEffect("select.mp3");
    
    Size visibleSize = Director::getInstance()->getVisibleSize();
    
    // 添加背景层
    addChild(LayerColor::create(Color4B(194, 226, 236, 255), visibleSize.width, visibleSize.height));
    
//    auto bgSpt = Scale9Sprite::create("bg_sky.png");
//    bgSpt->setContentSize(visibleSize);
//    bgSpt->setPosition(Point(visibleSize.width/2, visibleSize.height/2));
//    addChild(bgSpt);
    
    auto bgSpt = Sprite::create("bg_main1.png");
    bgSpt->setPosition(Point(visibleSize.width/2, visibleSize.height/2));
    bgSpt->setScale(visibleSize.width/bgSpt->getContentSize().width, visibleSize.height/bgSpt->getContentSize().height);
    addChild(bgSpt);
    
    m_cloudLayer = Layer::create();
    addChild(m_cloudLayer);
    
    createCloud1();
    createCloud2();
    
    auto titleBgLayer = LayerColor::create(Color4B(0, 0, 0, 125), visibleSize.width, 100);
    titleBgLayer->setPosition(Point(0, visibleSize.height - titleBgLayer->getContentSize().height));
    addChild(titleBgLayer);
    
    auto titleLabel = LabelTTF::create("选择主题", "Helvetica-Bold", 40);
    titleLabel->setPosition(Point(visibleSize.width/2, visibleSize.height - titleLabel->getContentSize().height - 5));
    addChild(titleLabel);
    
//    auto bottomBgSpt = Sprite::create("bg_bottom1.png");
//    bottomBgSpt->setPosition(Point(visibleSize.width/2, bottomBgSpt->getContentSize().height/2));
//    addChild(bottomBgSpt);
    
    auto *themeLayer = BBThemeLayer::create();
    addChild(themeLayer);
    
    int themeId = BBGameDataManager::getInstance()->getThemeId();
    if (themeId != 0) {
        themeLayer->addChild(BBLevelLayer::create(themeId));
    }
}

void BBWelcomeScene::createCloud1()
{
    Size visibleSize = Director::getInstance()->getVisibleSize();
    
    int heigt = visibleSize.height;
    auto cloudSpt = Sprite::create(__String::createWithFormat("map_cloud%d.png", (arc4random() % 3) + 1)->getCString());
    cloudSpt->setTag(101);
    cloudSpt->setPosition(Point(visibleSize.width + cloudSpt->getContentSize().width/2, (arc4random() % heigt) + visibleSize.height/2));
    m_cloudLayer->addChild(cloudSpt);
    
    cloudSpt->runAction(Sequence::create(MoveTo::create((arc4random() % 10) + 5, Point(-cloudSpt->getContentSize().width/2, cloudSpt->getPositionY())),CallFunc::create([this](){
        ((Sprite*)m_cloudLayer->getChildByTag(101))->removeFromParentAndCleanup(true);
        createCloud1();
    }),  NULL));
    cloudSpt->setScale(BBGameDataManager::getInstance()->getScreenScale());
}

void BBWelcomeScene::createCloud2()
{
    Size visibleSize = Director::getInstance()->getVisibleSize();
    
    auto cloudSpt = Sprite::create(__String::createWithFormat("map_cloud%d.png", (arc4random() % 3) + 1)->getCString());
    cloudSpt->setTag(102);
    int heigt = visibleSize.height - 100 - cloudSpt->getContentSize().height/2;
    cloudSpt->setPosition(Point(visibleSize.width + cloudSpt->getContentSize().width/2, (arc4random() % heigt) + visibleSize.height/2 + cloudSpt->getContentSize().height/2 + 100));
    m_cloudLayer->addChild(cloudSpt);
    
    cloudSpt->runAction(Sequence::create(MoveTo::create((arc4random() % 15) + 8, Point(-cloudSpt->getContentSize().width/2, cloudSpt->getPositionY())),CallFunc::create([this](){
        ((Sprite*)m_cloudLayer->getChildByTag(102))->removeFromParentAndCleanup(true);
        createCloud2();
    }),  NULL));
    cloudSpt->setScale(BBGameDataManager::getInstance()->getScreenScale());
}