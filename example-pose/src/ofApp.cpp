#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    grabber.setup(640, 480);
    tracker.setup();
}

//--------------------------------------------------------------
void ofApp::update(){
    grabber.update();
    if(grabber.isFrameNew()){
        tracker.update(ofxCv::toCv(grabber));
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    // Draw the grabber
    grabber.draw(0,0);
    
    // Draw debug pose
    tracker.drawPose();
    
    // Apply the pose matrix
    ofPushView();
    ofPushMatrix();
    tracker.applyPoseMatrix();
    
    // Draw sphere (0,0,0) is forehead
    ofDrawSphere(0, 0, 150, 20);
    
    ofPopMatrix();
    ofPopView();
}
