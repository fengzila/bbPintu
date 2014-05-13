//
//  BBThemeLayer.cpp
//  BBSudoku
//
//  Created by FengZi on 14-4-2.
//
//

#include "BBThemeLayer.h"
#include "BBGameScene.h"
#include "GameCenterApi.h"
#include "BBLevelLayer.h"
#include "BBAudio.h"
#include "BBGameDataManager.h"

BBThemeLayer::BBThemeLayer()
{
    
}

BBThemeLayer::~BBThemeLayer()
{
    m_pageArr->release();
}

bool BBThemeLayer::init()
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
void BBThemeLayer::initData()
{
    m_visibleSize = Director::getInstance()->getVisibleSize();
    
    m_curPage = 1;
    m_totalPages = 5;
    m_picWidth = m_visibleSize.width;
    
    m_pageArr = __Array::create();
    m_pageArr->retain();
    
    int themeId = BBGameDataManager::getInstance()->getThemeId();
    if (themeId == 0) {
        themeId = UserDefault::getInstance()->getIntegerForKey("themeId", 1);
    }
    m_curPage = validPageValue(themeId);
}

void BBThemeLayer::initSprite()
{
    setScale(BBGameDataManager::getInstance()->getScreenScale());
    GameCenterApi::getInstance()->hiddenAdBanner();
    
    m_themeBgSpt = Sprite::create("bg_theme.png");
    m_themeBgSpt->setPosition(Point(m_visibleSize.width/2, m_visibleSize.height/2));
    m_themeBgSpt->runAction(RepeatForever::create(RotateBy::create(30, 360)));
    addChild(m_themeBgSpt);
    
    m_scrollViewContent = Layer::create();
    m_scrollViewContent->setPosition(Point(- (m_curPage-1)*m_picWidth, 0));
    addChild(m_scrollViewContent);
    
    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = CC_CALLBACK_2(BBThemeLayer::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(BBThemeLayer::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(BBThemeLayer::onTouchEnded, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, m_scrollViewContent);
    
    for (int i = 1; i <= m_totalPages; i++) {
        //背景
        Sprite *spt = Sprite::create(String::createWithFormat("theme%d.png", i)->getCString());
        spt->setPosition(Point(m_visibleSize.width/2 + m_picWidth*(i-1), m_visibleSize.height/2));
        m_scrollViewContent->addChild(spt);
        
        UserDefault* ud = UserDefault::getInstance();
        int themeIdFramCache = ud->getIntegerForKey("themeId", 1);
        
        if (i > themeIdFramCache) {
            auto lockSpt = Sprite::create("lock.png");
            lockSpt->setPosition(Point(m_visibleSize.width/2 + m_picWidth*(i-1) + 150, m_visibleSize.height/2 - spt->getContentSize().height/2 + 90));
            m_scrollViewContent->addChild(lockSpt);
        } else {
            auto listener1 = EventListenerTouchOneByOne::create();
            listener1->setSwallowTouches(false);
            listener1->onTouchBegan = CC_CALLBACK_2(BBThemeLayer::themeOnTouchBegan, this);
            listener1->onTouchMoved = CC_CALLBACK_2(BBThemeLayer::themeOnTouchMoved, this);
            listener1->onTouchEnded = CC_CALLBACK_2(BBThemeLayer::themeOnTouchEnded, this);
            _eventDispatcher->addEventListenerWithSceneGraphPriority(listener1, spt);
        }
    }
    
    createPageControl();
    
    refreshPageControl();
    
    auto *settingItem = MenuItemImage::create(
                                              "btn_setting.png",
                                              "btn_setting.png",
                                              CC_CALLBACK_1(BBThemeLayer::menuCallback, this));
    settingItem->setPosition(Point(m_visibleSize.width - 50, m_visibleSize.height - 50));
    settingItem->setTag(1);
    
    auto menu = Menu::create(settingItem, NULL);
    menu->setPosition(Point::ZERO);
    addChild(menu);
}

void BBThemeLayer::createPageControl()
{
    float singleLen = 15;
    float gap = 10;
    float totalLen = m_totalPages*singleLen + gap*(m_totalPages - 1);
    float firstX = m_visibleSize.width/2 - totalLen/2;
    
    for (int i = 0; i < m_totalPages; i++) {
        auto pageSpt = Sprite::create("page_light.png");
        pageSpt->setPosition(Point(firstX + (singleLen + gap)*i, 80));
        addChild(pageSpt);
        
        m_pageArr->addObject(pageSpt);
    }
}

void BBThemeLayer::refreshPageControl()
{
    for (int i = 0; i < m_pageArr->count(); i++) {
        auto pageSpt = (Sprite*)m_pageArr->getObjectAtIndex(i);
        if (i+1 == m_curPage) {
            pageSpt->setOpacity(255);
        } else {
            pageSpt->setOpacity(125);
        }
    }
}


bool BBThemeLayer::onTouchBegan(Touch *touch, Event * event)
{
    Point location = Director::getInstance()->convertToGL(touch->getLocation());
    
    m_beginX = location.x;
    m_preX = location.x;
    
    m_themeBgSpt->setOpacity(0);
    return true;
}

void BBThemeLayer::onTouchMoved(Touch *touch, Event * event)
{
    Point location = Director::getInstance()->convertToGL(touch->getLocation());
    
    m_scrollViewContent->setPositionX(m_scrollViewContent->getPositionX() + location.x - m_preX);
    
    m_preX = location.x;
    
    m_themeBgSpt->setOpacity(0);
}

void BBThemeLayer::onTouchEnded(Touch *touch, Event * event)
{
    
    Point location = Director::getInstance()->convertToGL(touch->getLocation());
    
    float diffX = location.x - m_beginX;
    log("%f", diffX);
    if (diffX < -60) {
        // 往下翻
        nextPage();
    } else if (diffX > 60) {
        // 上
        prePage();
    } else {
        // 原位
        m_scrollViewContent->setPositionX(- (m_curPage-1)*m_picWidth);
        m_themeBgSpt->setOpacity(255);
    }
    refreshPageControl();
    
    BBAudio::playEffect("menuSelect.mp3");
}

void BBThemeLayer::onTouchCancelled(Touch *touch, Event * event)
{
    m_themeBgSpt->setOpacity(255);
}

bool BBThemeLayer::themeOnTouchBegan(Touch *touch, Event * event)
{
    auto target = static_cast<Sprite*>(event->getCurrentTarget());//获取的当前触摸的目标
    Point locationInNode = target->convertToNodeSpace(touch->getLocation());
    Size s = target->getContentSize();
    Rect rect = Rect(0, 0, s.width, s.height);
    
    if (rect.containsPoint(locationInNode)) {
        Point location = Director::getInstance()->convertToGL(touch->getLocation());
        m_beginX = location.x;
        return true;
    } else {
        return false;
    }

}

void BBThemeLayer::themeOnTouchMoved(Touch *touch, Event * event)
{}

void BBThemeLayer::themeOnTouchEnded(Touch *touch, Event * event)
{
    
    Point location = Director::getInstance()->convertToGL(touch->getLocation());
    
    if (abs(location.x - m_beginX) < 20) {
        // 手指滑动距离过短
        log("levelMap");
        
        addChild(BBLevelLayer::create(m_curPage));
        
        // 原位
        m_scrollViewContent->setPositionX(- (m_curPage-1)*m_picWidth);
        m_themeBgSpt->setOpacity(255);
        
        BBAudio::playEffect("select.mp3");
    }
}

void BBThemeLayer::themeOnTouchCancelled(Touch *touch, Event * event)
{}

int BBThemeLayer::validPageValue(int value)
{
    if ( value == 0 ) value = 1;
    if ( value == m_totalPages + 1 ) value = m_totalPages;
    
    return value;
}

void BBThemeLayer::showMenuLayer()
{
    Size visibleSize = Director::getInstance()->getVisibleSize();
    
    m_shadeLayer = LayerColor::create(Color4B(0, 0, 0, 195), visibleSize.width, visibleSize.height);
    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = [](Touch* touch, Event* event){return true;};
    listener->onTouchMoved = [](Touch* touch, Event* event){};
    listener->onTouchEnded = [=](Touch* touch, Event* event){};
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, m_shadeLayer);
    
    addChild(m_shadeLayer);
    
    m_soundItem = MenuItemToggle::createWithCallback(
             CC_CALLBACK_1(BBThemeLayer::menuCallback, this),
             MenuItemImage::create("btn_sound_on.png", "btn_sound_high.png"),
             MenuItemImage::create("btn_sound_off.png", "btn_sound_high.png"),
             NULL
                                                     
     );
    m_soundItem->setPosition(Point(visibleSize.width/2 - m_soundItem->getContentSize().width/2 - 100, visibleSize.height * 4/6));
    m_soundItem->setTag(2);
    
    m_musicItem = MenuItemToggle::createWithCallback(
         CC_CALLBACK_1(BBThemeLayer::menuCallback, this),
         MenuItemImage::create("btn_music_on.png", "btn_music_high.png"),
         MenuItemImage::create("btn_music_off.png", "btn_music_high.png"),
         NULL
         
     );
    
    m_musicItem->setPosition(Point(visibleSize.width/2 + m_musicItem->getContentSize().width/2 + 100, visibleSize.height * 4/6));
    m_musicItem->setTag(3);
    
    auto *closeItem = MenuItemImage::create(
                                           "setting_btn_ok.png",
                                           "setting_btn_ok.png",
                                           CC_CALLBACK_1(BBThemeLayer::menuCallback, this));
    closeItem->setPosition(Point(visibleSize.width/2, visibleSize.height * 3/6));
    closeItem->setTag(4);
    
    UserDefault* ud = UserDefault::getInstance();
    if (ud->getBoolForKey("soundOn", true)) {
        // on
        m_soundItem->setSelectedIndex(0);
    } else {
        // off
        
        m_soundItem->setSelectedIndex(1);
    }
    if (ud->getBoolForKey("musicOn", true)) {
        // on
        m_musicItem->setSelectedIndex(0);
    } else {
        // off
        m_musicItem->setSelectedIndex(1);
    }
    
    Menu *menu = Menu::create(closeItem, m_soundItem, m_musicItem, NULL);
    menu->setPosition(Point::ZERO);
    m_shadeLayer->addChild(menu);
    
}

void BBThemeLayer::menuCallback(Ref* pSender)
{
    BBAudio::playEffect("select.mp3");
    
    int tag = ((MenuItemLabel*)pSender)->getTag();
    UserDefault *ud = UserDefault::getInstance();
    switch (tag) {
        case 1:
            // setting
            showMenuLayer();
            break;
        case 2:
        {
            // sound
            bool soundOn = ud->getBoolForKey("soundOn", true);
            ud->setBoolForKey("soundOn", !soundOn);
            ud->flush();
            
            if (!soundOn) {
                // on
                m_soundItem->setSelectedIndex(0);
                BBAudio::setEffectOnOff(true);
            } else {
                // off
                m_soundItem->setSelectedIndex(1);
                BBAudio::setEffectOnOff(false);
            }
        }
            break;
        case 3:
        {
            // music
            bool musicOn = ud->getBoolForKey("musicOn", true);
            ud->setBoolForKey("musicOn", !musicOn);
            ud->flush();
            BBAudio::setMusicVolume(0);
            
            if (!musicOn) {
                // on
                m_musicItem->setSelectedIndex(0);
                BBAudio::setMusicVolume(true);
            } else {
                // off
                m_musicItem->setSelectedIndex(1);
                BBAudio::setMusicVolume(false);
            }
        }
            break;
        case 4:
            // close
            m_shadeLayer->removeFromParentAndCleanup(true);
            m_shadeLayer = NULL;

            break;
            
        default:
            break;
    }
    ud->flush();
}

void BBThemeLayer::nextPage()
{
    m_curPage = validPageValue(m_curPage + 1);
    float targetX;
    if (m_curPage == m_totalPages) {
        targetX = - (m_totalPages - 1)*m_picWidth;
    } else {
        targetX = - (m_curPage-1)*m_picWidth;
    }
    m_scrollViewContent->runAction(Sequence::create(MoveTo::create(.15, Point(targetX, m_scrollViewContent->getPositionY())), CallFunc::create([this](){
        m_themeBgSpt->setOpacity(255);;
    }), NULL));
}

void BBThemeLayer::prePage()
{
    m_curPage = validPageValue(m_curPage - 1);
    float targetX = -(m_curPage-1)*m_picWidth;
    
    m_scrollViewContent->runAction(Sequence::create(MoveTo::create(.15, Point(targetX, m_scrollViewContent->getPositionY())), CallFunc::create([this](){
        m_themeBgSpt->setOpacity(255);;
    }), NULL));
}

void BBThemeLayer::goToChooseLevel()
{
    
}