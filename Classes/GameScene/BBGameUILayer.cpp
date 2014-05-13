//
//  BBGameUILayer.cpp
//  puzzle
//
//  Created by FengZi on 14-1-22.
//
//

#include "BBGameUILayer.h"
#include "GameCenterApi.h"
#include "BBGameDataManager.h"
#include "BBWelcomeScene.h"

BBGameUILayer::BBGameUILayer()
{
    
}

BBGameUILayer::~BBGameUILayer()
{
    
}

bool BBGameUILayer::init()
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
void BBGameUILayer::initData()
{
    m_visibleSize = Director::getInstance()->getVisibleSize();
    m_homeLayer = NULL;
}
#pragma mark - initSprite
void BBGameUILayer::initSprite()
{
    auto *optionItem = MenuItemImage::create(
                                              "btn_menu.png",
                                              "btn_menu.png",
                                              CC_CALLBACK_1(BBGameUILayer::menuBackCallback, this));
    optionItem->setTag(1);
    
    if (BBGameDataManager::getInstance()->getScreenScale() == 1) {
        optionItem->setPosition(Point(500 + optionItem->getContentSize().width/2, m_visibleSize.height/2 + optionItem->getContentSize().height/2 + 260));
    } else {
        optionItem->setPosition(Point(80 + optionItem->getContentSize().width/2, m_visibleSize.height - optionItem->getContentSize().height/2 - 30));
    }
    
    Menu *menu = Menu::create(optionItem, NULL);
    menu->setPosition(Point::ZERO);
    addChild(menu);
}

void BBGameUILayer::showHomeLayer()
{
    if (m_homeLayer != NULL) {
        return;
    }
    m_homeLayer = LayerColor::create(Color4B(0, 0, 0, 195), m_visibleSize.width, m_visibleSize.height);
    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = [](Touch* touch, Event* event){return true;};
    listener->onTouchMoved = [](Touch* touch, Event* event){};
    listener->onTouchEnded = [=](Touch* touch, Event* event){};
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, m_homeLayer);
//    _eventDispatcher->addEventListenerWithFixedPriority(listener, 1);
    
    addChild(m_homeLayer);
    
    auto *resumeItem = MenuItemImage::create(
                                              "btn_resume.png",
                                              "btn_resume.png",
                                              CC_CALLBACK_1(BBGameUILayer::menuBackCallback, this));
    resumeItem->setPosition(Point(m_visibleSize.width/2, m_visibleSize.height * 4/5));
    resumeItem->setTag(3);
    resumeItem->setScale(BBGameDataManager::getInstance()->getScreenScale());
    
    auto *newItem = MenuItemImage::create(
                                           "btn_retry.png",
                                           "btn_retry.png",
                                           CC_CALLBACK_1(BBGameUILayer::menuBackCallback, this));
    newItem->setPosition(Point(m_visibleSize.width/2, m_visibleSize.height * 3/5));
    newItem->setTag(4);
    newItem->setScale(BBGameDataManager::getInstance()->getScreenScale());
    
    auto *homeItem = MenuItemImage::create(
                                           "btn_home.png",
                                           "btn_home.png",
                                           CC_CALLBACK_1(BBGameUILayer::menuBackCallback, this));
    homeItem->setPosition(Point(m_visibleSize.width/2, m_visibleSize.height * 2/5));
    homeItem->setTag(5);
    homeItem->setScale(BBGameDataManager::getInstance()->getScreenScale());
    
    auto *rateItem = MenuItemImage::create(
                                            "btn_rate.png",
                                            "btn_rate.png",
                                            CC_CALLBACK_1(BBGameUILayer::menuBackCallback, this));
    rateItem->setPosition(Point(m_visibleSize.width/2, m_visibleSize.height * 1/5));
    rateItem->setTag(6);
    rateItem->setScale(BBGameDataManager::getInstance()->getScreenScale());
    
    Menu *menu = Menu::create(resumeItem, newItem, rateItem, homeItem, NULL);
    menu->setPosition(Point::ZERO);
    m_homeLayer->addChild(menu);

}

void BBGameUILayer::menuBackCallback(Ref* pSender)
{
    int tag = ((MenuItemLabel*)pSender)->getTag();
    switch (tag) {
        case 1:
            // play
            showHomeLayer();
            break;
        case 2:
            // gamecenter
            GameCenterApi::getInstance()->showLeaderboard();
            break;
        case 3:
            // 继续
            removeHomeLayer();
            break;
        case 4:
            // 重玩
            NotificationCenter::getInstance()->postNotification("kRefresh");
            removeHomeLayer();
            break;
        case 5:
            // home
            Director::getInstance()->replaceScene(BBWelcomeScene::createScene());
            break;
        case 6:
            // 赞
            GameCenterApi::getInstance()->goToAppStoreEvaluate();
            break;
            
        default:
            break;
    }
}

void BBGameUILayer::removeHomeLayer()
{
//    m_homeLayer->runAction(Sequence::createWithTwoActions(FadeOut::create(1), CallFuncN::create(CC_CALLBACK_1(BBGameUILayer::removeNode, this))));
    m_homeLayer->removeFromParentAndCleanup(true);
    m_homeLayer = NULL;
}

void BBGameUILayer::removeNode(Node* node)
{
    node->stopAllActions();
    node->removeFromParentAndCleanup(true);
    node = NULL;
}