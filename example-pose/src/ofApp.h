#pragma once

#include "ofMain.h"
#include "ofxFaceTracker2.h"

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
        void draw();

        ofVideoGrabber grabber;
        ofxFaceTracker2 tracker;
		
};
