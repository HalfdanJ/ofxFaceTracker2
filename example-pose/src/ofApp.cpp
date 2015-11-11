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
    grabber.draw(0,0);
    tracker.drawPose();
}
