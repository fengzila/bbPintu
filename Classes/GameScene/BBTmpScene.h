//
//  BBTmpScene.h
//  BBFlycups
//
//  Created by FengZi on 14-4-10.
//
//

#ifndef __BBFlycups__BBTmpScene__
#define __BBFlycups__BBTmpScene__

#include "cocos2d.h"
USING_NS_CC;

class BBTmpScene : public Layer
{
public:
    BBTmpScene();
    ~BBTmpScene();
    static Scene* createScene();
    virtual bool init();
    CREATE_FUNC(BBTmpScene);
    
private:
    void    initData();
    void    goGmaeScene(float dt);
};

#endif /* defined(__BBFlycups__BBTmpScene__) */
