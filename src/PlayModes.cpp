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
	vGrabber.initGrabber(320,240);
	vRate.setup(vGrabber,fps);
	vBuffer.setup(vRate,NUM_FRAMES,true);
	vHeader.setup(vBuffer);
	vHeader.setDelayMs(0);
	vRenderer.setup(vHeader);
    vHeader.setPlaying(true);

}


void PlayModes::setFps(int _fps){
    vRate.setFps(_fps);
    vHeader.setFps(_fps);
}

void PlayModes::setDelay(float _delay){
    //vBuffer.setFramePos(ofMap(_delay, 0.0, 1.0, 0, NUM_FRAMES));

    delay = ofMap(_delay, 1.0, 0.0, 0, NUM_FRAMES);
    vHeader.setDelayFrames(delay);
}

//--------------------------------------------------------------
void PlayModes::update(){
	vGrabber.update();
    vBuffer.iterFramePos();

//    cout << "vBuffer Size = " << vBuffer.size() << endl;
//    cout << "FramePos (perc) = " << (float)vBuffer.getFramePos() / (float)NUM_FRAMES << endl;
}

//--------------------------------------------------------------
void PlayModes::draw(){
    
	vBuffer.draw();
	vHeader.draw();
    
	vRenderer.draw(0,0,ofGetWidth(),ofGetHeight());
    
	//gui.draw();
	ofDrawBitmapString("FPS: " + ofToString(int(ofGetFrameRate()))
                       + " || cameraBuffer FPS " + ofToString(vBuffer.getRealFPS())
                       + " || videoframes pool size: " + ofToString(VideoFrame::getPoolSize(VideoFormat(640,480,3)))
                       + " || total frames: " +ofToString(VideoFrame::getTotalNumFrames()),20,ofGetHeight()-40);
    
}

//--------------------------------------------------------------
void PlayModes::drawData(){
    float vhPos = ofMap(vHeader.getDelayFrames(),0,NUM_FRAMES,0.0,1.0);

    
    const float waveformWidth  = ofGetWidth() - 40;
    const float waveformHeight = 300;
    
    float top = ofGetHeight() - waveformHeight - 20;
    float left = 20;
    
    ////////// Video Header Play Pos ///////////////////////
    ofSetColor(255,0,0);
    ofDrawBitmapString("Video Header Play Pos", left, top-10);
    ofLine(left, top, waveformWidth, top);
    
    // frame pos
    ofSetColor(0,0,255);
    float framePosPerc = (float)vBuffer.getFramePos() / (float)NUM_FRAMES;
    ofLine(left+ (framePosPerc * (waveformWidth-left)), top, left+ (framePosPerc * (waveformWidth-left)), top+waveformHeight);
    ofCircle(left+(vhPos*(waveformWidth-left)), top, 10);
}

