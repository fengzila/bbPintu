//
//  BBThemeLayer.h
//  BBSudoku
//
//  Created by FengZi on 14-4-2.
//
//

#ifndef __BBSudoku__BBThemeLayer__
#define __BBSudoku__BBThemeLayer__

#include "cocos2d.h"
USING_NS_CC;

class BBThemeLayer : public Layer
{
public:
    BBThemeLayer();
    ~BBThemeLayer();
    virtual bool init();
    CREATE_FUNC(BBThemeLayer);
    
public:
    virtual bool        onTouchBegan(Touch *touch, Event * event);
    virtual void        onTouchEnded(Touch *touch, Event * event);
    virtual void        onTouchCancelled(Touch *touch, Event * event);
    virtual void        onTouchMoved(Touch *touch, Event * event);
    
    virtual bool        themeOnTouchBegan(Touch *touch, Event * event);
    virtual void        themeOnTouchEnded(Touch *touch, Event * event);
    virtual void        themeOnTouchCancelled(Touch *touch, Event * event);
    virtual void        themeOnTouchMoved(Touch *touch, Event * event);
    
private:
    Size                m_visibleSize;
    Layer               *m_scrollViewContent;
    Sprite              *m_loadingLayer;
    
    float               m_beginX;
    float               m_preX;
    int                 m_curPage;
    int                 m_totalPages;
    int                 m_picWidth;
    Sprite*             m_themeBgSpt;
    __Array             *m_pageArr;
    LayerColor          *m_shadeLayer;
    
    MenuItemToggle      *m_soundItem;
    MenuItemToggle      *m_musicItem;
    
private:
    void                initData();
    void                initSprite();
    void                menuCallback(Ref* pSender);
    void                setContentView();
    int                 validPageValue(int value);
    void                nextPage();
    void                prePage();
    void                goToChooseLevel();
    void                createPageControl();
    void                refreshPageControl();
    void                addLevelLayer(Ref* ref);
    void                showMenuLayer();
};

#endif /* defined(__BBSudoku__BBThemeLayer__) */
