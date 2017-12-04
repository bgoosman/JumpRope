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
    jumpRopeCount = 0;
    nextBuffer = 1;
    
    float width = 1280;
    float height = width / (1920.0/1080.0);
    grabber.setDeviceID(2);
	grabber.initGrabber(width, height);
	rate.setup(grabber, fps);

    for (int i = 0; i < NUM_BUFFERS; i++) {
        bool const allocateOnSetup = true;
        auto& buffer = buffers[i];
        buffer.setup(rate, NUM_FRAMES, allocateOnSetup);

        auto& header = headers[i];
        header.setup(buffer);
        header.setDelayMs(0);
    }
    
	renderer.setup(headers[0]);
}

bool PlayModes::isRecording(int index) {
    return !buffers[index].isStopped();
}

void PlayModes::toggleRecording() {
    toggleRecording(currentBuffer);
}

void PlayModes::toggleRecording(int index) {
    if (isRecording(index)) {
        resumeRecording(index);
    } else {
        pauseRecording(index);
    }
}

void PlayModes::pauseRecording(int index) {
    buffers[index].stop();
}

void PlayModes::resumeRecording(int index) {
    buffers[index].resume();
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
    jumpRopeCount += 1;
    auto& buffer = buffers[bufferIndex];
    float startTime = ofGetElapsedTimeMicros();
    TimeDiff const microsPerSecond = 1e6;
    float const jumpRopeDurationInBeats = 2;
    float const beatsPerSecond = beatsPerMinute / 60.0;
    float bufferDuration = (float)NUM_FRAMES / (float)buffer.getRealFPS();
    float jumpRopeDurationInSeconds = jumpRopeDurationInBeats / beatsPerSecond;
    float bufferDurationInSeconds = jumpRopeDurationInSeconds * (6.0/5.0);
    TimeDiff forwardDuration = bufferDurationInSeconds / 2.0 * microsPerSecond;
    TimeDiff backwardDuration = bufferDurationInSeconds / 3.0 * microsPerSecond;
    float targetDelayPercent = bufferDurationInSeconds / bufferDuration;
    std::cout << "Tweening " << bufferIndex << " delay from 0 to " << targetDelayPercent << " between t = [" << startTime << ", " << startTime + forwardDuration << "]" << " and from " << targetDelayPercent << " to 0 between t = [" << startTime + forwardDuration << ", " << startTime + forwardDuration + backwardDuration << "]" << std::endl;
    easings[bufferIndex] = new ease(forwardDuration, backwardDuration, 0, targetDelayPercent, ofxeasing::linear::easeIn);
}

void PlayModes::update() {
    TimeDiff timeMicros = ofGetElapsedTimeMicros();
    grabber.update();
    updateCurrentHeader(timeMicros);
}

void PlayModes::setCurrentBuffer(int index) {
    currentBuffer = index;
}

void PlayModes::startMeasure(int index) {
    if (index != currentBuffer) {
        resumeRecording(currentBuffer);
        currentBuffer = index;
    }
    
    renderer.setup(headers[currentBuffer]);
    jumpRopeCount = 0;
    nextBuffer = (currentBuffer + 1) % NUM_BUFFERS;
    buffers[currentBuffer].stop();
    jumpRope(currentBuffer);
}

void PlayModes::updateCurrentHeader(TimeDiff timeMicros) {
    auto easing = easings[currentBuffer];
    if (easing != nullptr) {
        float currentValue = easing->update(timeMicros);
        headers[currentBuffer].setDelayPct(currentValue);
        if (easing->isDone(timeMicros)) {
            freeEasing(currentBuffer);
            if (jumpRopeCount > MAX_JUMP_ROPES) {
                startMeasure(nextBuffer);
            } else {
                jumpRope(currentBuffer);
            }
        }
    }
}

void PlayModes::freeEasing(int index) {
    delete easings[index];
    easings[index] = nullptr;
}

void PlayModes::draw() {
    buffers[currentBuffer].draw();
    headers[currentBuffer].draw();
    renderer.draw();
    auto& buffer = getCurrentBuffer();
    std::string realFps = "Real FPS = " + ofToString(buffer.getRealFPS());
    ofDrawBitmapString(realFps, 15, ofGetHeight() - 15);
    std::string beatsPerMinuteString = "BPM = " + ofToString(beatsPerMinute);
    ofDrawBitmapString(beatsPerMinuteString, 15, ofGetHeight() - 35);
}
