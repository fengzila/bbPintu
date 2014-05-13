//
//  BBGameScene 2.h
//  BBFlycups
//
//  Created by FengZi on 14-4-10.
//
//

#ifndef __BBFlycups__BBGameScene__
#define __BBFlycups__BBGameScene__

#include "cocos2d.h"
USING_NS_CC;

class BBGameScene : public Layer
{
public:
    BBGameScene();
    ~BBGameScene();
    static Scene* createScene();
    virtual bool init();
    CREATE_FUNC(BBGameScene);
    
private:
    void    initData();
};

#endif /* defined(__BBFlycups__BBGameScene__) */
