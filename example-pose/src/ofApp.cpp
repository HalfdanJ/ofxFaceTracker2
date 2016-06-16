#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    // Setup grabber
    grabber.setup(1280,720);
    
    // All examples share data files from example-data, so setting data path to this folder
    // This is only relevant for the example apps
    ofSetDataPathRoot(ofFile(__BASE_FILE__).getEnclosingDirectory()+"../../model/");
    
    // Setup tracker
    tracker.setup();
}

//--------------------------------------------------------------
void ofApp::update(){
    grabber.update();
    
    // Update tracker when there are new frames
    if(grabber.isFrameNew()){
        tracker.update(grabber);
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    // Draw camera image
    grabber.draw(0,0);
    
    ofPushStyle();
    
    // Draw debug pose
    tracker.drawDebugPose();
    
    // Iterate over all faces
    for(auto face : tracker.getInstances()){
        // Apply the pose matrix
        ofPushView();
        face.loadPoseMatrix();
        
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
    
    ofDrawBitmapStringHighlight("Tracker fps: "+ofToString(tracker.getThreadFps()), 10, 20);
}
