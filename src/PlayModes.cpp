//
//  PlayModes.cpp
// 
//
//  Created by Joshua Batty on 5/05/14.
//
//

#include "PlayModes.h"
using namespace ofxPm;

PlayModes::PlayModes() {}

PlayModes::~PlayModes() {
    for (int i = 0; i < bufferCount; i++) {
        auto& buffer = buffers[i];
        buffer.stop();
    }
    grabber.close();
}

void PlayModes::setup() {
    fps = 60;
    delay = 0;
    speed = 1.0;
    currentBuffer = 0;

    float width = 640;
    float height = width / (1920.0/1080.0);
    grabber.listDevices();
    grabber.setDeviceID(2);
	grabber.initGrabber(width, height);
    videoWidth = grabber.getWidth();
    videoHeight = grabber.getHeight();
	rate.setup(grabber, fps);

    for (int i = 0; i < bufferCount; i++) {
        bool const allocateOnSetup = true;
        auto& buffer = buffers[i];
        buffer.setup(rate, bufferSize, allocateOnSetup);

        auto& header = headers[i];
        header.setup(buffer);
        header.setDelayMs(0);
        
        auto& renderer = renderers[i];
        renderer.setup(headers[i]);
    }
}

float PlayModes::getVideoWidth() {
    return videoWidth;
}

float PlayModes::getVideoHeight() {
    return videoHeight;
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

void PlayModes::setFps(int _fps){
    rate.setFps(_fps);
    getCurrentHeader().setFps(_fps);
}

void PlayModes::setDelayPercent(float pct) {
    headers[0].setDelayPct(pct);
}

void PlayModes::setDelay(float _delay){
    delay = ofMap(_delay, 1.0, 0.0, 0, bufferSize);
    getCurrentHeader().setDelayFrames(delay);
}

ofxPm::VideoBuffer& PlayModes::getCurrentBuffer() {
    return buffers[currentBuffer];
}

ofxPm::VideoHeader& PlayModes::getCurrentHeader() {
    return headers[currentBuffer];
}

void PlayModes::setForwardRatio(float value) {
    forwardRatio = value;
}

void PlayModes::setBackwardRatio(float value) {
    backwardRatio = value;
}

float PlayModes::getBufferDuration(int index) {
    auto& buffer = buffers[index];
    return (float)bufferSize / (float)buffer.getRealFPS();
}

void PlayModes::update() {
    grabber.update();
}

void PlayModes::setCurrentBuffer(int index) {
    currentBuffer = index;
}

int PlayModes::getBufferCount() {
    return bufferCount;
}

ofTexture& PlayModes::getBufferTexture(int index) {
    return renderers[index].getNextTexture();
}

void PlayModes::setDelayPercent(int index, float percent) {
    headers[index].setDelayPct(percent);
}

void PlayModes::draw() {
    auto videoWidth = ofGetWidth() / 3;
    auto videoHeight = videoWidth * (grabber.getHeight() / grabber.getWidth());
    auto x = 0.0;
    for (int i = 0; i < bufferCount; i++, x += videoWidth) {
        ofTexture& rendererTexture = getBufferTexture(i);
        rendererTexture.draw(x, 0, videoWidth, videoHeight);
    }
}
