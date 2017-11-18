//
//  PlayModes.cpp
// 
//
//  Created by Joshua Batty on 5/05/14.
//
//

#include "PlayModes.h"
using namespace ofxPm;

//--------------------------------------------------------------
void PlayModes::setup(){
    fps = 60;
    delay = 0;
    speed = 1.0;
    float width = 1280;
    float height = width / (1920.0/1080.0);
	grabber.initGrabber(width, height);
	rate.setup(grabber,fps);
	buffer.setup(rate,NUM_FRAMES,true);
	header.setup(buffer);
	header.setDelayMs(0);
	renderer.setup(header);
}

void PlayModes::beginJumpRope() {
    
}

bool PlayModes::isPaused() {
    return buffer.isStopped();
}

void PlayModes::togglePlay() {
    if (buffer.isStopped()) {
        buffer.resume();
    } else {
        buffer.stop();
    }
}

void PlayModes::reverse() {
    speed *= -1;
    header.setSpeed(speed);
}

void PlayModes::setFps(int _fps){
    rate.setFps(_fps);
    header.setFps(_fps);
}

void PlayModes::setDelayPercent(float pct) {
    header.setDelayPct(pct);
}

void PlayModes::setDelay(float _delay){
    delay = ofMap(_delay, 1.0, 0.0, 0, NUM_FRAMES);
    header.setDelayFrames(delay);
}

//--------------------------------------------------------------
void PlayModes::update() {
    grabber.update();
}

//--------------------------------------------------------------
void PlayModes::draw() {
	buffer.draw();
	header.draw();
	renderer.draw(0,0,ofGetWidth(),ofGetHeight());
}
