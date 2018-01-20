#include "ofApp.h"

void ofApp::setup() {
    ofSetVerticalSync(true);
    ofEnableAlphaBlending();
    
    ableton.setupLink(beatsPerMinute);
    
    float const width = 1280;
    float const height = width / (1920.0/1080.0);
    playModes.setup("HD Pro Webcam C920", width, height);
    ofSetWindowShape(playModes.getVideoWidth(), playModes.getVideoHeight());

    forwardRatio = 0.5;
    backwardRatio = 0.33;
    beatsPerBoomerang = 4;
    currentBuffer = 0;
    
    setupMidiFighterTwister();
    beatsPerMinute = 120;
    
    for (int i = 0; i < playModes.getBufferCount(); i++) {
        easings[i] = nullptr;
        boomerangCount[i] = -1;
        std::string name = "buffer" + ofToString(i);
        syphon[i].setName(name);
        continueRecordingOnBeat[i] = -1;
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
    playModes.update();

    ofSetWindowTitle(ofToString(ofGetFrameRate()));

    ofxBenG::TimeDiff timeMicros = ofGetElapsedTimeMicros();
    
//    int currentBeat = ableton.getBeat();
//    if (currentBeat != lastBeat) {
//        drawNewFrame = true;
//        lastBeat = currentBeat;
//    }
    updateBoomerangs(timeMicros);

    if (startMeasureOnBeat > 0 && ableton.getBeat() == startMeasureOnBeat && (isLastBoomerang(currentBuffer) || boomerangCount[currentBuffer] < 0)) {
        std::cout << "startMeasureOnBeat == " << startMeasureOnBeat << std::endl;
        startMeasure(startMeasureOnBuffer);
        startMeasureOnBuffer = -1;
        startMeasureOnBeat = -1;
    }

    for (int i = 0; i < playModes.getBufferCount(); i++) {
        auto recordOnBeat = continueRecordingOnBeat[i];
        if (recordOnBeat > 0) {
            if (ableton.getBeat() == recordOnBeat) {
                std::cout << "recordOnBeat == " << recordOnBeat << std::endl;
                freeEasing(i);
                playModes.setDelayPercent(i, 0);
                playModes.resumeRecording(i);
                continueRecordingOnBeat[i] = -1;
            }
            if (recordOnBeat > 4 && ableton.getBeat() == 4) {
                continueRecordingOnBeat[i] = recordOnBeat - 4;
            }
        }
    }
    
}

void ofApp::drawVideo() {
    auto bufferCount = playModes.getBufferCount();
    auto columnWidth = ofGetWidth() / bufferCount;
    auto columnHeight = ofGetHeight();
    auto videoWidth = playModes.getVideoWidth();
    auto videoHeight = playModes.getVideoHeight();
    auto x = 0.0;
    for (int i = 0; i < bufferCount; i++, x += columnWidth) {
        auto thisColumnWidth = (videoWidth < columnWidth) ? videoWidth : columnWidth;
        auto thisColumnHeight = (videoHeight < columnHeight) ? videoHeight : columnHeight;
        auto cropX = (thisColumnWidth < videoWidth) ? videoWidth / 3.0f : 0.0f;
        auto cropWidth = (thisColumnWidth < videoWidth) ? thisColumnWidth : videoWidth;
        auto y = (videoHeight < ofGetHeight()) ? ofGetHeight() / 2.0f - thisColumnHeight / 2.0f : 0.0f;
        ofTexture& rendererTexture = playModes.getBufferTexture(i);
        rendererTexture.drawSubsection(x, y, thisColumnWidth, thisColumnHeight, cropX, 0, thisColumnWidth, videoHeight);
    }
}

void ofApp::draw() {
    ofBackground(0);
//
//    if (drawNewFrame) {
//        drawNewFrame = false;
//        currentFrame = playModes.getBufferTexture(0);
//    }
//    if (currentFrame.isAllocated()) {
//        currentFrame.draw(0, 0);
//    }
    
    drawVideo();

    for (int i = 0; i < playModes.getBufferCount(); i++) {
        syphon[i].publishTexture(&playModes.getBufferTexture(i));
    }
    
    if (!inFullscreen) {
        float top = ofGetHeight();
        std::string forwardRatioString = "forwardRatio = " + ofToString(forwardRatio);
        ofDrawBitmapString(forwardRatioString, 15, top-15);
        std::string backwardRatioString = "backwardRatio = " + ofToString(backwardRatio);
        ofDrawBitmapString(backwardRatioString, 15, top-35);
        std::string beatsPerBoomerangString = "beatsPerBoomerang = " + ofToString(beatsPerBoomerang);
        ofDrawBitmapString(beatsPerBoomerangString, 15, top-55);
        std::string beat = "beat = " + ofToString(ableton.getBeat());
        ofDrawBitmapString(beat, 15, top-75);
        std::string bpm = "bpm = " + ofToString(ableton.getTempo());
        ofDrawBitmapString(bpm, 15, top-95);
    }
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
        continueRecordingOnBeat[currentBuffer] = 1;
        fadeToBuffer(index);
        lastBuffer = currentBuffer;
        currentBuffer = index;
    }
    
    playModes.pauseRecording(index);
    boomerangCount[index] = 0;
    boomerang(index);
}

bool ofApp::isLastBoomerang(int index) {
    return boomerangCount[index] == playModes.getBoomerangsPerMeasure();
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
        inFullscreen = !inFullscreen;
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
