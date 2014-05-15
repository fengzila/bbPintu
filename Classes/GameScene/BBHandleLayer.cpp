//
//  BBHandleLayer.cpp
//  puzzle
//
//  Created by FengZi on 14-1-22.
//
//


#include "BBHandleLayer.h"
#include "GameCenterApi.h"
#include "BBGameDataManager.h"
#include "BBWelcomeScene.h"
#include "BBTmpScene.h"
#include "BBBlockSprite.h"
#include "BBAudio.h"

enum direction {
    kTop = 0,
    kRight = 1,
    kBottom = 2,
    kLeft = 3,
};

enum puzzel_type {
    k3X3 = 0,
    k4X4 = 1,
};

BBHandleLayer::BBHandleLayer()
{
    
}

BBHandleLayer::~BBHandleLayer()
{
    m_blockArr->release();
    NotificationCenter::getInstance()->removeObserver(this, "kRefreshUI");
}

bool BBHandleLayer::init()
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
void BBHandleLayer::initData()
{
    m_visibleSize = Director::getInstance()->getVisibleSize();

    GameCenterApi::getInstance()->showAdBanner();
    
    m_row = 3;
    m_col = 3;
    
    m_mixMaxTimes = 60;
    
    int levelId = BBGameDataManager::getInstance()->getLevelId();
    if (levelId > 6) {
        m_blockLength = 150;
        
        m_row = 4;
        m_col = 4;
        
        m_picPreix = k4X4;
        
        m_mixMaxTimes = 80;
    } else {
        m_blockLength = 200;
        m_picPreix = k3X3;
    }
    
    setScale(BBGameDataManager::getInstance()->getScreenScale());
    m_blockArr = Array::create();
    m_blockArr->retain();
    
    resetData();
}

void BBHandleLayer::resetData()
{
    m_showTips = false;
    m_canTouch = true;
    m_blockAnimOver = true;
    m_isUsingGoldFinger = false;
    m_isOver = false;
    m_hasTryAgain = false;
    
    m_curIndex = m_row * m_col + 1;
    
    m_mixTimes = m_mixMaxTimes;
    
    m_idleBlockLastDirection = 0;
    m_countDown = 3;
    
    m_goldFingerId1 = 0;
    m_goldFingerId2 = 0;
    
    m_time = 0;
}

#pragma mark - initSprite
void BBHandleLayer::initSprite()
{
//    BBAudio::playMusic(__String::createWithFormat("game_bg_0%d.mp3", arc4random()%6+1)->getCString());
    
    GameCenterApi::getInstance()->initStoreKit();
    
    m_testLayer = LayerColor::create(Color4B(255, 0, 0, 255), 10, 10);
    
    float screenScale = BBGameDataManager::getInstance()->getScreenScale();
    auto bgSpt = Sprite::create("bg_main.png");
    bgSpt->setPosition(Point(m_visibleSize.width/2, m_visibleSize.height/2));
    bgSpt->setScale(m_visibleSize.width/bgSpt->getContentSize().width/screenScale, m_visibleSize.height/bgSpt->getContentSize().height/screenScale);
    addChild(bgSpt);
    
    auto gridBgLayer = LayerColor::create(Color4B(255, 255, 255, 255), 620, 620);
    gridBgLayer->setPosition(Point(m_visibleSize.width/2 - gridBgLayer->getContentSize().width/2, m_visibleSize.height/2 - gridBgLayer->getContentSize().height/2 - 60));
    addChild(gridBgLayer);
    
    m_relativeX = gridBgLayer->getPositionX() + 10;
    m_relativeY = gridBgLayer->getPositionY() + 10;
    
    auto idleBlockBgLayer = LayerColor::create(Color4B(255, 255, 255, 255), m_blockLength + 20, m_blockLength);
    idleBlockBgLayer->setPosition(Point(0, gridBgLayer->getContentSize().height));
    gridBgLayer->addChild(idleBlockBgLayer);
    
    auto idleBlockInnerBgLayer = LayerColor::create(Color4B(181, 201, 209, 255), m_blockLength, m_blockLength - 10);
    idleBlockInnerBgLayer->setPosition(Point(10, gridBgLayer->getContentSize().height));
    gridBgLayer->addChild(idleBlockInnerBgLayer);
    
    m_blockContainer = Layer::create();
    m_blockContainer->setCascadeOpacityEnabled(true);
    addChild(m_blockContainer);
    
//    addChild(m_testLayer);
//    m_testLayer->setPosition(Point(m_relativeX, m_relativeY));
    
    createIdleBlock();
    
    createBlocks();
    
    createTipsLayer();
    
    createUI();
    
    createCountDown();
    
    schedule(schedule_selector(BBHandleLayer::update), 1);
    
    NotificationCenter::getInstance()->addObserver(this, callfuncO_selector(BBHandleLayer::refreshUI), "kRefreshUI", NULL);
}

void BBHandleLayer::createBlocks()
{
    int themeId = BBGameDataManager::getInstance()->getThemeId();
    int levelId = BBGameDataManager::getInstance()->getLevelId();
    Texture2D *texture = Director::getInstance()->getTextureCache()->addImage(String::createWithFormat("level_%d_%d.png", themeId, levelId)->getCString());
    int i = 0;
    for (int r = 1; r <= m_row; r++) {
        for (int c = 1; c <= m_col; c++) {
            auto frame = SpriteFrame::createWithTexture(texture, Rect(m_blockLength*(c - 1), m_blockLength*(m_row - r), m_blockLength, m_blockLength));
            
            auto blockSpt = BBBlockSprite::create(__String::createWithFormat("bg_block_%d.png", m_picPreix)->getCString());
            float blockX = m_relativeX + m_blockLength * (c - 1) + blockSpt->getContentSize().width/2;
            float blockY = m_relativeY +m_blockLength * (r - 1) + blockSpt->getContentSize().height/2;
            blockSpt->setPosition(Point(blockX, blockY));
            int index = computeIndex(blockX, blockY);
            blockSpt->setOrder(index);
            blockSpt->setTag(index);
            m_blockContainer->addChild(blockSpt);
            
            auto spt = Sprite::createWithSpriteFrame(frame);
            float scaleRate = (m_blockLength - 2.0f) / m_blockLength;
            spt->setScale(scaleRate);
            spt->setPosition(Point(blockSpt->getContentSize().width/2, blockSpt->getContentSize().height/2));
            blockSpt->addChild(spt);
            
            blockSpt->setCascadeOpacityEnabled(true);
            
            m_blockArr->addObject(blockSpt);
            
            
            auto listener = EventListenerTouchOneByOne::create();
            listener->setSwallowTouches(true);
            listener->onTouchBegan = CC_CALLBACK_2(BBHandleLayer::onTouchBegan, this);
            listener->onTouchMoved = CC_CALLBACK_2(BBHandleLayer::onTouchMoved, this);
            listener->onTouchEnded = CC_CALLBACK_2(BBHandleLayer::onTouchEnded, this);
            _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, blockSpt);
            
            i++;
        }
    }
}

void BBHandleLayer::createIdleBlock()
{
    auto idleBlockSpt = BBBlockSprite::create(__String::createWithFormat("IdleBlock_%d.png", m_picPreix)->getCString());
    
    float blockX = m_relativeX + m_blockLength * 0 + idleBlockSpt->getContentSize().width/2;
    float blockY = m_relativeY +m_blockLength * (m_row - 0) + idleBlockSpt->getContentSize().height/2;
    
    idleBlockSpt->setPosition(Point(blockX, blockY));
    int index = computeIndex(blockX, blockY);
    idleBlockSpt->setTag(index);
    idleBlockSpt->setOrder(index);
    m_blockContainer->addChild(idleBlockSpt);
    
    float scaleRate = (m_blockLength - 2.0f) / m_blockLength;
    idleBlockSpt->setScale(scaleRate);
    
    // 加入到集合末尾
    m_blockArr->addObject(idleBlockSpt);
    
    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = CC_CALLBACK_2(BBHandleLayer::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(BBHandleLayer::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(BBHandleLayer::onTouchEnded, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, idleBlockSpt);
}

void BBHandleLayer::createTipsLayer()
{
    m_tipsLayer = LayerColor::create(Color4B(255, 255, 255, 255), 395, 195);
    m_tipsLayer->setPosition(Point(m_relativeX + 215, m_relativeY +m_blockLength * m_row + 15));
    addChild(m_tipsLayer);
    
    float sptWidth = 180.0f;
    int themeId = BBGameDataManager::getInstance()->getThemeId();
    int levelId = BBGameDataManager::getInstance()->getLevelId();
    auto *spt = Sprite::create(__String::createWithFormat("level_%d_%d.png", themeId, levelId)->getCString());
    Point sptPos = Point(m_tipsLayer->getPositionX() + 30 + sptWidth/2, m_tipsLayer->getPositionY() + m_tipsLayer->getContentSize().height/2);
    spt->setPosition(sptPos);
    addChild(spt);
    
    float scaleRate = sptWidth / spt->getContentSize().width;
    spt->setScale(scaleRate);
    
    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = [](Touch* touch, Event* event){
        auto target = static_cast<Sprite*>(event->getCurrentTarget());//获取的当前触摸的目标
        Point locationInNode = target->convertToNodeSpace(touch->getLocation());
        Size s = target->getContentSize();
        Rect rect = Rect(0, 0, s.width, s.height);
        
        if (rect.containsPoint(locationInNode)) {
            return true;
        } else {
            return false;
        }
    };
    listener->onTouchMoved = [](Touch* touch, Event* event){};
    listener->onTouchEnded = [=](Touch* touch, Event* event){
        showBigTipsPic();
    };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, spt);
}

void BBHandleLayer::createUI()
{
    auto *optionItem = MenuItemImage::create(
                                             "btn_menu.png",
                                             "btn_menu.png",
                                             CC_CALLBACK_1(BBHandleLayer::menuBackCallback, this));
    optionItem->setTag(3);
    
    optionItem->setPosition(Point(m_tipsLayer->getContentSize().width * 3/4 - 0 - optionItem->getContentSize().width/2, m_tipsLayer->getContentSize().height/4));
    
    auto *fingerItem = MenuItemImage::create(
                                             "btn_ finger.png",
                                             "btn_ finger.png",
                                             CC_CALLBACK_1(BBHandleLayer::menuBackCallback, this));
    fingerItem->setTag(10);
    
    fingerItem->setPosition(Point(m_tipsLayer->getContentSize().width * 3/4 + 0 + optionItem->getContentSize().width/2, m_tipsLayer->getContentSize().height/4));
    
    auto fingerTipsBg = Sprite::create("bg_fingerTips.png");
    fingerTipsBg->setPosition(Point(fingerItem->getContentSize().width * 3/4, fingerTipsBg->getContentSize().height/2));
    fingerItem->addChild(fingerTipsBg);
    
    UserDefault* ud = UserDefault::getInstance();
    int fingerLeftTimes = ud->getIntegerForKey("fingerLeftTimes", 3);
    
    m_fingerLeftTimesLabel = LabelTTF::create(__String::createWithFormat("%d", fingerLeftTimes)->getCString(), "Helvetica-Bold", 25);
    m_fingerLeftTimesLabel->setColor(Color3B(255, 255, 255));
    m_fingerLeftTimesLabel->setPosition(Point(fingerTipsBg->getContentSize().width/2, fingerTipsBg->getContentSize().height/2));
    fingerTipsBg->addChild(m_fingerLeftTimesLabel);
    
    m_cancelGoldFingerSpt = Sprite::create("btn_close.png");
    m_cancelGoldFingerSpt->setPosition(Point(fingerItem->getContentSize().width/2, fingerItem->getContentSize().height/2));
    fingerItem->addChild(m_cancelGoldFingerSpt);
    m_cancelGoldFingerSpt->setOpacity(0);
    
    Menu *menu = Menu::create(optionItem, fingerItem, NULL);
    menu->setPosition(Point::ZERO);
    m_tipsLayer->addChild(menu);
    
    m_goldFingerFocusSpt = Sprite::create("bg_blockFocus.png");
    m_goldFingerFocusSpt->setOpacity(0);
    m_blockContainer->addChild(m_goldFingerFocusSpt);
    
    
    m_logoSpt = Sprite::create("logo.png");
    m_logoSpt->setPosition(Point(m_tipsLayer->getContentSize().width * 3/4, m_tipsLayer->getContentSize().height * 3/4));
    m_tipsLayer->addChild(m_logoSpt);
    
    m_timeLabel = LabelTTF::create("00:00", "Arial", 50);
    m_timeLabel->setColor(Color3B(79, 132, 174));
    m_timeLabel->setPosition(Point(m_tipsLayer->getContentSize().width * 3/4, m_tipsLayer->getContentSize().height * 3/4));
    m_tipsLayer->addChild(m_timeLabel);
    m_timeLabel->setScale(0, 1);
}

void BBHandleLayer::showMenuLayer()
{
    if (m_shadeLayer != NULL) {
        return;
    }
    m_shadeLayer = LayerColor::create(Color4B(0, 0, 0, 195), m_visibleSize.width, m_visibleSize.height);
    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = [](Touch* touch, Event* event){return true;};
    listener->onTouchMoved = [](Touch* touch, Event* event){};
    listener->onTouchEnded = [=](Touch* touch, Event* event){};
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, m_shadeLayer);
    
    addChild(m_shadeLayer);
    
    auto *resumeItem = MenuItemImage::create(
                                             "btn_resume.png",
                                             "btn_resume.png",
                                             CC_CALLBACK_1(BBHandleLayer::menuBackCallback, this));
    resumeItem->setPosition(Point(m_visibleSize.width/2 - resumeItem->getContentSize().width/2 - 100, m_visibleSize.height/2 + 200));
    resumeItem->setTag(4);
    
    auto *newItem = MenuItemImage::create(
                                          "btn_retry.png",
                                          "btn_retry.png",
                                          CC_CALLBACK_1(BBHandleLayer::menuBackCallback, this));
    newItem->setPosition(Point(m_visibleSize.width/2, m_visibleSize.height/2 + 200));
    newItem->setTag(5);
    
    auto *homeItem = MenuItemImage::create(
                                           "btn_home.png",
                                           "btn_home.png",
                                           CC_CALLBACK_1(BBHandleLayer::menuBackCallback, this));
    homeItem->setPosition(Point(m_visibleSize.width/2 + homeItem->getContentSize().width/2 + 100, m_visibleSize.height/2 + 200));
    homeItem->setTag(6);
    
    m_soundItem = MenuItemToggle::createWithCallback(
        CC_CALLBACK_1(BBHandleLayer::menuBackCallback, this),
        MenuItemImage::create("btn_sound_on.png", "btn_sound_high.png"),
        MenuItemImage::create("btn_sound_off.png", "btn_sound_high.png"),
        NULL
                                                   
    );
    m_soundItem->setPosition(Point(m_visibleSize.width/2 - m_soundItem->getContentSize().width/2 - 100, m_visibleSize.height/2));
    m_soundItem->setTag(7);
    
    UserDefault* ud = UserDefault::getInstance();
    if (ud->getBoolForKey("soundOn", true)) {
        // on
        m_soundItem->setSelectedIndex(0);
    } else {
        // off
        m_soundItem->setSelectedIndex(1);
    }
    
    m_musicItem = MenuItemToggle::createWithCallback(
        CC_CALLBACK_1(BBHandleLayer::menuBackCallback, this),
        MenuItemImage::create("btn_music_on.png", "btn_music_high.png"),
        MenuItemImage::create("btn_music_off.png", "btn_music_high.png"),
        NULL
                                                     
     );
    if (ud->getBoolForKey("musicOn", true)) {
        // on
        m_musicItem->setSelectedIndex(0);
    } else {
        // off
        m_musicItem->setSelectedIndex(1);
    }

    m_musicItem->setPosition(Point(m_visibleSize.width/2, m_visibleSize.height/2));
    m_musicItem->setTag(8);
    
    auto *rateItem = MenuItemImage::create(
                                           "btn_rate.png",
                                           "btn_rate.png",
                                           CC_CALLBACK_1(BBHandleLayer::menuBackCallback, this));
    rateItem->setPosition(Point(m_visibleSize.width/2 + rateItem->getContentSize().width/2 + 100, m_visibleSize.height/2));
    rateItem->setTag(9);
    
    Menu *menu = Menu::create(resumeItem, newItem, rateItem, homeItem, m_soundItem, m_musicItem, NULL);
    menu->setPosition(Point::ZERO);
    m_shadeLayer->addChild(menu);
    
}

void BBHandleLayer::removeMenuLayer()
{
    if (m_shadeLayer == NULL) {
        return;
    }
    m_shadeLayer->removeFromParentAndCleanup(true);
    m_shadeLayer = NULL;
}

void BBHandleLayer::createCountDown()
{
    m_shadeLayer = LayerColor::create(Color4B(0, 0, 0, 125), m_visibleSize.width, m_visibleSize.height);
    addChild(m_shadeLayer);
    
    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = [](Touch* touch, Event* event){return true;};
    listener->onTouchMoved = [](Touch* touch, Event* event){};
    listener->onTouchEnded = [=](Touch* touch, Event* event){};
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, m_shadeLayer);
    
    m_countDownLabel = LabelTTF::create("3", "Helvetica-Bold", 100);
    m_countDownLabel->setColor(Color3B(255, 255, 255));
    m_countDownLabel->setPosition(Point(m_visibleSize.width/2, m_visibleSize.height/2));
    m_shadeLayer->addChild(m_countDownLabel);
    
    auto bgSpt = Sprite::create("bg_countDown.png");
    bgSpt->setPosition(Point(m_visibleSize.width/2, m_visibleSize.height/2));
    m_shadeLayer->addChild(bgSpt);
    bgSpt->runAction(RepeatForever::create(RotateBy::create(1, 360)));
    
    BBAudio::playEffect("countDown.mp3");
}

void BBHandleLayer::update(float dt)
{
    if (m_isOver) {
        return;
    }
    
    m_time++;
    
    if (m_time == m_countDown) {
        if (!m_hasTryAgain) {
            removeMenuLayer();
            
            BBAudio::playEffect("go.mp3");
            
            // 打乱重排
            mixBlock();
        }
    } else if (m_time < m_countDown) {
        if (!m_hasTryAgain) {
            m_countDownLabel->setString(__String::createWithFormat("%d", m_countDown - m_time)->getCString());
            m_countDownLabel->setScale(0);
            m_countDownLabel->runAction(ScaleTo::create(.25, 1));
            BBAudio::playEffect("countDown.mp3");
        }
    } else if (m_time > m_countDown) {
        updateTimeLabel();
    }
}

void BBHandleLayer::updateTimeLabel()
{
    m_timeLabel->setString(formatTime(m_time)->getCString());
    
    if (m_time % 30 == 0) {
        m_logoSpt->runAction(ScaleTo::create(.35, 0, 1));
        m_timeLabel->runAction(Sequence::createWithTwoActions(ScaleTo::create(.35, 1, 1), CallFunc::create([this](){
            m_logoSpt->runAction(Sequence::createWithTwoActions(DelayTime::create(15), ScaleTo::create(.35, 1, 1)));
            m_timeLabel->runAction(Sequence::createWithTwoActions(DelayTime::create(15), ScaleTo::create(.35, 0, 1)));
        })));
    }
}

__String* BBHandleLayer::formatTime(float time)
{
    int d = floor(time / (24 * 60 * 60));
    int h = floor((time - d * 24 * 60 * 60) / 3600);
    int m = floor((time - h * 3600) / 60);
    int s = time - h * 3600 - m * 60;
    
    if (d > 0) {
        return __String::createWithFormat("%d:%02d:%02d:%02d", d, h, m, s);
    } else if (h > 0) {
        return __String::createWithFormat("%02d:%02d:%02d", h, m, s);
    } else if(m > 0) {
        return __String::createWithFormat("%02d:%02d", m, s);
    } else {
        return __String::createWithFormat("00:%02d", s);
    }
}

// 打乱重排
void BBHandleLayer::mixBlock()
{
    m_blockAnimOver = false;
    mixBlockOneByOne();
}

void BBHandleLayer::mixBlockOneByOne()
{
    BBAudio::playEffect("move.mp3");
    m_mixTimes--;
    if (m_mixTimes <= 0) {
        m_blockAnimOver = true;
        return;
    }
    
    auto idleSpt = (BBBlockSprite*)m_blockContainer->getChildByTag(m_curIndex);
        
    int targetIndex = 0;
    BBBlockSprite *targetSpt;
    
    __Array *indexArr = Array::create();
    indexArr->retain();
    
    __Array *directionArr = Array::create();
    directionArr->retain();
    
    int c = m_curIndex % m_col;
    int r = floor(m_curIndex / m_col) + 1;
    if (c == 0){
        c = m_col;
        r = r - 1;
    }
    
    for (int i = 1; i <= 4; i++) {
        switch (i) {
            case 1:
                if (m_idleBlockLastDirection == 3) {
                    continue;
                }
                if (c == 1) {
                    if (r == m_row + 1) {
                        continue;
                    }
                } else if (r == m_row) {
                    continue;
                }
                targetIndex = m_curIndex + m_col;
                
//                log("up");
                
                indexArr->addObject(Integer::create(targetIndex));
                directionArr->addObject(Integer::create(1));
                break;
            case 2:
                if (m_idleBlockLastDirection == 4) {
                    continue;
                }
                if (c == m_col || r == m_row + 1) {
                    continue;
                }
//                log("left");
                targetIndex = m_curIndex + 1;
                indexArr->addObject(Integer::create(targetIndex));
                directionArr->addObject(Integer::create(2));
                break;
            case 3:
                if (r != m_row + 1 && m_idleBlockLastDirection == 1) {
                    continue;
                }
                if (r == 1) {
                    continue;
                }
                targetIndex = m_curIndex - m_col;
                
//                log("down");
                
                indexArr->addObject(Integer::create(targetIndex));
                directionArr->addObject(Integer::create(3));
                break;
            case 4:
                if (m_idleBlockLastDirection == 2) {
                    continue;
                }
                if (c == 1) {
                    continue;
                }
//                log("right");
                targetIndex = m_curIndex - 1;
                
                indexArr->addObject(Integer::create(targetIndex));
                directionArr->addObject(Integer::create(4));
                break;
            default:
                break;
        }
    }
    
    if (indexArr->count() == 0) {
        m_blockAnimOver = true;
        m_mixTimes = 0;
        return;
    }
    int randIndex = (arc4random() % indexArr->count()) + 0;
    __Integer *index = (__Integer*)indexArr->getObjectAtIndex(randIndex);
    targetIndex = index->getValue();
    
    m_idleBlockLastDirection = ((__Integer*)directionArr->getObjectAtIndex(randIndex))->getValue();
    
    targetSpt = (BBBlockSprite*)m_blockContainer->getChildByTag(targetIndex);
    Point curPoint = idleSpt->getPosition();
    int curTag = idleSpt->getTag();
    idleSpt->setPosition(targetSpt->getPosition());
    idleSpt->setTag(targetSpt->getTag());
    targetSpt->runAction(Sequence::create(MoveTo::create(.015, curPoint), CallFunc::create([this](){
        mixBlockOneByOne();
    }), NULL));
    
    targetSpt->setTag(curTag);
    
    m_curIndex = targetIndex;
    
    indexArr->release();
    directionArr->release();
}

void BBHandleLayer::showBigTipsPic()
{
    m_shadeLayer = LayerColor::create(Color4B(0, 0, 0, 125), m_visibleSize.width, m_visibleSize.height);
    getParent()->addChild(m_shadeLayer);
    m_shadeLayer->setScale(BBGameDataManager::getInstance()->getScreenScale());

    float sptWidth = 180.0f;
    int themeId = BBGameDataManager::getInstance()->getThemeId();
    int levelId = BBGameDataManager::getInstance()->getLevelId();
    auto *spt = Sprite::create(__String::createWithFormat("level_%d_%d.png", themeId, levelId)->getCString());
    Point sptPos = Point(m_tipsLayer->getPositionX() + 30 + sptWidth/2, m_tipsLayer->getPositionY() + m_tipsLayer->getContentSize().height/2);
    spt->setPosition(sptPos);
    m_shadeLayer->addChild(spt);
    
    float scaleRate = sptWidth / spt->getContentSize().width;
    spt->setScale(scaleRate);
    
    spt->runAction(Spawn::create(MoveTo::create(.15, Point(m_relativeX+spt->getContentSize().width/2, m_relativeY+spt->getContentSize().height/2)), ScaleTo::create(.15, 1), NULL));
    
    auto *tipsTitleLabel = LabelTTF::create("Tap To Continue", "Arial", 40);
    tipsTitleLabel->setColor(Color3B(255, 255, 255));
    tipsTitleLabel->setPosition(Point(m_visibleSize.width/2, m_visibleSize.height/2 - 260));
    m_shadeLayer->addChild(tipsTitleLabel);
    
    tipsTitleLabel->runAction(RepeatForever::create(Sequence::create(FadeIn::create(.45), FadeOut::create(.65),DelayTime::create(.3), NULL)));
    
    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = [](Touch* touch, Event* event){
        return true;
    };
    listener->onTouchMoved = [](Touch* touch, Event* event){};
    listener->onTouchEnded = [=](Touch* touch, Event* event){
        spt->runAction(Sequence::create(Spawn::create(MoveTo::create(.15, sptPos), ScaleTo::create(.15, scaleRate), NULL), CallFunc::create([this](){
            removeMenuLayer();
        }), NULL));
    };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, m_shadeLayer);
}

void BBHandleLayer::refresh()
{
    if (!m_blockAnimOver) {
        return;
    }
    
    resetData();
    
    m_hasTryAgain = true;
    
    Ref *spt = NULL;
    CCARRAY_FOREACH(m_blockArr, spt)
    {
        auto blockSpt = (BBBlockSprite*)spt;
        int index = blockSpt->getOrder();
        
        blockSpt->setTag(index);
        
        int c = index % m_col;
        int r = floor(index / m_col) + 1;
        if (c == 0){
            c = m_col;
            r = r - 1;
        }
        
        float blockX = m_relativeX + m_blockLength * (c - 1) + blockSpt->getContentSize().width/2;
        float blockY = m_relativeY +m_blockLength * (r - 1) + blockSpt->getContentSize().height/2;
        blockSpt->setPosition(Point(blockX, blockY));
    }

//    mixBlock();
}

void BBHandleLayer::refreshUI(Ref *ref)
{
    UserDefault* ud = UserDefault::getInstance();
    int fingerLeftTimes = ud->getIntegerForKey("fingerLeftTimes", 3);
    
    m_fingerLeftTimesLabel->setString(__String::createWithFormat("%d", fingerLeftTimes)->getCString());
}

int BBHandleLayer::computeIndex(float x, float y)
{
    if (x < m_relativeX || x > m_relativeX + m_blockLength * (m_col + 0) || y < m_relativeY || y > m_relativeY + m_blockLength * (m_row + 1)) {
        return -1;
    }
    
    float realX = x - m_relativeX;
    float realY = y - m_relativeY;
    
    int index = ceil(realX / m_blockLength) + (ceil(realY / m_blockLength) - 1) * (m_col + 0);
    if (index < 1 || index > (m_row + 1) * m_col - (m_col - 1)) {
        index = -1;
    }
    return index;
}

bool BBHandleLayer::onTouchBegan(Touch *touch, Event * event)
{
    if (!m_blockAnimOver) {
        return false;
    }
    auto target = static_cast<Sprite*>(event->getCurrentTarget());//获取的当前触摸的目标
    Point locationInNode = target->convertToNodeSpace(touch->getLocation());
    Size s = target->getContentSize();
    Rect rect = Rect(0, 0, s.width, s.height);
    
    if (rect.containsPoint(locationInNode)) {
        //判断触摸点是否在目标的范围内
        if (m_isUsingGoldFinger) {
            // 正在使用金手指
            BBAudio::playEffect("select.mp3");
            
            UserDefault* ud = UserDefault::getInstance();
            int fingerLeftTimes = ud->getIntegerForKey("fingerLeftTimes", 3);
            if (m_goldFingerId1 == 0) {
                m_goldFingerId1 = ((BBBlockSprite*)target)->getTag();
                m_goldFingerFocusSpt->setOpacity(255);
                m_goldFingerFocusSpt->setPosition(target->getPosition());
            } else {
                if (((BBBlockSprite*)target)->getTag() == m_goldFingerId1) {
                    return false;
                }
                m_goldFingerId2 = ((BBBlockSprite*)target)->getTag();
                int index1 = m_goldFingerId1;
                int index2 = m_goldFingerId2;
                goldFingerEnd();
                exchangeBlock(index2, index1);
                
                ud->setIntegerForKey("fingerLeftTimes", fingerLeftTimes - 1);
                
                m_fingerLeftTimesLabel->setString(__String::createWithFormat("%d", fingerLeftTimes-1)->getCString());
                
                if (checkOver()) {
                    // 拼图成功 弹出结算面板
                    m_isOver = true;
                    showOverLayer();
                }
            }
            return false;
        }
        
        m_firstPoint = touch->getLocation();
        m_canTouch = true;
        return true;
    } else {
        return false;
    }
}

bool BBHandleLayer::checkOver()
{
    Ref *spt = NULL;
    CCARRAY_FOREACH(m_blockArr, spt)
    {
        auto blockSpt = (BBBlockSprite*)spt;
        int order = blockSpt->getOrder();
        
        if (order != blockSpt->getTag()) {
            return false;
        }
    }
    return true;
}

bool BBHandleLayer::checkIndex(int index)
{
    if (index < 1 || index > (m_row + 1) * m_col - (m_col - 1))
    {
        return false;
    }
    
    return true;
}

bool BBHandleLayer::isEffectiveTouchDistance(Touch *touch)
{
    Point pos = touch->getLocation();
    
    log("touch move len is %f", sqrt(pow(pos.x - m_firstPoint.x, 2) + pow(pos.y - m_firstPoint.y, 2)));
    if (sqrt(pow(pos.x - m_firstPoint.x, 2) + pow(pos.y - m_firstPoint.y, 2)) < 35) {
        // 手指滑动距离过短
        return false;
    }
    return true;
}

void BBHandleLayer::onTouchEnded(Touch *touch, Event * event)
{
    if (!isEffectiveTouchDistance(touch)) {
        // 手指滑动距离过短
        return;
    }
    
    BBAudio::playEffect("move.mp3");
    
    if (checkOver()) {
        // 拼图成功 弹出结算面板
        m_isOver = true;
        showOverLayer();
        return;
    }
}
void BBHandleLayer::onTouchCancelled(Touch *touch, Event * event)
{
}
void BBHandleLayer::onTouchMoved(Touch *touch, Event * event)
{
    if (!m_canTouch) {
        return;
    }
    moveBlock(touch, event);
}

void BBHandleLayer::exchangeBlock(int index1, int index2)
{
    auto curSpt = (BBBlockSprite*)m_blockContainer->getChildByTag(index1);
    auto targetSpt = (BBBlockSprite*)m_blockContainer->getChildByTag(index2);
    Point curPoint = curSpt->getPosition();
    curSpt->setPosition(targetSpt->getPosition());
    curSpt->setTag(index2);
    targetSpt->runAction(Sequence::create(MoveTo::create(.2, curPoint), CallFunc::create([this](){
        m_blockAnimOver = true;
    }), NULL));
    
    targetSpt->setTag(index1);
    
    int idleIndex = m_row * m_col + 1;
    if (curSpt->getOrder() == idleIndex) {
        m_curIndex = index2;
    } else if (targetSpt->getOrder() == idleIndex) {
        m_curIndex = index1;
    }
}

void BBHandleLayer::moveBlock(Touch *touch, Event * event)
{
    Point pos = touch->getLocation();
    
    if (!isEffectiveTouchDistance(touch)) {
        // 手指滑动距离过短
        return;
    }
    
    double angle = atan2(pos.x - m_firstPoint.x, pos.y - m_firstPoint.y) * 180 / 3.14;
    
    int targetIndex = 0;
    
    int c = m_curIndex % m_col;
    int r = floor(m_curIndex / m_col) + 1;
    if (c == 0){
        c = m_col;
        r = r - 1;
    }
    
    if (angle > -45 && angle < 45) {
        log("up");
        if (r == 1) {
            return;
        }
        targetIndex = m_curIndex - m_col;
        
    } else if (angle > 45 && angle < 135) {
        log("right");
        if (c == 1) {
            return;
        }
        targetIndex = m_curIndex - 1;
        
    } else if ((angle > 135 && angle < 180) || (angle > -180 && angle < -135)) {
        log("down");
        if (c == 1) {
            if (r == m_row + 1) {
                return;
            }
        } else if (r == m_row) {
            return;
        }
        targetIndex = m_curIndex + m_col;
    } else if (angle > -135 && angle < -45) {
        log("left");
        if (c == m_col) {
            return;
        }
        targetIndex = m_curIndex + 1;
    }
    
    if (!checkIndex(targetIndex)) {
        return;
    }
    
    exchangeBlock(m_curIndex, targetIndex);
    
    m_canTouch = false;
    m_blockAnimOver = false;
}

void BBHandleLayer::showOverLayer()
{
    int themeId = BBGameDataManager::getInstance()->getThemeId();
    int levelId = BBGameDataManager::getInstance()->getLevelId();
    
    BBAudio::playEffect("win.mp3");
    
    m_shadeLayer = LayerColor::create(Color4B(0, 0, 0, 75), m_visibleSize.width, m_visibleSize.height);
    getParent()->addChild(m_shadeLayer);
    m_shadeLayer->setScale(BBGameDataManager::getInstance()->getScreenScale());
    
    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = [](Touch* touch, Event* event){return true;};
    listener->onTouchMoved = [](Touch* touch, Event* event){};
    listener->onTouchEnded = [=](Touch* touch, Event* event){};
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, m_shadeLayer);
    
    int bestTime = BBGameDataManager::getInstance()->getBestTime(themeId, levelId);
    if (bestTime == 0) {
        bestTime = m_time;
    }
    
    if (bestTime >= m_time) {
        ParticleSystem *meteor1 = ParticleSystemQuad::create("boom.plist");
        meteor1->setPosition(Point(0, m_visibleSize.height));
        m_shadeLayer->addChild(meteor1);
        
        ParticleSystem *meteor2 = ParticleSystemQuad::create("boom.plist");
        meteor2->setPosition(Point(m_visibleSize.width, m_visibleSize.height));
        m_shadeLayer->addChild(meteor2);
    }
    
    Sprite *scoreBg = Sprite::create("over_bg.png");
    scoreBg->setScale(0);
    scoreBg->setPosition(Point(m_visibleSize.width/2, m_visibleSize.height/2));
    m_shadeLayer->addChild(scoreBg);
    
    Sprite *overBestSpt = Sprite::create("over_best.png");
    overBestSpt->setPosition(Point(scoreBg->getContentSize().width/2, scoreBg->getContentSize().height));
    overBestSpt->setOpacity(0);
    overBestSpt->setScale(15);
    scoreBg->addChild(overBestSpt);
    
    auto leftTimeLabel = LabelTTF::create(formatTime(m_time)->getCString(), "Arial", 40);
    leftTimeLabel->setColor(Color3B(75, 135, 180));
    leftTimeLabel->setAnchorPoint(Point(.5, .5));
    leftTimeLabel->setPosition(Point(scoreBg->getContentSize().width/2, scoreBg->getContentSize().height - leftTimeLabel->getContentSize().height/2 - 20));
    scoreBg->addChild(leftTimeLabel);
    
    auto bestLabel = LabelTTF::create(formatTime(bestTime)->getCString(), "Arial", 40);
    bestLabel->setColor(Color3B(75, 135, 180));
    bestLabel->setAnchorPoint(Point(0, .5));
    bestLabel->setPosition(Point(scoreBg->getContentSize().width/2 - 10, 50));
    scoreBg->addChild(bestLabel);
    
    MenuItemImage *resumeItem = MenuItemImage::create(
                          "btn_nextLevel.png",
                          "btn_nextLevel.png",
                          CC_CALLBACK_1(BBHandleLayer::menuBackCallback, this));
    
    resumeItem->setPosition(Point(scoreBg->getContentSize().width/2 + 70, -scoreBg->getContentSize().height/2 + 60));
    resumeItem->setTag(1);
    
    MenuItemImage *homeItem = MenuItemImage::create(
                                                   "btn_home.png",
                                                   "btn_home.png",
                                                   CC_CALLBACK_1(BBHandleLayer::menuBackCallback, this));
    homeItem->setPosition(Point(scoreBg->getContentSize().width/2 - 70, -scoreBg->getContentSize().height/2 + 60));
    homeItem->setTag(2);
    
    Menu *menu = Menu::create(resumeItem, homeItem, NULL);
    menu->setPosition(Point::ZERO);
    scoreBg->addChild(menu);
    
    float bgDelayTime = 2.0f;
    scoreBg->runAction(Sequence::create(DelayTime::create(bgDelayTime), ScaleTo::create(.15, 1.35), ScaleTo::create(.1, 1), NULL));
    overBestSpt->runAction(Sequence::create(DelayTime::create(.25 + bgDelayTime), FadeIn::create(.35), NULL));
    overBestSpt->runAction(Sequence::create(DelayTime::create(.25 + bgDelayTime), ScaleTo::create(.2, 1.0f), NULL));
    
    for (int i = 0; i < 3; i++) {
        Sprite *starSpt = Sprite::create("star_gray.png");
        
        starSpt->setPosition(Point(scoreBg->getContentSize().width/2 + (i-1)*starSpt->getContentSize().width, scoreBg->getContentSize().height/2 + 10));
        scoreBg->addChild(starSpt);
    }
    int star = 0;
    
    if (m_time < 120) {
        star = 3;
    } else if (m_time < 300) {
        star = 2;
    } else {
        star = 1;
    }
    BBGameDataManager::getInstance()->setGameOverData(star, m_time);
    
    for (int i = 0; i < star; i++) {
        Sprite *starSpt = Sprite::create("star.png");
        starSpt->setScale(5);
        starSpt->setOpacity(0);
        
        starSpt->setPosition(Point(scoreBg->getContentSize().width/2 + (i-1)*starSpt->getContentSize().width, scoreBg->getContentSize().height/2 + 10));
        starSpt->runAction(Sequence::create(DelayTime::create(bgDelayTime + 1 + i*0.45), Spawn::create(FadeIn::create(.25), ScaleTo::create(.25, 1), NULL), CallFunc::create([this](){
            BBAudio::playEffect("star.mp3");
        }), NULL));
        scoreBg->addChild(starSpt);
    }
}

void BBHandleLayer::menuBackCallback(Ref* pSender)
{
    BBAudio::playEffect("select.mp3");
    
    UserDefault* ud = UserDefault::getInstance();
    int tag = ((MenuItemLabel*)pSender)->getTag();
    switch (tag) {
        case 1:
            // next
            Director::getInstance()->replaceScene(BBTmpScene::createScene());
            break;
        case 2:
            // home
            Director::getInstance()->replaceScene(BBWelcomeScene::createScene());
            break;
        case 3:
            showMenuLayer();
            break;
        case 4:
            // 继续
            removeMenuLayer();
            break;
        case 5:
            // 重玩
            refresh();
            removeMenuLayer();
            break;
        case 6:
            // home
            Director::getInstance()->replaceScene(BBWelcomeScene::createScene());
            break;
        case 7:
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
        case 8:
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
        case 9:
            // 赞
            GameCenterApi::getInstance()->goToAppStoreEvaluate();
            break;
        case 10:
            // 金手指
            if (m_isUsingGoldFinger) {
                // 正在使用，则取消金手指
                goldFingerEnd();
            } else {
                int fingerLeftTimes = ud->getIntegerForKey("fingerLeftTimes", 3);
                if (fingerLeftTimes <= 0) {
                    GameCenterApi::getInstance()->purchaseItem("goldFingerX3");
                    break;
                }
                goldFingerBegin();
            }
            break;
            
        default:
            break;
    }
}

void BBHandleLayer::goldFingerBegin()
{
    m_cancelGoldFingerSpt->setOpacity(255);
    m_isUsingGoldFinger = true;
    
    Ref *spt = NULL;
    CCARRAY_FOREACH(m_blockArr, spt)
    {
        auto blockSpt = (BBBlockSprite*)spt;
        blockSpt->setScale(.985);
        blockSpt->runAction(RepeatForever::create(Sequence::create(RotateBy::create(.03, -2.5), RotateBy::create(.06, 5), RotateBy::create(.03, -2.5), NULL)));
    }
}

void BBHandleLayer::goldFingerEnd()
{
    m_isUsingGoldFinger = false;
    m_cancelGoldFingerSpt->setOpacity(0);
    m_goldFingerId1 = 0;
    m_goldFingerId2 = 0;
    
    m_goldFingerFocusSpt->setOpacity(0);
    
    Ref *spt = NULL;
    CCARRAY_FOREACH(m_blockArr, spt)
    {
        auto blockSpt = (BBBlockSprite*)spt;
        blockSpt->setScale(1);
        blockSpt->setRotation(0);
        blockSpt->stopAllActions();
    }
}
