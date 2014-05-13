//
//  BBLevelLayer.h
//  BBSudoku
//
//  Created by FengZi on 14-4-2.
//
//

#ifndef __BBSudoku__BBLevelLayer__
#define __BBSudoku__BBLevelLayer__

#include "cocos2d.h"
USING_NS_CC;

class BBLevelLayer : public Layer
{
public:
    BBLevelLayer();
    ~BBLevelLayer();
    virtual bool init(int themeId);
    static BBLevelLayer* create(int themeId);
    
public:
    virtual bool        onTouchBegan(Touch *touch, Event * event);
    virtual void        onTouchEnded(Touch *touch, Event * event);
    virtual void        onTouchCancelled(Touch *touch, Event * event);
    virtual void        onTouchMoved(Touch *touch, Event * event);
    
    virtual bool        levelOnTouchBegan(Touch *touch, Event * event);
    virtual void        levelOnTouchEnded(Touch *touch, Event * event);
    virtual void        levelOnTouchCancelled(Touch *touch, Event * event);
    virtual void        levelOnTouchMoved(Touch *touch, Event * event);
    
    
private:
    Size                m_visibleSize;
    Layer               *m_scrollViewContent;
    
    float               m_beginX;
    float               m_preX;
    int                 m_curPage;
    int                 m_totalPages;
    int                 m_loadingNum;
    int                 m_picWidth;
    int                 m_themeId;
    int                 m_scaleWidth;
    __Array             *m_pageArr;
    Sprite              *m_loadingSpt;
    Sprite              *m_lockSpt;
    Menu                *m_playMenu;
    
private:
    void                initData(int themeId);
    void                initSprite();
    void                loadingCallback(Ref* ref);
    void                createSptList();
    void                menuBeginCallback(Ref* pSender);
    void                setContentView();
    int                 validPageValue(int value);
    void                nextPage();
    void                prePage();
    void                createPageControl();
    void                refreshPageControl();
};

#endif /* defined(__BBSudoku__BBLevelLayer__) */
