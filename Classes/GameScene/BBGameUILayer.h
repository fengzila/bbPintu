//
//  BBGameUILayer.h
//  puzzle
//
//  Created by FengZi on 14-1-22.
//
//

#ifndef __puzzle__BBGameUILayer__
#define __puzzle__BBGameUILayer__

#include "cocos2d.h"
USING_NS_CC;

class BBGameUILayer : public Layer
{
public:
    BBGameUILayer();
    ~BBGameUILayer();
    virtual bool init();
    CREATE_FUNC(BBGameUILayer);
    
private:
    Size                m_visibleSize;
    Layer               *m_homeLayer;
    
private:
    void                initData();
    void                initSprite();
    void                showHomeLayer();
    void                menuBackCallback(Ref* pSender);
    void                removeHomeLayer();
    void                removeNode(Node* node);
};

#endif /* defined(__puzzle__BBGameUILayer__) */
