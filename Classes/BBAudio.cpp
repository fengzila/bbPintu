//
//  BBAudio.cpp
//  BBPintu
//
//  Created by FengZi on 14-5-7.
//
//

#include "BBAudio.h"
#include "SimpleAudioEngine.h"

#if !defined(MIN)
#define MIN(A,B)	((A) < (B) ? (A) : (B))
#endif

#if !defined(MAX)
#define MAX(A,B)	((A) > (B) ? (A) : (B))
#endif

void BBAudio::preloadEffect(const char* filePath)
{
    CocosDenshion::SimpleAudioEngine::getInstance()->preloadEffect(filePath);
}

void BBAudio::preloadMusic(const char* filePath)
{
    CocosDenshion::SimpleAudioEngine::getInstance()->preloadBackgroundMusic(filePath);
}

void BBAudio::playMusic(const char* filePath, bool loop) {
    if (!UserDefault::getInstance()->getBoolForKey("musicOn", true))
    {
        return;
    }
    CocosDenshion::SimpleAudioEngine::getInstance()->playBackgroundMusic(filePath, loop);
}

int BBAudio::playEffect(const char* filePath, bool loop) {
    if (!UserDefault::getInstance()->getBoolForKey("soundOn", true))
    {
        return 0;
    }
    int id = CocosDenshion::SimpleAudioEngine::getInstance()->playEffect(filePath, loop);
    return id;
}

void BBAudio::stopMusic(bool release) {
    CocosDenshion::SimpleAudioEngine::getInstance()->stopBackgroundMusic();
}

void BBAudio::stopEffect(int id) {
    CocosDenshion::SimpleAudioEngine::getInstance()->stopEffect(id);
}
float BBAudio::getMusicVolume(){
    return CocosDenshion::SimpleAudioEngine::getInstance()->getBackgroundMusicVolume();
}

void BBAudio::setMusicVolume(float volume){
    volume = MAX( MIN(volume, 1.0), 0 );
    CocosDenshion::SimpleAudioEngine::getInstance()->setBackgroundMusicVolume(volume);
}
void BBAudio::setEffectVolume(float volume){
    volume = MAX( MIN(volume, 1.0), 0 );
    CocosDenshion::SimpleAudioEngine::getInstance()->setEffectsVolume(volume);
}

void BBAudio::setMusicOnOff(bool onOff) {
    if (onOff) {
        CocosDenshion::SimpleAudioEngine::getInstance()->setBackgroundMusicVolume(1);
    } else {
        CocosDenshion::SimpleAudioEngine::getInstance()->setBackgroundMusicVolume(0);
    }
}

void BBAudio::setEffectOnOff(bool onOff) {
    if (onOff) {
        CocosDenshion::SimpleAudioEngine::getInstance()->setEffectsVolume(1);
    } else {
        CocosDenshion::SimpleAudioEngine::getInstance()->setEffectsVolume(0);
    }
}