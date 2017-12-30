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
#include "VideoFormat.h"
#include "BasicVideoRenderer.h"
#include "ofxSyphon.h"
#include "ofxBenG.h"

#define bufferSize 180
#define bufferCount 3
#define boomerangsPerMeasure 2

class PlayModes {
public:
    PlayModes();
    ~PlayModes();
    void setup();
    void update();
    void draw();
    void setCurrentBuffer(int index);
    void setDelay(float _delay);
    void setDelayPercent(float delay);
    void setFps(int _fps);
    void toggleRecording();
    void toggleRecording(int index);
    bool isRecording(int index);
    void pauseRecording(int index);
    void resumeRecording(int index);
    void setForwardRatio(float value);
    void setBackwardRatio(float value);
    ofxPm::VideoBuffer& getCurrentBuffer();
    ofxPm::VideoHeader& getCurrentHeader();
    ofTexture& getBufferTexture(int index);
    float getBufferDuration(int index);
    float getVideoWidth();
    float getVideoHeight();
    int getBufferCount();
    void setDelayPercent(int index, float percent);

    std::map<ofxPm::VideoFormat, std::vector<ofPtr<ofxPm::VideoFrame::Obj>>>* pool;
    ofxPm::VideoGrabber grabber;
    ofxPm::VideoBuffer buffers[bufferCount];
    ofxPm::VideoHeader headers[bufferCount];
    ofxPm::BasicVideoRenderer renderers[bufferCount];
    ofxPm::VideoRate rate;

    float videoWidth, videoHeight;
    float beatsPerMinute;
    float forwardRatio;
    float backwardRatio;
    float speed;
    int currentBuffer;
    int delay;
    int fps;
};
