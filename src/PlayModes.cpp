//
//  PlayModes.cpp
// 
//
//  Created by Joshua Batty on 5/05/14.
//
//

#include "PlayModes.h"
using namespace ofxPm;

void PlayModes::setup(float bpm) {
    beatsPerMinute = bpm;
    fps = 60;
    delay = 0;
    speed = 1.0;
    currentBuffer = 0;
    
    float width = 1280;
    float height = width / (1920.0/1080.0);
	grabber.initGrabber(width, height);
	rate.setup(grabber, fps);

    for (int i = 0; i < NUM_BUFFERS; i++) {
        auto& buffer = buffers[i];
        buffer.setup(rate, NUM_FRAMES, true);

        auto& header = headers[i];
        header.setup(buffer);
        header.setDelayMs(0);
    }
    
	renderer.setup(getCurrentHeader());
}

bool PlayModes::isPaused() {
    return getCurrentBuffer().isStopped();
}

void PlayModes::togglePlay() {
    auto& buffer = getCurrentBuffer();
    if (buffer.isStopped()) {
        buffer.resume();
    } else {
        buffer.stop();
    }
}

void PlayModes::reverse() {
    speed *= -1;
    getCurrentHeader().setSpeed(speed);
}

void PlayModes::setBeatsPerMinute(float bpm) {
    beatsPerMinute = bpm;
}

void PlayModes::setFps(int _fps){
    rate.setFps(_fps);
    getCurrentHeader().setFps(_fps);
}

void PlayModes::setDelayPercent(float pct) {
    headers[0].setDelayPct(pct);
//    getCurrentHeader().setDelayPct(pct);
}

void PlayModes::setDelay(float _delay){
    delay = ofMap(_delay, 1.0, 0.0, 0, NUM_FRAMES);
    getCurrentHeader().setDelayFrames(delay);
}

ofxPm::VideoBuffer& PlayModes::getCurrentBuffer() {
    return buffers[currentBuffer];
}

ofxPm::VideoHeader& PlayModes::getCurrentHeader() {
    return headers[currentBuffer];
}

void PlayModes::jumpRope(int bufferIndex) {
    float startTime = ofGetElapsedTimeMicros();
    auto& buffer = getCurrentBuffer();
    TimeDiff const microsPerSecond = 1e6;
    float const jumpRopeDurationInBeats = 2;
    float const beatsPerSecond = beatsPerMinute / 60.0;
    float bufferDuration = (float)NUM_FRAMES / (float)buffer.getRealFPS();
    float jumpRopeDurationInSeconds = jumpRopeDurationInBeats / beatsPerSecond;
    float bufferDurationInSeconds = jumpRopeDurationInSeconds * (6.0/5.0);
    TimeDiff forwardDuration = bufferDurationInSeconds / 2.0 * microsPerSecond;
    TimeDiff backwardDuration = bufferDurationInSeconds / 3.0 * microsPerSecond;
    float targetDelayPercent = bufferDurationInSeconds / bufferDuration;
    std::cout << "Tweening " << bufferIndex << " delay from 0 to 1 between t = [" << startTime << ", " << startTime + forwardDuration << "]" << " and from 1 to 0 between t = [" << startTime + forwardDuration << ", " << startTime + forwardDuration + backwardDuration << "]" << std::endl;
    easings[bufferIndex] = new ease(forwardDuration, backwardDuration, 0, targetDelayPercent, ofxeasing::linear::easeIn);
}

void PlayModes::update() {
    TimeDiff timeMicros = ofGetElapsedTimeMicros();
    if (!isPaused()) {
        grabber.update();
    }
    for (int i = 0; i < NUM_BUFFERS; i++) {
        auto easing = easings[i];
        if (easing != nullptr) {
            float currentValue = easing->update(timeMicros);
            headers[i].setDelayPct(currentValue);
            if (easing->isDone(timeMicros)) {
                delete easings[i];
                easings[i] = nullptr;
                if (isPaused())
                    jumpRope(i);
            }
        }
    }
}

void PlayModes::draw() {
	getCurrentBuffer().draw();
	getCurrentHeader().draw();
	renderer.draw(0, 0, ofGetWidth(), ofGetHeight());
    auto& buffer = getCurrentBuffer();
    std::string realFps = "Real FPS = " + ofToString(buffer.getRealFPS());
    ofDrawBitmapString(realFps, 15, ofGetHeight() - 15);
    std::string beatsPerMinuteString = "BPM = " + ofToString(beatsPerMinute);
    ofDrawBitmapString(beatsPerMinuteString, 15, ofGetHeight() - 35);
}
