#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetVerticalSync(true);
    ofEnableAlphaBlending();
    playModes.setup();
}

//--------------------------------------------------------------
void ofApp::update() {
    ofSetWindowTitle(ofToString(ofGetFrameRate()));
    playModes.update();
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
        playModes.beginJumpRope();
    }
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ) {
    if (playModes.isPaused()) {
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
