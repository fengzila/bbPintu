//
//  BBGameDataManager.h
//  puzzle
//
//  Created by FengZi on 14-2-26.
//
//

#ifndef __puzzle__BBGameDataManager__
#define __puzzle__BBGameDataManager__

#include <iostream>
#include "cocos2d.h"
USING_NS_CC;

class BBGameDataManager {
public:
    static BBGameDataManager* getInstance();
    inline int getScreenScale(){return m_screenScale;}
    inline void setScreenScale(int scale){m_screenScale = scale;}
    inline int getThemeId(){return m_themeId;}
    inline int getLevelId(){return m_levelId;}
    void setThemeIdAndLevelId(int themeId, int levelId);
    void addCanUseTipsCount();
    void setGameOverData(int star, int leftTime);
    int getBestTime(int themeId, int levelId);
    int getStar(int themeId, int levelId);
    
private:
    int m_screenScale;
    int m_themeId;
    int m_levelId;
    
private:
    void initData();
};
#endif /* defined(__puzzle__BBGameDataManager__) */
