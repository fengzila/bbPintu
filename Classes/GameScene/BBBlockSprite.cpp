//
//  BBBlockSprite.cpp
//  puzzle
//
//  Created by FengZi on 14-1-23.
//
//

#include "BBBlockSprite.h"

BBBlockSprite::BBBlockSprite()
{
    
}

BBBlockSprite::~BBBlockSprite()
{
    
}

BBBlockSprite* BBBlockSprite::create(const std::string& filename)
{
    BBBlockSprite *sprite = new BBBlockSprite();
    if (sprite && sprite->initWithFile(filename))
    {
        sprite->autorelease();
        return sprite;
    }
    CC_SAFE_DELETE(sprite);
    return nullptr;
}

void BBBlockSprite::setOrder(int order)
{
    m_order = order;
}

int BBBlockSprite::getOrder()
{
    return m_order;
}