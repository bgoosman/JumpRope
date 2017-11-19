#ifndef ease_h
#define ease_h

#include "ofxEasing.h"
#include "Poco/Timestamp.h"
#include <algorithm>

typedef Poco::Timestamp::TimeDiff TimeDiff;

class ease {
public:
    ease(TimeDiff startTime, TimeDiff forwardDuration, float startValue, float targetValue, ofxeasing::function easingFunction) :
        startTime(startTime),
        forwardDuration(forwardDuration),
        backwardDuration(-1),
        isGoingBackward(false),
        startValue(startValue),
        targetValue(targetValue),
        easingFunction(easingFunction) {}
    
    ease(TimeDiff startTime, TimeDiff forwardDuration, TimeDiff backwardDuration, float startValue, float targetValue, ofxeasing::function easingFunction) :
         startTime(startTime),
         forwardDuration(forwardDuration),
         backwardDuration(backwardDuration),
         isGoingBackward(false),
         startValue(startValue),
         targetValue(targetValue),
         easingFunction(easingFunction) {}
    
    ~ease() {}
    
    float update(ofxPm::TimeDiff currentTime) {
        auto endTime = getEndTime();
        if (currentTime < startTime) {
            currentValue = startValue;
        } else if (currentTime >= endTime) {
            if (backwardDuration > 0 && !isGoingBackward) {
                isGoingBackward = true;
                startTime = currentTime;
                float temp = startValue;
                startValue = targetValue;
                targetValue = temp;
            } else {
                currentValue = targetValue;
            }
        } else {
            currentValue = ofxeasing::map(currentTime, startTime, endTime, startValue, targetValue, easingFunction);
        }
        
        return currentValue;
    }
    
    TimeDiff getDuration() {
        return isGoingBackward ? backwardDuration : forwardDuration;
    }
    
    TimeDiff getEndTime() {
        return startTime + getDuration();
    }
    
    bool isPlaying() {
        return startValue < currentValue && currentValue < targetValue;
    }
    
    bool isDone(TimeDiff currentTime) {
        return currentTime > getEndTime();
    }
    
private:
    bool isGoingBackward;
    float currentValue;
    float targetValue;
    TimeDiff startTime;
    TimeDiff startValue;
    TimeDiff forwardDuration;
    TimeDiff backwardDuration;
    ofxeasing::function easingFunction;
};

#endif /* ease_h */
