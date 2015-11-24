#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    grabber.setup(800, 600);
    tracker.setup();
    
    ofEnableAlphaBlending();
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
    
    ofPushStyle();
    for(int i=0;i<tracker.size();i++){
        
        // Draw debug pose
        tracker.drawPose(i);
        
        // Apply the pose matrix
        ofPushView();
        tracker.loadPoseMatrix(i);
        
        // Now position 0,0,0 is at the forehead
        
        ofSetColor(255,0,0,50);
        ofDrawRectangle(0, 0, 200, 200);

        ofPushMatrix();
        ofSetColor(0,255,0,50);
        ofRotate(-90, 1, 0, 0);
        ofDrawRectangle(0, 0, 200, 200);
        ofPopMatrix();

        ofPushMatrix();
        ofSetColor(0,0,255,50);
        ofRotate(90, 0, 1, 0);
        ofDrawRectangle(0, 0, 200, 200);
        ofPopMatrix();

        
        ofPopView();
    }
    ofPopStyle();
    
    ofDrawBitmapString("Tracker fps: "+ofToString(tracker.getThreadFps()), 10, 20);
}
