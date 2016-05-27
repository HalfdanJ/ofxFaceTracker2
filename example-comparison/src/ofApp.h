#pragma once

#include "ofMain.h"

#include "ofxCv.h"
#include "ofxFaceTracker.h"
#include "ofxFaceTracker2.h"

using namespace std;

class ofApp : public ofBaseApp{
    
public:
    void setup();
    void update();
    void draw();
    
    ofVideoGrabber grabber;

    ofxFaceTracker tracker;
    ofxFaceTracker2 tracker2;
        
    std::deque<double> oldTracker;
    std::deque<double> newTracker;
};
