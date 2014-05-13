//
//  BBBlockSprite.h
//  puzzle
//
//  Created by FengZi on 14-1-23.
//
//

#ifndef __puzzle__BBBlockSprite__
#define __puzzle__BBBlockSprite__

#include "cocos2d.h"
USING_NS_CC;

class BBBlockSprite :public Sprite
{
public:
    BBBlockSprite();
    ~BBBlockSprite();
    static BBBlockSprite* create(const std::string& filename);
    
public:
    void            setOrder(int order);
    int             getOrder();
    
private:
    int             m_order;
};

#endif /* defined(__puzzle__BBBlockSprite__) */
