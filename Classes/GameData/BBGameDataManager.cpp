//
//  BBGameDataManager.cpp
//  puzzle
//
//  Created by FengZi on 14-2-26.
//
//

#include "BBGameDataManager.h"

static BBGameDataManager *instance = NULL;

BBGameDataManager* BBGameDataManager::getInstance()
{
    if (instance == NULL) {
        instance = new BBGameDataManager();
        instance->initData();
    }
    
    return instance;
}

void BBGameDataManager::initData()
{   
    m_screenScale = 1;
    m_themeId = 0;
    m_levelId = 0;
}

void BBGameDataManager::setThemeIdAndLevelId(int themeId, int levelId)
{
    m_themeId = themeId;
    m_levelId = levelId;
}

void BBGameDataManager::addCanUseTipsCount()
{
    UserDefault* ud = UserDefault::getInstance();
    ud->setIntegerForKey("fingerLeftTimes", 3);
    ud->flush();
}

void BBGameDataManager::setGameOverData(int star, int leftTime)
{
    int themeId = m_themeId;
    int levelId = m_levelId + 1;
    
    UserDefault* ud = UserDefault::getInstance();
    // 更新星级
    __String *starkey = __String::createWithFormat("star_%d_%d", m_themeId, m_levelId);
    int curStar = ud->getIntegerForKey(starkey->getCString(), 0);
    if (curStar < star) {
        ud->setIntegerForKey(starkey->getCString(), star);
    }
    
    __String *bestkey = __String::createWithFormat("best_%d_%d", m_themeId, m_levelId);
    int bestTime = ud->getIntegerForKey(bestkey->getCString(), 0);
    if (bestTime == 0 || bestTime > leftTime) {
        ud->setIntegerForKey(bestkey->getCString(), leftTime);
    }
    
    // 更新关卡进度
    int themeIdFramCache = ud->getIntegerForKey("themeId", 1);
    int levelIdFramCache = ud->getIntegerForKey("levelId", 1);
    
    if (themeIdFramCache > themeId) {
        return;
    } else if (themeIdFramCache == themeId) {
        if (levelIdFramCache >= levelId) {
            return;
        }
    }
    
    if (levelId > 9) {
        // 进入下一个主题
        themeId++;
        levelId = 1;
    }
    
    ud->setIntegerForKey("themeId", themeId);
    ud->setIntegerForKey("levelId", levelId);
    
    ud->flush();

}

int BBGameDataManager::getBestTime(int themeId, int levelId)
{
    UserDefault* ud = UserDefault::getInstance();
    
    __String *bestkey = __String::createWithFormat("best_%d_%d", themeId, levelId);
    return ud->getIntegerForKey(bestkey->getCString(), 0);
}

int BBGameDataManager::getStar(int themeId, int levelId)
{
    UserDefault* ud = UserDefault::getInstance();
    // 更新星级
    __String *starkey = __String::createWithFormat("star_%d_%d", themeId, levelId);
    return ud->getIntegerForKey(starkey->getCString(), 0);
}