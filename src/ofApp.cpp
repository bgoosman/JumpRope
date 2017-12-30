#include "ofApp.h"

void ofApp::setup() {
    ofSetVerticalSync(true);
    ofEnableAlphaBlending();
    ableton.setupLink(beatsPerMinute);
    playModes.setup();
    forwardRatio = 0.5;
    backwardRatio = 0.33;
    beatsPerBoomerang = 4;
    ofSetWindowShape(playModes.getVideoWidth(), playModes.getVideoHeight());
    setupMidiFighterTwister();
    currentBuffer = 0;
    for (int i = 0; i < playModes.getBufferCount(); i++) {
        boomerangCount[i] = -1;
        std::string name = "buffer" + ofToString(i);
        syphon[i].setName(name);
    }
}

ofApp::ofApp() {}

ofApp::~ofApp() {
    savePropertiesToXml(ofApp::SETTINGS_FILE);
}

void ofApp::loadPropertiesFromXml(std::string& file) {
    settings.load(file);
    
    for (auto property : properties) {
        property->load(settings);
    }
}

void ofApp::savePropertiesToXml(std::string& file) {
    for (auto property : properties) {
        property->save(settings);
    }
    
    settings.save(file);
}

void ofApp::setupMidiFighterTwister() {
    twister.setup();
    ofAddListener(twister.eventEncoder, this, &ofApp::onEncoderUpdate);
    ofAddListener(twister.eventPushSwitch, this, &ofApp::onPushSwitchUpdate);
    ofAddListener(twister.eventSideButton, this, &ofApp::onSideButtonPressed);
    for (int i = 0; i < ofxMidiFighterTwister::NUM_ENCODERS; i++) {
        encoders[i] = new ofxBenG::encoder(i, 0, 127, &twister);
    }

    beatsPerMinute.addSubscriber([&]() {
        ableton.setBeatsPerMinute((int)beatsPerMinute);
    });
    delayPercent.addSubscriber([&]() { playModes.setDelayPercent(delayPercent); });
    forwardRatio.addSubscriber([&]() { playModes.setForwardRatio(forwardRatio); });
    backwardRatio.addSubscriber([&]() { playModes.setBackwardRatio(backwardRatio); });
    
    ofxBenG::property_base* bank[4][4][4] = {
        {
            {δ(beatsPerMinute), δ(delayPercent), δ(forwardRatio), δ(backwardRatio)},
            {δ(beatsPerBoomerang), nullptr, nullptr, nullptr},
            {nullptr, nullptr, nullptr, nullptr},
            {nullptr, nullptr, nullptr, nullptr}
        },
        {
            {nullptr, nullptr, nullptr, nullptr},
            {nullptr, nullptr, nullptr, nullptr},
            {nullptr, nullptr, nullptr, nullptr},
            {nullptr, nullptr, nullptr, nullptr}
        },
        {
            {nullptr, nullptr, nullptr, nullptr},
            {nullptr, nullptr, nullptr, nullptr},
            {nullptr, nullptr, nullptr, nullptr},
            {nullptr, nullptr, nullptr, nullptr},
        },
        {
            {nullptr, nullptr, nullptr, nullptr},
            {nullptr, nullptr, nullptr, nullptr},
            {nullptr, nullptr, nullptr, nullptr},
            {nullptr, nullptr, nullptr, nullptr},
        }
    };
    
    for (int bankIndex = 0; bankIndex < MAX_BANKS; bankIndex++) {
        for (int rowIndex = 0; rowIndex < MAX_ROWS_PER_BANK; rowIndex++) {
            for (int colIndex = 0; colIndex < MAX_COLUMNS_PER_ROW; colIndex++) {
                auto property = bank[bankIndex][rowIndex][colIndex];
                if (property != nullptr) {
                    properties.push_back(property);
                }
            }
        }
    }
    
    loadPropertiesFromXml(ofApp::SETTINGS_FILE);
    
    int encoderIndex = 0;
    for (int bankIndex = 0; bankIndex < MAX_BANKS; bankIndex++) {
        for (int rowIndex = 0; rowIndex < MAX_ROWS_PER_BANK; rowIndex++) {
            for (int colIndex = 0; colIndex < MAX_COLUMNS_PER_ROW; colIndex++) {
                auto property = bank[bankIndex][rowIndex][colIndex];
                if (property != nullptr) {
                    encoders[encoderIndex++]->bind(property);
                }
            }
        }
    }
}

void ofApp::onEncoderUpdate(ofxMidiFighterTwister::EncoderEventArgs& a) {
    if (a.ID < properties.size()) {
        std::cout << "onEncoderUpdate(" << a.ID << "): Setting " << properties[a.ID]->getName() << " to MIDI " << a.value << std::endl;
        encoders[a.ID]->setValue(a.value);
    }
}

void ofApp::onPushSwitchUpdate(ofxMidiFighterTwister::PushSwitchEventArgs& a) {
    
}

void ofApp::onSideButtonPressed(ofxMidiFighterTwister::SideButtonEventArgs & a) {
    std::cout << "Side Button Pressed" << std::endl;
}

void ofApp::update() {
    for (auto& global : properties) {
        global->clean();
    }

    ofSetWindowTitle(ofToString(ofGetFrameRate()));

    ofxBenG::TimeDiff timeMicros = ofGetElapsedTimeMicros();
    updateBoomerangs(timeMicros);
    
    if (startMeasureOnBeat > 0 && ableton.getBeat() == startMeasureOnBeat && (isLastBoomerang(currentBuffer) || boomerangCount[currentBuffer] < 0)) {
        std::cout << "startMeasureOnBeat == " << startMeasureOnBeat << std::endl;
        startMeasure(startMeasureOnBuffer);
        startMeasureOnBuffer = -1;
        startMeasureOnBeat = -1;
    }
    
    if (continueRecordingOnBeat > 0 && ableton.getBeat() == continueRecordingOnBeat) {
        std::cout << "continueRecordingOnBeat == " << continueRecordingOnBeat << std::endl;
        freeEasing(lastBuffer);
        playModes.setDelayPercent(lastBuffer, 0);
        playModes.resumeRecording(lastBuffer);
        continueRecordingOnBeat = -1;
    }
                         
    playModes.update();
}

void ofApp::draw() {
    playModes.draw();
    
    for (int i = 0; i < playModes.getBufferCount(); i++) {
        syphon[i].publishTexture(&playModes.getBufferTexture(i));
    }
    
    float top = ofGetHeight();
    std::string forwardRatioString = "forwardRatio = " + ofToString(forwardRatio);
    ofDrawBitmapString(forwardRatioString, 15, top-15);
    std::string backwardRatioString = "backwardRatio = " + ofToString(backwardRatio);
    ofDrawBitmapString(backwardRatioString, 15, top-35);
    std::string beatsPerBoomerangString = "beatsPerBoomerang = " + ofToString(beatsPerBoomerang);
    ofDrawBitmapString(beatsPerBoomerangString, 15, top-55);
    std::string beat = "beat = " + ofToString(ableton.getBeat());
    ofDrawBitmapString(beat, 15, top-75);
}

void ofApp::fadeToBuffer(int buffer) {
    std::cout << "fading to buffer " << buffer << std::endl;
    std::string channel = "/isadora/" + ofToString(9+buffer);
    osc.send(channel, 1);
}

int ofApp::getNextBuffer() {
    return (currentBuffer + 1) % playModes.getBufferCount();
}

void ofApp::scheduleMeasureOnBeat(int beat, int index) {
    startMeasureOnBeat = beat;
    startMeasureOnBuffer = index;
    std::cout << "scheduling measure on beat " << startMeasureOnBeat << " on buffer " << startMeasureOnBuffer << std::endl;
}

void ofApp::updateBoomerangs(ofxBenG::TimeDiff timeMicros) {
    for (int i = 0; i < playModes.getBufferCount(); i ++) {
        auto easing = easings[i];
        if (easing != nullptr) {
            float currentValue = easing->update(timeMicros);
            playModes.setDelayPercent(i, currentValue);
            if (easing->isDone(timeMicros)) {
                freeEasing(i);
                boomerang(i);
                if (isLastBoomerang(i)) {
                    scheduleMeasureOnBeat(1, getNextBuffer());
                }
            }
        }
    }
}

void ofApp::boomerang(int index) {
    boomerangCount[index] += 1;
    
    float startTime = ofGetElapsedTimeMicros();
    ofxBenG::TimeDiff const microsPerSecond = 1e6;
    float const beatsPerSecond = beatsPerMinute / 60.0;
    float secondsPerBoomerang = beatsPerBoomerang / beatsPerSecond;
    float bufferDurationInSeconds = secondsPerBoomerang / (forwardRatio + backwardRatio);
    ofxBenG::TimeDiff forwardDuration = bufferDurationInSeconds * forwardRatio * microsPerSecond;
    ofxBenG::TimeDiff backwardDuration = bufferDurationInSeconds * backwardRatio * microsPerSecond;
    float targetDelayPercent = bufferDurationInSeconds / bufferDurationInSeconds;
    targetDelayPercent = 1;
    
    std::cout << "Tweening " << index << " delay from 0 to " << targetDelayPercent << " between t = [" << startTime << ", " << startTime + forwardDuration << "]" << " and from " << targetDelayPercent << " to 0 between t = [" << startTime + forwardDuration << ", " << startTime + forwardDuration + backwardDuration << "]" << std::endl;
    easings[index] = new ofxBenG::ease(forwardDuration, backwardDuration, 0, targetDelayPercent, ofxeasing::linear::easeIn);
}

void ofApp::startMeasure(int index) {
    std::cout << "starting measure on " << index << std::endl;
    if (index != currentBuffer) {
        fadeToBuffer(index);
        lastBuffer = currentBuffer;
        currentBuffer = index;
        continueRecordingOnBeat = 2;
    }
    
    playModes.pauseRecording(index);
    boomerangCount[index] = 0;
    boomerang(index);
}

bool ofApp::isLastBoomerang(int index) {
    return boomerangCount[index] == boomerangsPerMeasure;
}

void ofApp::freeEasing(int index) {
    delete easings[index];
    easings[index] = nullptr;
}

void ofApp::keyPressed(int key) { }

void ofApp::keyReleased(int key) {
    if (key == ' ') {
        scheduleMeasureOnBeat(1, currentBuffer);
    } else if (key == 'f') {
        ofToggleFullscreen();
    }
}

void ofApp::mouseMoved(int x, int y) { }
void ofApp::mouseDragged(int x, int y, int button) { }
void ofApp::mousePressed(int x, int y, int button) { }
void ofApp::mouseReleased(int x, int y, int button) { }
void ofApp::mouseEntered(int x, int y) { }
void ofApp::mouseExited(int x, int y) { }
void ofApp::windowResized(int w, int h) { }
void ofApp::gotMessage(ofMessage msg) { }
void ofApp::dragEvent(ofDragInfo dragInfo) { }
