#include "ofApp.h"
#include "ofxAndroidVideoGrabber.h"
#include "ofxCv.h"


//--------------------------------------------------------------
void ofApp::setup(){
	// Select front camera
	grabber.setDeviceID(1);

	// Make sure its running in mono pixels mode for pixels (much faster)
	grabber.setPixelFormat(OF_PIXELS_MONO);

	// High res camera
	grabber.setup(1280,960);
	tracker.setup();

	// Get the orientation and facing of the current camera
	cameraOrientation = ((ofxAndroidVideoGrabber*)grabber.getGrabber().get())->getCameraOrientation();
	cameraFacingFront = ((ofxAndroidVideoGrabber*)grabber.getGrabber().get())->getFacingOfCamera();

	ofBackground(0);
	ofEnableAlphaBlending();

	fbo.allocate(grabber.getWidth(), grabber.getHeight());
}

//--------------------------------------------------------------
void ofApp::exit() {
}

//--------------------------------------------------------------
void ofApp::update(){
	grabber.update();
	if(grabber.isFrameNew()) {
		int o = (appOrientation+cameraOrientation)%360;
		tracker.setFaceRotation(o);

		cv::Mat cvImg = ofxCv::toCv(grabber);
		tracker.update(cvImg);

		fbo.begin(); {
			grabber.draw(0, 0);
			tracker.drawDebug();
			tracker.drawDebugPose();
		} fbo.end();
	}
}

//--------------------------------------------------------------
void ofApp::draw(){
	ofSetColor(255,255,255);

	ofDisableDepthTest();

	// Calculate aspect ratio of grabber image
	float grabberAspectRatio = grabber.getWidth() / grabber.getHeight();

	// Draw camera image centered in the window
	ofPushMatrix();
	ofSetColor(255);
	ofSetRectMode(OF_RECTMODE_CENTER);

	// Draw from the center of the window
	ofTranslate(ofGetWidth() / 2, ofGetHeight() / 2);

	// If the camera is front, then rotate clockwise
	ofRotate(appOrientation);
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
		fbo.draw(0,0, width * grabberAspectRatio,
					 width);

//		tracker.drawDebug(0,0,width * grabberAspectRatio,
//						  width);
	} else {
		fbo.draw(0,0, height,
					 height * 1.0/grabberAspectRatio);

//		tracker.drawDebug(0,0, height,
//						  height * 1.0/grabberAspectRatio);
	}



	ofPopMatrix();

	ofSetRectMode(OF_RECTMODE_CORNER);
	ofDrawBitmapString("app fps: " + ofToString(ofGetFrameRate()),20,40);
	ofDrawBitmapString("thread fps: " + ofToString(tracker.getThreadFps()),20,60);
}

// Watch for oriantation change of the app
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
