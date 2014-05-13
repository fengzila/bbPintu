//
//  GameCenterApi.h
//  puzzle
//
//  Created by FengZi on 14-2-21.
//
//

#ifndef __puzzle__GameCenterApi__
#define __puzzle__GameCenterApi__

#include <string>
using  std::string;

class GameCenterApi
{
public:
    GameCenterApi();
    ~GameCenterApi();
    static GameCenterApi* getInstance();
    void authenticateLocalUser();
    
    void showLeaderboard();
    void reportScore(int64_t score, const char* category);
    
    void goToAppStoreEvaluate();
    
    void showAdBanner();
    void hiddenAdBanner();
    
    void initStoreKit();
    bool isProcessPayments();
    void purchaseItem(const char* identifier);
};
#endif /* defined(__puzzle__GameCenterApi__) */
