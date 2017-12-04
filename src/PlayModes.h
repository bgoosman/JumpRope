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
#define NUM_BUFFERS 2
#define MAX_JUMP_ROPES 4

class PlayModes {
public:
    void setup(float beatsPerMinute);
    void update();
    void draw();
    void jumpRope(int buffer);
    void setBeatsPerMinute(float bpm);
    void setCurrentBuffer(int index);
    void setDelay(float _delay);
    void setDelayPercent(float delay);
    void setFps(int _fps);
    void toggleRecording();
    void toggleRecording(int index);
    bool isRecording(int index);
    void pauseRecording(int index);
    void resumeRecording(int index);
    void startMeasure(int index);
    void updateCurrentHeader(TimeDiff timeMicros);
    ofxPm::VideoBuffer& getCurrentBuffer();
    ofxPm::VideoHeader& getCurrentHeader();
    void freeEasing(int index);

    ofxPm::VideoGrabber grabber;
    ofxPm::VideoBuffer buffers[NUM_BUFFERS];
    ofxPm::VideoHeader headers[NUM_BUFFERS];
    ofxPm::BasicVideoRenderer renderer;
    ofxPm::VideoRate rate;
    ease* easings[NUM_BUFFERS];
    int currentBuffer;
    int delay;
    int fps;
    int jumpRopeCount;
    int nextBuffer;
    float beatsPerMinute;
    float speed;
};
