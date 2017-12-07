#pragma once

#include "ofMain.h"
#include "ofxAndroid.h"
#include "ofxFaceTracker2.h"

using namespace std;

class ofApp : public ofxAndroidApp, public ofThread{
	
	public:
		
		void setup();
		void update();
		void draw();
		
		void keyPressed(int key);
		void keyReleased(int key);
		void windowResized(int w, int h);

		void touchDown(int x, int y, int id);
		void touchMoved(int x, int y, int id);
		void touchUp(int x, int y, int id);
		void touchDoubleTap(int x, int y, int id);
		void touchCancelled(int x, int y, int id);
		void swipe(ofxAndroidSwipeDir swipeDir, int id);
	void exit();
	void deviceOrientationChanged(ofOrientation newOrientation);

	ofVideoGrabber grabber;
	ofxFaceTracker2 tracker;

	ofFbo fbo;

	bool cameraFacingFront;
	int cameraOrientation;
	int appOrientation;

	ofVec3f rotation;

	void pause();
		void stop();
		void resume();
		void reloadTextures();

};
