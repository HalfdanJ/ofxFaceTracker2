#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    grabber.setup(1280, 720);
    
    // Setup ofxFaceTracker
    tracker.setup();

    // All examples share data files from example-data, so setting data path to this folder
    // This is only relevant for the example apps
    ofSetDataPathRoot(ofFile(__BASE_FILE__).getEnclosingDirectory()+"../../model/");
    
    // Setup ofxFaceTracker2
    tracker2.setup();
    
    ofRestoreWorkingDirectoryToDefault();
}


//--------------------------------------------------------------
void ofApp::update(){
    grabber.update();
    if(grabber.isFrameNew()){
        clock_t start = clock() ;
        
        tracker.update(ofxCv::toCv(grabber));

        clock_t end = clock() ;
        double elapsed_time = (end-start)/(double)CLOCKS_PER_SEC ;
        oldTracker.push_back(elapsed_time);
        
        start = clock() ;
        
        tracker2.update(ofxCv::toCv(grabber));
       
        end = clock() ;
        elapsed_time = (end-start)/(double)CLOCKS_PER_SEC ;
        newTracker.push_back(elapsed_time);
        
        if(oldTracker.size() > 100){
            oldTracker.pop_front();
            newTracker.pop_front();
        }

    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    // Draw camera image
    ofSetColor(180);
    grabber.draw(0,0);
    
    
    // Draw original ofxFaceTracker
    ofSetColor(255,0,0);
    tracker.draw();
    
    // Draw new ofxFaceTracker2
    ofSetColor(255);
    tracker2.drawDebug();
    
    // Draw graph showing their processing time
    ofPushMatrix();
    ofTranslate(0, 490);
    
    ofSetColor(30);
    ofDrawRectangle(0, 0, 300, 200);
    ofDrawBitmapStringHighlight("Tracker processing time", 10,15);
    
    ofSetColor(255,0,0);
    if(oldTracker.size() > 0){
        glBegin(GL_LINE_STRIP);
        for(int i=0;i<oldTracker.size()-1;i++){
            glVertex2d(i*3, 200-2*200*oldTracker[i]);
        }
        glEnd();
        
        ofSetColor(255,255,255);
        
        glBegin(GL_LINE_STRIP);
        for(int i=0;i<newTracker.size()-1;i++){
            glVertex2d(i*3, 200-2*200*newTracker[i]);
        }
        glEnd();
    }
    ofPopMatrix();
    
    // Draw debug text
    ofDrawBitmapStringHighlight("ofxFaceTracker", 10, 20, ofColor::black, ofColor::red);
    ofDrawBitmapStringHighlight("ofxFaceTracker2", 10, 40);    
}
