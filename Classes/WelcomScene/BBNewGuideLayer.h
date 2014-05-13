//
//  BBNewGuideLayer.h
//  BBSudoku
//
//  Created by FengZi on 14-4-2.
//
//

#ifndef __BBSudoku__BBNewGuideLayer__
#define __BBSudoku__BBNewGuideLayer__

#include "cocos2d.h"
USING_NS_CC;

class BBNewGuideLayer : public Layer
{
public:
    BBNewGuideLayer();
    ~BBNewGuideLayer();
    virtual bool init();
    CREATE_FUNC(BBNewGuideLayer);
    
public:
    virtual bool        onTouchBegan(Touch *touch, Event * event);
    virtual void        onTouchEnded(Touch *touch, Event * event);
    virtual void        onTouchCancelled(Touch *touch, Event * event);
    virtual void        onTouchMoved(Touch *touch, Event * event);

    
private:
    Size                m_visibleSize;
    Layer               *m_scrollViewContent;
    
    float               m_beginX;
    float               m_preX;
    int                 m_curPage;
    int                 m_totalPages;
    int                 m_picWidth;
    
private:
    void                initData();
    void                initSprite();
    void                menuBeginCallback(Ref* pSender);
    void                setContentView();
    int                 validPageValue(int value);
    void                nextPage();
    void                prePage();
};

#endif /* defined(__BBSudoku__BBNewGuideLayer__) */
