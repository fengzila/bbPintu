//
//  BBAudio.h
//  BBPintu
//
//  Created by FengZi on 14-5-7.
//
//

#ifndef __BBPintu__BBAudio__
#define __BBPintu__BBAudio__

#include <iostream>
#include "cocos2d.h"
USING_NS_CC;

class BBAudio {
public:
    static void preloadEffect(const char* filePath);
    static void preloadMusic(const char* filePath);
    
    static void playMusic(const char* filePath, bool loop=true);
    static int playEffect(const char* filePath, bool loop=false);
    
    static void stopMusic(bool release=false);
    static void stopEffect(int id);
    
    static void setMusicOnOff(bool onOff);
    static void setEffectOnOff(bool onOff);
    
    static void setMusicVolume(float volume);
    static void setEffectVolume(float volume);
    
    static float getMusicVolume();
    
};

#endif /* defined(__BBPintu__BBAudio__) */
