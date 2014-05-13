//
//  BBWelcomeScene.h
//  puzzle
//
//  Created by FengZi on 14-1-22.
//
//

#ifndef __puzzle__BBWelcomeScene__
#define __puzzle__BBWelcomeScene__

#include "cocos2d.h"
USING_NS_CC;

#include "extensions/cocos-ext.h"
USING_NS_CC_EXT;

class BBWelcomeScene : public Layer
{
public:
    BBWelcomeScene();
    ~BBWelcomeScene();
    static Scene* createScene();
    virtual bool init();
    CREATE_FUNC(BBWelcomeScene);
    
private:
    Layer   *m_cloudLayer;
    LayerColor  *m_shadeLayer;
    
private:
    void    initData();
    void    initSprite();
    void    createCloud1();
    void    createCloud2();
    void    menuBackCallback(Ref* pSender);
};

#endif /* defined(__puzzle__BBWelcomeScene__) */
