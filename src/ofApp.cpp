#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetVerticalSync(true);
    ofEnableAlphaBlending();
    playModes.setup(60);
    setupMidiFighterTwister();
    nextJumpRope = ofGetElapsedTimef() + 5;
}

void ofApp::setupMidiFighterTwister() {
    twister.setup();
    ofAddListener(twister.eventEncoder, this, &ofApp::onEncoderUpdate);
    ofAddListener(twister.eventPushSwitch, this, &ofApp::onPushSwitchUpdate);
    ofAddListener(twister.eventSideButton, this, &ofApp::onSideButtonPressed);
}

void ofApp::onEncoderUpdate(ofxMidiFighterTwister::EncoderEventArgs& a){
    std::cout << "Encoder '" << a.ID << "' Event! val: " << a.value << std::endl;
    if (a.ID == 0) {
        auto delayPercent = ofMap(a.value, 0, 127, 0, 1);
        playModes.setDelayPercent(delayPercent);
    } else if (a.ID == 1) {
        playModes.setBeatsPerMinute(ofMap(a.value, 0, 127, 0, 300));
    }
}

void ofApp::onPushSwitchUpdate(ofxMidiFighterTwister::PushSwitchEventArgs& a){
    std::cout << "PushSwitch '" << a.ID << "' Event! val: " << a.value << std::endl;
    if (a.ID == 0 && a.value == 127) {
        playModes.jumpRope(a.ID);
    }
}

void ofApp::onSideButtonPressed(ofxMidiFighterTwister::SideButtonEventArgs & a){
    std::cout << "Side Button Pressed" << std::endl;
}

//--------------------------------------------------------------
void ofApp::update() {
    ofSetWindowTitle(ofToString(ofGetFrameRate()));
    playModes.update();
    float currentTimeSeconds = ofGetElapsedTimef();
    if (currentTimeSeconds > nextJumpRope) {
        nextJumpRope = currentTimeSeconds + 5;
    }
}

//--------------------------------------------------------------
void ofApp::draw() {
    ofSetColor(255);
    playModes.draw();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {
    if (key == 'r') {
        playModes.reverse();
    } else if (key == ' ') {
        playModes.setDelayPercent(0);
        playModes.togglePlay();
    } else if (key == 'j' && playModes.isPaused()) {
        playModes.jumpRope(0);
    }
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y) {
    if (playModes.isPaused() && false) {
        playModes.setDelayPercent(float(x)/float(ofGetWidth()));
    }
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
