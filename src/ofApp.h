#pragma once

#include "ofMain.h"
#include "ofxBenG.h"
#include "ofxMidiFighterTwister.h"
#include "ofxSyphon.h"

class ofApp : public ofBaseApp{
public:
    ofApp();
    virtual ~ofApp();
    void setup();
    void update();
    void draw();

    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y);
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void mouseEntered(int x, int y);
    void mouseExited(int x, int y);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    
    void drawVideo();
    void boomerang(int buffer);
    void updateBoomerangs(ofxBenG::TimeDiff timeMicros);
    void freeEasing(int index);
    bool isLastBoomerang(int index);
    void startMeasure(int index);
    void fadeToBuffer(int buffer);
    int getNextBuffer();
    
    void scheduleMeasureOnBeat(int beat, int index);
    ofxSyphonServer syphon[ofxBenG::PlayModes::bufferCount];
    ofxBenG::ease* easings[ofxBenG::PlayModes::bufferCount];
    int boomerangCount[ofxBenG::PlayModes::bufferCount];
    int startMeasureOnBeat = -1;
    int startMeasureOnBuffer = -1;
    int currentBuffer;
    int lastBuffer = 0;
    int lastBeat = 0;
    bool drawNewFrame = false;
    ofTexture currentFrame;

    ofxBenG::ableton ableton;
    ofxBenG::osc osc = {"10.1.10.202", 1234};
    ofxBenG::PlayModes playModes;

    ofxMidiFighterTwister twister;
    int const MAX_BANKS = 4;
    int const MAX_ROWS_PER_BANK = 4;
    int const MAX_COLUMNS_PER_ROW = 4;
    ofxBenG::encoder* encoders[ofxMidiFighterTwister::NUM_ENCODERS];
    void setupMidiFighterTwister();
    void onEncoderUpdate(ofxMidiFighterTwister::EncoderEventArgs &);
    void onPushSwitchUpdate(ofxMidiFighterTwister::PushSwitchEventArgs &);
    void onSideButtonPressed(ofxMidiFighterTwister::SideButtonEventArgs &);
    
    ofxXmlSettings settings;
    std::string SETTINGS_FILE = "settings.xml";
    std::vector<ofxBenG::property_base*> properties;
    ofxBenG::property<float> beatsPerMinute = {"beatsPerMinute", 60, 0, 480};
    ofxBenG::property<float> delayPercent = {"delayPercent", 0, 0, 1};
    ofxBenG::property<float> forwardRatio = {"forwardRatio", 0.5, 0, 1};
    ofxBenG::property<float> backwardRatio = {"backwardRatio", 1.0/3.0, 0, 1};
    ofxBenG::property<int> beatsPerBoomerang = {"beatsPerBoomerang", 4, 0, 12};
    void setupProperties();
    void savePropertiesToXml(std::string& file);
    void loadPropertiesFromXml(std::string& file);
    
    bool inFullscreen = false;
    int continueRecordingOnBeat[ofxBenG::PlayModes::bufferCount];
};
