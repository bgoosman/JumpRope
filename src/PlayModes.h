//
//  PlayModes.h
//  
//
//  Created by Joshua Batty on 5/05/14.
//  Modified by Benjamin Goosman in Fall 2018
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
#include "ease.h"

#define NUM_FRAMES 300
#define NUM_BUFFERS 1

class PlayModes {
public:
    void setup(float beatsPerMinute);
    void update();
    void draw();
    void setBeatsPerMinute(float bpm);
    void setFps(int _fps);
    void setDelay(float _delay);
    void setDelayPercent(float delay);
    void reverse();
    void togglePlay();
    bool isPaused();
    void jumpRope(int buffer);
    ofxPm::VideoBuffer& getCurrentBuffer();
    ofxPm::VideoHeader& getCurrentHeader();

    ofxPm::VideoGrabber grabber;
    ofxPm::VideoBuffer buffers[NUM_BUFFERS];
    ofxPm::VideoHeader headers[NUM_BUFFERS];
    ofxPm::BasicVideoRenderer renderer;
    ofxPm::VideoRate rate;
    int fps;
    int delay;
    int currentBuffer;
    float beatsPerMinute;
    float speed;
    ease* easings[NUM_BUFFERS];
};
