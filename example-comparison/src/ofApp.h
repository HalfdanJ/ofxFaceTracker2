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
    
    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void mouseEntered(int x, int y);
    void mouseExited(int x, int y);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    void exit();
    
    ofVideoGrabber grabber;

    ofxFaceTracker tracker;
    ofxFaceTracker2 tracker2;
    
    ofxCv::ObjectFinder finder;
    
    std::deque<double> oldTracker;
    std::deque<double> newTracker;
};
