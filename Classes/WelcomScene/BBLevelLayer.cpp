//
//  BBLevelLayer.cpp
//  BBSudoku
//
//  Created by FengZi on 14-4-2.
//
//

#include "BBLevelLayer.h"
#include "BBGameScene.h"
#include "GameCenterApi.h"
#include "BBGameDataManager.h"
#include "BBAudio.h"

BBLevelLayer::BBLevelLayer()
{
    
}

BBLevelLayer::~BBLevelLayer()
{
    m_pageArr->release();
}

BBLevelLayer* BBLevelLayer::create(int themeId)
{
    BBLevelLayer* pRet = new BBLevelLayer();
    if (pRet && pRet->init(themeId)){
        pRet->autorelease();
    }else{
        CC_SAFE_DELETE(pRet);
    }
    return pRet;
}

bool BBLevelLayer::init(int themeId)
{
    if ( !Layer::init() )
    {
        return false;
    }
    initData(themeId);
    initSprite();
    return true;
}

#pragma mark - initData
void BBLevelLayer::initData(int themeId)
{
    m_visibleSize = Director::getInstance()->getVisibleSize();
    
    m_themeId = themeId;
    // 最大支持20个关卡
    m_totalPages = 9;
    m_loadingNum = 0;
    m_picWidth = m_visibleSize.width;
    
    m_pageArr = __Array::create();
    m_pageArr->retain();
    
    m_curPage = 1;
    int levelId = BBGameDataManager::getInstance()->getLevelId();
    if (levelId == 0) {
        levelId = UserDefault::getInstance()->getIntegerForKey("levelId", 1);
    }
    int curThemeId = BBGameDataManager::getInstance()->getThemeId();
    if (curThemeId == 0) {
        curThemeId = UserDefault::getInstance()->getIntegerForKey("themeId", 1);
    }

    if (curThemeId == m_themeId && levelId > 0) {
        m_curPage = validPageValue(levelId);
    }
    
    if (BBGameDataManager::getInstance()->getScreenScale() == 1) {
        m_scaleWidth = 500;
    } else {
        m_scaleWidth = 1000;
    }
}

void BBLevelLayer::initSprite()
{
    if (BBGameDataManager::getInstance()->getScreenScale() == 2) {
        setScale(.5);
    }
    GameCenterApi::getInstance()->hiddenAdBanner();
    
    auto bgSpt = Sprite::create("bg_main1.png");
    bgSpt->setPosition(Point(m_visibleSize.width/2, m_visibleSize.height/2));
    bgSpt->setScale(m_visibleSize.width/bgSpt->getContentSize().width, m_visibleSize.height/bgSpt->getContentSize().height);
    addChild(bgSpt);
    
    auto bgLogoSpt = Sprite::create("bg_logo.png");
    bgLogoSpt->setScale(BBGameDataManager::getInstance()->getScreenScale());
    bgLogoSpt->setPosition(Point(m_visibleSize.width/2, m_visibleSize.height/2 + 100));
    addChild(bgLogoSpt);
    
    m_loadingSpt = Sprite::create("loading.png");
    m_loadingSpt->setScale(2.0);
    m_loadingSpt->setPosition(Point(m_visibleSize.width/2, m_visibleSize.height/2 - 50));
    m_loadingSpt->runAction(RepeatForever::create(RotateBy::create(1, -360)));
    addChild(m_loadingSpt);
    
    m_scrollViewContent = Layer::create();
    m_scrollViewContent->runAction(MoveTo::create(.15, Point(- (m_curPage-1)*m_picWidth, 0)));
//    m_scrollViewContent->setPosition(Point(- (m_curPage-1)*m_picWidth, 0));
    addChild(m_scrollViewContent);
    
    auto topBgLayer = LayerColor::create(Color4B(0, 0, 0, 125), m_visibleSize.width, 100);
    topBgLayer->setPosition(Point(0, m_visibleSize.height - topBgLayer->getContentSize().height));
    addChild(topBgLayer);
    
    for (int i = 1; i <= m_totalPages; i++) {
        Director::getInstance()->getTextureCache()->addImageAsync(__String::createWithFormat(String::createWithFormat("level_%d_%d.png", m_themeId, i)->getCString(), m_curPage)->getCString(), CC_CALLBACK_1(BBLevelLayer::loadingCallback, this));
    }
}

void BBLevelLayer::loadingCallback(Ref* ref)
{
    m_loadingNum++;
    if (m_loadingNum == m_totalPages) {
        m_loadingSpt->removeFromParentAndCleanup(true);
        createSptList();
        createPageControl();
        refreshPageControl();
        
        auto listener = EventListenerTouchOneByOne::create();
        listener->setSwallowTouches(true);
        listener->onTouchBegan = CC_CALLBACK_2(BBLevelLayer::onTouchBegan, this);
        listener->onTouchMoved = CC_CALLBACK_2(BBLevelLayer::onTouchMoved, this);
        listener->onTouchEnded = CC_CALLBACK_2(BBLevelLayer::onTouchEnded, this);
        _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, m_scrollViewContent);
    }
}

void BBLevelLayer::createSptList()
{
    auto *backItem = MenuItemImage::create(
                                           "btn_back.png",
                                           "btn_back_light.png",
                                           CC_CALLBACK_1(BBLevelLayer::menuBeginCallback, this));
    backItem->setPosition(Point(50, m_visibleSize.height - 50));
    backItem->setTag(1);
    
    auto menu = Menu::create(backItem, NULL);
    menu->setPosition(Point::ZERO);
    addChild(menu);
    
    for (int i = 1; i <= m_totalPages; i++) {
        //背景
        Sprite *spt = Sprite::create(String::createWithFormat("level_%d_%d.png", m_themeId, i)->getCString());
        spt->setScale(m_scaleWidth/spt->getContentSize().width);
        spt->setPosition(Point(m_visibleSize.width/2 + m_picWidth*(i-1), m_visibleSize.height/2));
        m_scrollViewContent->addChild(spt);
        
        auto skinSpt = Sprite::create("skin_level.png");
        skinSpt->setScale(BBGameDataManager::getInstance()->getScreenScale());
        skinSpt->setPosition(Point(m_visibleSize.width/2 + m_picWidth*(i-1), m_visibleSize.height/2));
        m_scrollViewContent->addChild(skinSpt);
        
        auto pageSpt = Sprite::create("level_page.png");
        pageSpt->setPosition(Point(skinSpt->getContentSize().width * .255, skinSpt->getContentSize().height - pageSpt->getContentSize().height/2));
        skinSpt->addChild(pageSpt);
        
        auto pageLabel = LabelTTF::create(__String::createWithFormat("%d", i)->getCString(), "Helvetica-Bold", 60);
        pageLabel->setColor(Color3B(255, 255, 255));
        pageLabel->setPosition(Point(pageSpt->getContentSize().width/2, pageSpt->getContentSize().height/2));
        pageSpt->addChild(pageLabel);
        
        UserDefault* ud = UserDefault::getInstance();
        int themeIdFramCache = ud->getIntegerForKey("themeId", 1);
        int levelIdFramCache = ud->getIntegerForKey("levelId", 1);
        
        if (m_themeId > themeIdFramCache || (m_themeId == themeIdFramCache && i > levelIdFramCache)) {
            auto lockSpt = Sprite::create("lock.png");
            lockSpt->setPosition(Point(skinSpt->getContentSize().width * .825, skinSpt->getContentSize().height * .145));
            skinSpt->addChild(lockSpt);
        } else {
            auto listener1 = EventListenerTouchOneByOne::create();
            listener1->setSwallowTouches(false);
            listener1->onTouchBegan = CC_CALLBACK_2(BBLevelLayer::levelOnTouchBegan, this);
            listener1->onTouchMoved = CC_CALLBACK_2(BBLevelLayer::levelOnTouchMoved, this);
            listener1->onTouchEnded = CC_CALLBACK_2(BBLevelLayer::levelOnTouchEnded, this);
            _eventDispatcher->addEventListenerWithSceneGraphPriority(listener1, spt);
            
            int star = BBGameDataManager::getInstance()->getStar(m_themeId, i);
            for (int star_i = 0; star_i < 3; star_i++) {
                Sprite *starSpt;
                if (star_i <= star - 1) {
                    starSpt = Sprite::create("star.png");
                } else {
                    starSpt = Sprite::create("star_gray.png");
                }
                
                starSpt->setPosition(Point(skinSpt->getContentSize().width * .5 + starSpt->getContentSize().width * star_i, skinSpt->getContentSize().height * .135));
                skinSpt->addChild(starSpt);
            }
        }
    }
}

void BBLevelLayer::createPageControl()
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
    
    auto *btnPlaySpt = MenuItemImage::create(
                                             "btn_play.png",
                                             "btn_play_light.png",
                                             CC_CALLBACK_1(BBLevelLayer::menuBeginCallback, this));
    btnPlaySpt->setPosition(Point(m_visibleSize.width * .5, m_visibleSize.width * .205));
    btnPlaySpt->setScale(BBGameDataManager::getInstance()->getScreenScale());
    btnPlaySpt->setTag(2);
    
    m_playMenu = Menu::create(btnPlaySpt, NULL);
    m_playMenu->setPosition(Point::ZERO);
    addChild(m_playMenu);
    
    m_lockSpt = Sprite::create("btn_lock.png");
    m_lockSpt->setPosition(Point(m_visibleSize.width * .5, m_visibleSize.width * .205));
    m_lockSpt->setScale(BBGameDataManager::getInstance()->getScreenScale());
    addChild(m_lockSpt);

}

void BBLevelLayer::refreshPageControl()
{
    for (int i = 0; i < m_pageArr->count(); i++) {
        auto pageSpt = (Sprite*)m_pageArr->getObjectAtIndex(i);
        if (i+1 == m_curPage) {
            pageSpt->setOpacity(255);
        } else {
            pageSpt->setOpacity(125);
        }
    }
    
    UserDefault* ud = UserDefault::getInstance();
    int themeIdFramCache = ud->getIntegerForKey("themeId", 1);
    int levelIdFramCache = ud->getIntegerForKey("levelId", 1);
    if (m_themeId > themeIdFramCache || (m_themeId == themeIdFramCache && m_curPage > levelIdFramCache)) {
        m_playMenu->setOpacity(0);
        m_lockSpt->setOpacity(255);
    } else {
        m_playMenu->setOpacity(255);
        m_lockSpt->setOpacity(0);
    }
}

bool BBLevelLayer::onTouchBegan(Touch *touch, Event * event)
{
    Point location = Director::getInstance()->convertToGL(touch->getLocation());
    
    m_beginX = location.x;
    m_preX = location.x;
    return true;
}

void BBLevelLayer::onTouchMoved(Touch *touch, Event * event)
{
    Point location = Director::getInstance()->convertToGL(touch->getLocation());
    
    m_scrollViewContent->setPositionX(m_scrollViewContent->getPositionX() + location.x - m_preX);
    
    m_preX = location.x;
}

void BBLevelLayer::onTouchEnded(Touch *touch, Event * event)
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
    
    refreshPageControl();
    
    BBAudio::playEffect("menuSelect.mp3");
}

void BBLevelLayer::onTouchCancelled(Touch *touch, Event * event)
{
    
}

bool BBLevelLayer::levelOnTouchBegan(Touch *touch, Event * event)
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

void BBLevelLayer::levelOnTouchMoved(Touch *touch, Event * event)
{}

void BBLevelLayer::levelOnTouchEnded(Touch *touch, Event * event)
{
    
    Point location = Director::getInstance()->convertToGL(touch->getLocation());
    
    if (abs(location.x - m_beginX) < 20) {
        // 手指滑动距离过短
        
        // 原位
        m_scrollViewContent->setPositionX(- (m_curPage-1)*m_picWidth);
        
        BBGameDataManager::getInstance()->setThemeIdAndLevelId(m_themeId, m_curPage);
        
        BBAudio::stopMusic();
        
        Director::getInstance()->replaceScene(BBGameScene::createScene());
    }
}

void BBLevelLayer::levelOnTouchCancelled(Touch *touch, Event * event)
{}

int BBLevelLayer::validPageValue(int value)
{
    if ( value == 0 ) value = 1;
    if ( value == m_totalPages + 1 ) value = m_totalPages;
    
    return value;
}

void BBLevelLayer::menuBeginCallback(Ref* pSender)
{
    BBAudio::playEffect("select.mp3");
    
    int tag = ((MenuItemLabel*)pSender)->getTag();
    UserDefault *ud = UserDefault::getInstance();
    switch (tag) {
        case 1:
            this->removeFromParentAndCleanup(true);
            break;
        case 2:
            BBAudio::stopMusic();
            
            BBGameDataManager::getInstance()->setThemeIdAndLevelId(m_themeId, m_curPage);
            
            Director::getInstance()->replaceScene(BBGameScene::createScene());
            break;
        default:
            break;
    }
    ud->flush();
}

void BBLevelLayer::nextPage()
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

void BBLevelLayer::prePage()
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