#include "ofApp.h"
#include "ofxAndroidVideoGrabber.h"
#include "ofxCv.h"


//--------------------------------------------------------------
void ofApp::setup(){

	grabber.setDeviceID(1);
	grabber.setPixelFormat(OF_PIXELS_MONO);

	grabber.setup(640,480);

	tracker.setRescale(0.25);
	tracker.setLandmarkRescale(1.0);
	tracker.setup();

	// Get the orientation and facing of the current camera
	cameraOrientation = ((ofxAndroidVideoGrabber*)grabber.getGrabber().get())->getCameraOrientation();
	cameraFacingFront = ((ofxAndroidVideoGrabber*)grabber.getGrabber().get())->getFacingOfCamera();

	//ofSetOrientation(OF_ORIENTATION_DEFAULT);

	ofBackground(0);
}

//--------------------------------------------------------------
void ofApp::exit() {
	tracker.waitForThread();
	tracker.stopThread();
}

//--------------------------------------------------------------
void ofApp::update(){
	grabber.update();
	if(grabber.isFrameNew()) {
		int o = (appOrientation+cameraOrientation)%360;
		//ofLog()<<"Orientation "<<o;
		tracker.setRotation(o);
		tracker.update(ofxCv::toCv(grabber));
	}
}

//--------------------------------------------------------------
void ofApp::draw(){
	ofSetColor(255,255,255);

	/*grabber.draw(0,0);

*/

	// Calculate aspect ratio of grabber image
	float grabberAspectRatio = grabber.getWidth() / grabber.getHeight();

	// Draw camera image centered in the window
	ofPushMatrix();
	ofSetColor(255);
	ofSetRectMode(OF_RECTMODE_CENTER);

	// Draw from the center of the window
	ofTranslate(ofGetWidth() / 2, ofGetHeight() / 2);

	if(cameraFacingFront) {
		// If the camera is front, then rotate clockwise
		ofRotate(appOrientation);
	} else {
		// If the camera is backfacing, then rotate counter clockwise
		ofRotate(-appOrientation);
	}

	// Rotate the cameras orientation offset
	ofRotate(cameraOrientation);

	int width = ofGetWidth();
	int height = ofGetHeight();

	// If its landscape mode, then swap width and height
	if(appOrientation == 90 || appOrientation == 270){
		std::swap(width, height);
	}

	// Draw the image
	if(width < height) {
		grabber.draw(0,0, width * grabberAspectRatio,
					 width);

	} else {
		grabber.draw(0,0, height,
					 height * 1.0/grabberAspectRatio);


	}

	ofPopMatrix();

	ofPushMatrix();

	// Draw from the center of the window
	ofTranslate(ofGetWidth() / 2, ofGetHeight() / 2);

	// Draw the image
	if(ofGetWidth() < ofGetHeight()) {
		tracker.draw(0,0, ofGetWidth() , ofGetWidth()* grabberAspectRatio);

	} else {
		tracker.draw(0,0, ofGetHeight() * grabberAspectRatio,
					 ofGetHeight());

	}



	ofPopMatrix();


	ofSetRectMode(OF_RECTMODE_CORNER);


	ofDrawBitmapString("app fps: " + ofToString(ofGetFrameRate()),20,40);
	ofDrawBitmapString("thread fps: " + ofToString(tracker.getThreadFps()),20,60);



}

void ofApp::deviceOrientationChanged(ofOrientation newOrientation){
	appOrientation = ofOrientationToDegrees(newOrientation);
}


//--------------------------------------------------------------
void ofApp::keyPressed  (int key){ 
	
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){ 
	
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::touchDown(int x, int y, int id){

}

//--------------------------------------------------------------
void ofApp::touchMoved(int x, int y, int id){

}

//--------------------------------------------------------------
void ofApp::touchUp(int x, int y, int id){

}

//--------------------------------------------------------------
void ofApp::touchDoubleTap(int x, int y, int id){

}

//--------------------------------------------------------------
void ofApp::touchCancelled(int x, int y, int id){

}

//--------------------------------------------------------------
void ofApp::swipe(ofxAndroidSwipeDir swipeDir, int id){

}

//--------------------------------------------------------------
void ofApp::pause(){

}

//--------------------------------------------------------------
void ofApp::stop(){

}

//--------------------------------------------------------------
void ofApp::resume(){

}

//--------------------------------------------------------------
void ofApp::reloadTextures(){

}

//--------------------------------------------------------------
bool ofApp::backPressed(){
	return false;
}

//--------------------------------------------------------------
void ofApp::okPressed(){

}

//--------------------------------------------------------------
void ofApp::cancelPressed(){

}
