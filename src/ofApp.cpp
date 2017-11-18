#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetVerticalSync(true);
    ofEnableAlphaBlending();
    
    // Video
    playModes.setup();
    
    // Audio
    sampleRate = 44100;
    bufferSize = 512;
    grainPlayer.setup();
    mixer.addInputFrom(&grainPlayer);
    
    // SoundStream
    std::cout << soundStream.getDeviceList();
    soundStream.setup(this, 2, 2, sampleRate, bufferSize, 4);
    soundStream.setOutput(this);
    
    // GUI
    gui = new ofxUICanvas();
    gui->setFont("GUI/Fett.ttf");
    gui->addLabel("granular synthesis");
    gui->addSpacer();
    gui->addSlider("Speed", -4.0, 4.0, &grainPlayer.speed);
    gui->addSlider("Pitch", 0.0, 10.0, &grainPlayer.pitch);
    gui->addSlider("GrainSize", 0.025, 0.45, &grainPlayer.grainSize);
    gui->addSlider("Overlaps", 1, 5, &grainPlayer.overlaps);
    gui->addSpacer();
    gui->addToggle("Record Input", true);
    gui->addToggle("Set Position", &grainPlayer.bSetPosition);
    gui->addSlider("Position", 0.0, 1.0, 1.0);
    gui->addSlider("Volume", 0.0, 1.0, &grainPlayer.volume);
    gui->autoSizeToFitWidgets();
    gui->loadSettings("GUI/guiSettings.xml");
    ofAddListener(gui->newGUIEvent,this, &ofApp::guiEvent);
}

//--------------------------------------------------------------
void ofApp::update() {
    ofSetWindowTitle(ofToString(ofGetFrameRate()));
    
    playModes.update();
    grainPlayer.updatePlayHead();
    
    if (!grainPlayer.bRecLiveInput && !grainPlayer.bSetPosition) {
        playModes.setDelay(grainPlayer.ps->getNormalisedPosition());
    }
}

//--------------------------------------------------------------
void ofApp::draw() {
    ofSetColor(255);
    playModes.draw();
    grainPlayer.draw();
    playModes.drawData();
}

//--------------------------------------------------------------
void ofApp::audioIn(float * input, int bufferSize, int nChannels) {
    grainPlayer.audioReceived(input,bufferSize,nChannels);
}

//--------------------------------------------------------------
void ofApp::audioOut(float * output, int bufferSize, int nChannels) {
    mixer.audioRequested(output, bufferSize, nChannels);
}

//--------------------------------------------------------------
void ofApp::guiEvent(ofxUIEventArgs &e){
    std::string name = e.widget->getName();
    if (name == "Position") {
        ofxUISlider* slider = (ofxUISlider *) e.widget;
        grainPlayer.playHead = slider->getScaledValue();
        playModes.setDelay(slider->getScaledValue());
    } else if (name == "Record Input") {
        ofxUIToggle* toggle = (ofxUIToggle *) e.widget;
        grainPlayer.bRecLiveInput = toggle->getValue();
        if (toggle->getValue()) {
            //playModes.vHeader.setInPct(grainPlayer.getRecordPostion());
            playModes.vBuffer.resume();
        } else {
            playModes.vBuffer.stop();
        }
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

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
