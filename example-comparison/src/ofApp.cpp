#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    
    grabber.setup(640, 480);
        
    tracker.setup();

    
    ofSetDataPathRoot(ofFile(__BASE_FILE__).getEnclosingDirectory()+"../../example-data/");
    tracker2.setup();
    ofRestoreWorkingDirectoryToDefault();
}


void ofApp::exit() {
    tracker2.waitForThread();
    tracker2.stopThread();
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
    ofSetColor(255);
    grabber.draw(0,0);
    
    ofSetColor(255,0,0);
    tracker.draw();
    
    ofSetColor(255);
    tracker2.draw();
    
    ofDrawBitmapString("tracker thread fps "+ofToString(tracker2.getThreadFps()), 20, 60);

    
    
    ofSetColor(255,255,255);
    
    
    ofTranslate(0, 490);
    
    ofSetColor(30);
    ofDrawRectangle(0, 0, 300, 200);
    
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
    
    ofSetColor(255,255,255);


}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
