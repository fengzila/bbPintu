//
//  BBGameScene 2.cpp
//  BBFlycups
//
//  Created by FengZi on 14-4-10.
//
//

#include "BBGameScene.h"
#include "BBHandleLayer.h"
#include "BBGameUILayer.h"

BBGameScene::BBGameScene()
{
    
}

BBGameScene::~BBGameScene()
{
    
}

Scene* BBGameScene::createScene()
{
    auto scene = Scene::create();
    auto layer = BBGameScene::create();
    scene->addChild(layer);
    return scene;
}

bool BBGameScene::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    initData();
    return true;
}

#pragma mark - initData
void BBGameScene::initData()
{   
    auto handleLayer = BBHandleLayer::create();
    addChild(handleLayer);
    
//    auto uiLayer = BBGameUILayer::create();
//    uiLayer->setCascadeOpacityEnabled(true);
//    uiLayer->setOpacity(0);
//    uiLayer->runAction(Sequence::create(DelayTime::create(2), FadeIn::create(1), NULL));
//    addChild(uiLayer);
    
}