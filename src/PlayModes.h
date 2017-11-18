//
//  PlayModes.h
//  
//
//  Created by Joshua Batty on 5/05/14.
//
//

#pragma once

#include "ofMain.h"
#include "VideoBuffer.h"
#include "VideoHeader.h"
#include "VideoGrabber.h"
#include "VideoRenderer.h"
#include "VideoRate.h"
#include "BasicVideoRenderer.h"


#define NUM_FRAMES 400

class PlayModes{
    
public:
    void setup();
    void update();
    void draw();
    void setFps(int _fps);
    void setDelay(float _delay);
    void setDelayPercent(float delay);
    void reverse();
    void togglePlay();
    bool isPaused();
    void beginJumpRope();

    ofxPm::VideoGrabber grabber;
    ofxPm::VideoBuffer buffer;
    ofxPm::VideoHeader header;
    ofxPm::BasicVideoRenderer renderer;
    ofxPm::VideoRate rate;
    int fps;
    int delay;
    float speed;
};
