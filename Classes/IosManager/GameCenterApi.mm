//
//  GameCenterApi.mm
//  puzzle
//
//  Created by FengZi on 14-2-21.
//
//

#include "GameCenterApi.h"

#include <iostream>
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
#include "GameCenterManager.h"
#include "AdBannerManager.h"
#endif

static GameCenterApi *instance = NULL;

GameCenterApi::GameCenterApi()
{
    
}

GameCenterApi::~GameCenterApi()
{
    
}

GameCenterApi* GameCenterApi::getInstance()
{
    if (!instance) {
        instance = new GameCenterApi();
    }
    return instance;
}

void GameCenterApi::authenticateLocalUser()
{
    #if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    [[GameCenterManager getInstance] authenticateLocalUser];
    #endif
}

void GameCenterApi::showLeaderboard()
{
    #if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    [[GameCenterManager getInstance] showLeaderboard];
    #endif
}

void GameCenterApi::reportScore(int64_t score, const char* category)
{
    #if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    [[GameCenterManager getInstance] reportScore:score forCategory:[NSString stringWithUTF8String:category]];
    #endif
}

void GameCenterApi::goToAppStoreEvaluate()
{
    #if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    [GameCenterManager goToAppStoreEvaluate];
    #endif
}

void GameCenterApi::showAdBanner()
{
    #if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    [AdBannerManager show];
    #endif
}

void GameCenterApi::hiddenAdBanner()
{
    #if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    [AdBannerManager hiden];
    #endif
}

void GameCenterApi::initStoreKit()
{
    [[GameCenterManager getInstance] initStoreKit];
}

bool GameCenterApi::isProcessPayments()
{
    return [[GameCenterManager getInstance] isProcessPayments];
}

void GameCenterApi::purchaseItem(const char* identifier)
{
    [[GameCenterManager getInstance] purchaseItem:[NSString stringWithUTF8String:identifier]];
}