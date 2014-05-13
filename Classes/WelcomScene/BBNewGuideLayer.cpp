//
//  BBNewGuideLayer.cpp
//  BBSudoku
//
//  Created by FengZi on 14-4-2.
//
//

#include "BBNewGuideLayer.h"
#include "BBGameScene.h"
#include "BBGameDataManager.h"
#include "GameCenterApi.h"

BBNewGuideLayer::BBNewGuideLayer()
{
    
}

BBNewGuideLayer::~BBNewGuideLayer()
{
    
}

bool BBNewGuideLayer::init()
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
void BBNewGuideLayer::initData()
{
    m_visibleSize = Director::getInstance()->getVisibleSize();
    
    m_curPage = 1;
    m_totalPages = 6;
    m_picWidth = 768*BBGameDataManager::getInstance()->getScreenScale();
}

void BBNewGuideLayer::initSprite()
{
    GameCenterApi::getInstance()->hiddenAdBanner();
    
    auto *bgLayer = LayerColor::create(Color4B(255, 255, 255, 255), m_visibleSize.width, m_visibleSize.height);
    addChild(bgLayer);

    m_scrollViewContent = Layer::create();
    addChild(m_scrollViewContent);
    
    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = CC_CALLBACK_2(BBNewGuideLayer::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(BBNewGuideLayer::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(BBNewGuideLayer::onTouchEnded, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, m_scrollViewContent);
    
    for (int i = 1; i <= m_totalPages; i++) {
        //背景
        Sprite *spt = Sprite::create(String::createWithFormat("help_%d.png", i-1)->getCString());
        spt->setPosition(Point(m_visibleSize.width/2 + m_picWidth*(i-1), m_visibleSize.height/2));
        spt->setScale(BBGameDataManager::getInstance()->getScreenScale());
        m_scrollViewContent->addChild(spt);
        
        if (i == m_totalPages) {
            auto *playItem = MenuItemImage::create(
                                                      "btn_play.png",
                                                      "btn_play.png",
                                                      CC_CALLBACK_1(BBNewGuideLayer::menuBeginCallback, this));
            playItem->setPosition(Point(spt->getContentSize().width/2, spt->getContentSize().height/3));
            playItem->setTag(1);
            
            auto menu = Menu::create(playItem, NULL);
            menu->setPosition(Point::ZERO);
            spt->addChild(menu);
        }
    }
}

bool BBNewGuideLayer::onTouchBegan(Touch *touch, Event * event)
{
    Point location = Director::getInstance()->convertToGL(touch->getLocation());

    m_beginX = location.x;
    m_preX = location.x;
    return true;
}

void BBNewGuideLayer::onTouchMoved(Touch *touch, Event * event)
{
    Point location = Director::getInstance()->convertToGL(touch->getLocation());
    
    m_scrollViewContent->setPositionX(m_scrollViewContent->getPositionX() + location.x - m_preX);
    
    m_preX = location.x;
}

void BBNewGuideLayer::onTouchEnded(Touch *touch, Event * event)
{
    Point location = Director::getInstance()->convertToGL(touch->getLocation());

    float diffX = location.x - m_beginX;
    if (diffX < -100) {
        // 往下翻
        nextPage();
    } else if (diffX > 100) {
        // 上
        prePage();
    } else {
        // 原位
        m_scrollViewContent->setPositionX(- (m_curPage-1)*m_picWidth);
    }
}

void BBNewGuideLayer::onTouchCancelled(Touch *touch, Event * event)
{
    
}

int BBNewGuideLayer::validPageValue(int value)
{
    if ( value == 0 ) value = 1;
    if ( value == m_totalPages + 1 ) value = m_totalPages;
            
    return value;
}

void BBNewGuideLayer::menuBeginCallback(Ref* pSender)
{
    int tag = ((MenuItemLabel*)pSender)->getTag();
    UserDefault *ud = UserDefault::getInstance();
    switch (tag) {
        case 1:
            ud->setBoolForKey("notNewUser", true);
            Director::getInstance()->replaceScene(TransitionSlideInR::create(.5, BBGameScene::createScene()));
            break;
        default:
            break;
    }
    ud->flush();
}

void BBNewGuideLayer::nextPage()
{
    m_curPage = validPageValue(m_curPage + 1);
    float targetX;
    if (m_curPage == m_totalPages) {
        targetX = - (m_totalPages - 1)*m_picWidth;
    } else {
        targetX = - (m_curPage-1)*m_picWidth;
    }
    m_scrollViewContent->runAction(MoveTo::create(.15, Point(targetX, m_scrollViewContent->getPositionY())));
}

void BBNewGuideLayer::prePage()
{
    m_curPage = validPageValue(m_curPage - 1);
    float targetX;
    if (m_curPage == 1) {
        targetX = 0;
    } else {
        targetX = -(m_curPage-1)*m_picWidth;
    }
    m_scrollViewContent->runAction(MoveTo::create(.15, Point(targetX, m_scrollViewContent->getPositionY())));
}