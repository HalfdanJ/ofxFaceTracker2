#pragma once

#include "ofMain.h"
#include "ofxCv.h"
#include "Clone.h"
#include "Coord.h"
#include "TargetMesh.h"
#include "ofxFaceTracker2.h"

class ofApp : public ofBaseApp {
public:
	void setup();
	void update();
	void draw();
	void loadFace(string face);
	

	ofVideoGrabber cam;
	ofVideoPlayer targetVideoPlayer;
	ofMesh camMesh;
	ofxFaceTracker2 srcTracker;
	ofxFaceTracker2 camTracker;
	ofImage src;
	TargetMesh targetMesh;
	vector<glm::vec2> srcPoints;
	
	bool cloneReady;
	Clone clone;
	ofFbo srcFbo, maskFbo;

	ofDirectory faces;
	int currentFace;
};
