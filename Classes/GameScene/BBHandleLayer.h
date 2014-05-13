//
//  BBHandleLayer.h
//  puzzle
//
//  Created by FengZi on 14-1-22.
//
//

#ifndef __puzzle__BBHandleLayer__
#define __puzzle__BBHandleLayer__

#include "cocos2d.h"

USING_NS_CC;

class BBHandleLayer : public Layer
{
public:
    BBHandleLayer();
    ~BBHandleLayer();
    virtual bool init();
    CREATE_FUNC(BBHandleLayer);
    
public:
    virtual bool        onTouchBegan(Touch *touch, Event * event);
    virtual void        onTouchEnded(Touch *touch, Event * event);
    virtual void        onTouchCancelled(Touch *touch, Event * event);
    virtual void        onTouchMoved(Touch *touch, Event * event);
    
private:
    EventListenerTouchOneByOne* m_listener;
    Size                m_visibleSize;
    int                 m_blockLength;
    int                 m_textureLength;
    __Array             *m_blockArr;            // 拼图格子对象集合
    int                 m_row;                  // 行数
    int                 m_col;                  // 列数
    float               m_relativeX;            // 所有格子的相对原点坐标
    float               m_relativeY;            // 所有格子的相对原点坐标
    int                 m_countDown;
    int                 m_time;
    int                 m_mixTimes;
    int                 m_mixMaxTimes;
    int                 m_idleBlockLastDirection;
    int                 m_picPreix;
    int                 m_goldFingerId1;
    int                 m_goldFingerId2;
    
    bool                m_showTips;
    bool                m_isOver;
    bool                m_hasTryAgain;
    Point               m_firstPoint;
    int                 m_curIndex;
    bool                m_canTouch;
    bool                m_blockAnimOver;
    LabelTTF            *m_fingerLeftTimesLabel;
    LabelTTF            *m_timeLabel;
    bool                m_isUsingGoldFinger;
    Sprite              *m_logoSpt;
    
    Sprite              *m_grid_bg;
    LayerColor          *m_testLayer;
    LayerColor          *m_tipsLayer;
    LayerColor          *m_shadeLayer;
    LabelTTF            *m_countDownLabel;
    
    Layer               *m_blockContainer;
    Sprite              *m_cancelGoldFingerSpt;
    Sprite              *m_goldFingerFocusSpt;
    
    MenuItemToggle      *m_soundItem;
    MenuItemToggle      *m_musicItem;
    
private:
    void                initData();
    void                initSprite();
    void                resetData();
    
    void                createBlocks();
    void                createIdleBlock();
    void                createTipsLayer();
    void                createUI();
    void                showMenuLayer();
    void                removeMenuLayer();
    int                 computeIndex(float x, float y);
    bool                checkMove(Point pos);
    int                 getNearByIdleIndex(int index);
    bool                checkOver();
    void                showOverLayer();
    bool                isEffectiveTouchDistance(Touch *touch);
    void                moveBlock(Touch *touch, Event * event);
    bool                checkIndex(int index);
    void                mixBlock();
    void                mixBlockOneByOne();
    void                update(float dt);
    void                updateTimeLabel();
    void                refresh(Ref* obj);
    void                createCountDown();
    void                showBigTipsPic();
    void                menuBackCallback(Ref* pSender);
    void                goldFingerBegin();
    void                goldFingerEnd();
    void                exchangeBlock(int index1, int index2);
    __String            *formatTime(float time);
};

#endif /* defined(__puzzle__BBHandleLayer__) */
