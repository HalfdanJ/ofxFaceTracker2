#include <time.h>
#include "ofxFaceTracker2.h"


using namespace ofxCv;
using namespace cv;
using namespace std;

#define it at<int>
#define db at<double>


ofxFaceTracker2::ofxFaceTracker2()
		:failed(true)
		,threaded(true)
		,imageRotation(0)
		,numFaces(0)
		,landmarkDetectorImageSize(-1)

#ifdef TARGET_ANDROID
		,faceDetectorImageSize(160*120*1.3)
#else
,faceDetectorImageSize(480*360)
#endif

{
}

void ofxFaceTracker2::setup(string dataPath) {
#ifndef __OPTIMIZE__
	ofLogWarning("ofxFaceTracker2")<<"Warning, the facetracker background thread runs very slowly in debug mode!";
#endif

	// Add listener for app exit to properly close thread
	ofAddListener(ofEvents().exit, this, &ofxFaceTracker2::exitEvent);

	// Get dlib's frontal face detector
	faceDetector = dlib::get_frontal_face_detector();

	// Load landmark data file
	ofFile dataFile = ofFile(dataPath);
	if(dataFile.exists()){
		dlib::deserialize(dataFile.path()) >> landmarkDetector;
	} else {
        ofLogError("ofxFaceTracker2")<<"shape_predictor_68_face_landmarks.dat data file not found at "<<dataFile.getAbsolutePath()<<"."
                                 <<"\n\n"
                                 <<"Please download and extract it from http://sourceforge.net/projects/dclib/files/dlib/v18.10/shape_predictor_68_face_landmarks.dat.bz2";
		throw;
	}
    
	thread_fps = 0;
    
    

	// Start the background thread
	if(threaded){
		startThread();
	}
    
    // Setup tracker keeping persistent id's of rectangles
    faceRectanglesTracker.setMaximumDistance(200);
}

// ----------------

ofxFaceTracker2::~ofxFaceTracker2(){
	if(isThreadRunning()) {
		ofLogError("ofxFaceTrackerThreaded")<<"Tracker was not stopped. You must call the trackers stop() before destroying the tracker object.";
	}
}

// ----------------

void ofxFaceTracker2::stop(){
	if(isThreadRunning()){
		waitForThread();
		stopThread();
	}
}

// ----------------
// Called on exit event of app, makes sure thread closes down properly
void ofxFaceTracker2::exitEvent(ofEventArgs& e){
    ofRemoveListener(ofEvents().exit, this, &ofxFaceTracker2::exitEvent);
    stop();
}


// ----------------

bool ofxFaceTracker2::update(Mat image, cv::Rect _roi) {
	clock_t start = clock() ;
    
    // Prepare image, resize if required
	float aspect = (float)image.rows/image.cols;
	if(landmarkDetectorImageSize == -1 || image.rows*image.cols <= landmarkDetectorImageSize) {
		im = image;
	} else {
		float scale = sqrt((float) landmarkDetectorImageSize / (image.rows*image.cols));
		resize(image, im, cv::Size(), scale,scale, cv::INTER_NEAREST);
	}
    
    // Rotate image if required
	if(imageRotation){
		rotate_90n(im, im, imageRotation);
	}

    // Update info object
    if(info.inputWidth != image.cols || info.inputHeight != image.rows || info.imageRotation != imageRotation){
        info = ofxFaceTracker2InputInfo(image.cols, image.rows, im.cols, im.rows, imageRotation);
    }

	if(threaded) mutex.lock();

	roi = _roi;
	if(roi.width == 0 && roi.height == 0){
		roi = cv::Rect(0, 0, im.cols, im.rows);
	}

    // Do color convertion to grayscale if required
    if(im.type() == CV_8UC1) {
        im.copyTo(gray);
    } else {
        cvtColor(im, gray, CV_RGB2GRAY);
    }
	imageDirty = true;
    
    
    // If the tracker runs without background thread, then run face detector now
    if(!threaded){
        runFaceDetector(false);
	}

    // Run the landmark detector
    runLandmarkDetector();

	if(threaded) mutex.unlock();

	return !failed;
}

// ----------------

void ofxFaceTracker2::threadedFunction(){
	while(isThreadRunning()) {
		if(imageDirty){
            clock_t start = clock() ;
            
            imageDirty = false;
            
            // Run the detector
            runFaceDetector(true);
            
            // Calculate thread fps
            clock_t end = clock() ;
            double elapsed_time = (end-start)/(double)CLOCKS_PER_SEC ;
            thread_fps = 1.0/elapsed_time;
        }
        //yield();
        sleep(5);
    }
}

// ----------------

void ofxFaceTracker2::runFaceDetector(bool lockMutex){
    cv::Rect detectorRoi;
    
    // Prepare image for face detection
    if(lockMutex) mutex.lock();
    
    // Make sure roi is within boundaries
    detectorRoi =       roi;
    detectorRoi.x =     (int) ofClamp(detectorRoi.x, 0, gray.cols);
    detectorRoi.y =     (int) ofClamp(detectorRoi.y, 0, gray.rows);
    detectorRoi.width = (int) ofClamp(detectorRoi.width, 1, gray.cols - detectorRoi.x);
    detectorRoi.height= (int) ofClamp(detectorRoi.height,1, gray.rows - detectorRoi.y);
    
    float scale = 1;

    // If there is no resize, or if image is below max face detector pixel size, then just crop to ROI size
    if(faceDetectorImageSize == -1 || detectorRoi.width*detectorRoi.height <= faceDetectorImageSize){
        gray(detectorRoi).copyTo(threadGray);
    }
    // Otherwise scale image to fit within max face detector pixel size
    else {
        scale = sqrt((float) faceDetectorImageSize / (detectorRoi.width*detectorRoi.height));
        resize(gray(detectorRoi), threadGray, cv::Size(), scale, scale, cv::INTER_NEAREST);
    }
    
    if(lockMutex) mutex.unlock();
    
    // Run face detector (the slow part)
    dlib::cv_image<unsigned char> cvimg(threadGray);
    std::vector<dlib::rectangle> detectedFaceRectangles = faceDetector(cvimg);
    
    if(lockMutex) mutex.lock();
    
    vector<cv::Rect> rects;
    // Store face detector data
    float s = 1.0f/scale;
    for(auto rect : detectedFaceRectangles){
        rects.push_back(cv::Rect(detectorRoi.x + rect.left() * s,
                                 detectorRoi.y + rect.top()  * s,
                                 rect.width()  * s,
                                 rect.height() * s));
    }
    faceRectanglesTracker.track(rects);
    
    if(lockMutex) mutex.unlock();
}

// ----------------

void ofxFaceTracker2::runLandmarkDetector(){
    failed = true;
    numFaces = 0;
    instances.clear();

    if(faceRectanglesTracker.getCurrentLabels().size() != 0){
        dlib::cv_image<unsigned char> dlibimg(gray);

        instances.reserve(faceRectanglesTracker.getCurrentLabels().size());
        
        for(auto label : faceRectanglesTracker.getCurrentLabels()){
            auto cvrect = faceRectanglesTracker.getCurrent(label);
            auto rect = dlib::rectangle(cvrect.x, cvrect.y, cvrect.x + cvrect.width, cvrect.y + cvrect.height);
            
            // Do the actual landmark detection
            dlib::full_object_detection shape = landmarkDetector(dlibimg, rect);

            instances.push_back(ofxFaceTracker2Instance(label, shape, rect, info));
        }
        
        failed = false;
        numFaces = faceRectanglesTracker.getCurrentLabels().size();
    }
}


const vector<ofxFaceTracker2Instance> & ofxFaceTracker2::getInstances() const{
    return instances;
}
vector<ofxFaceTracker2Instance> & ofxFaceTracker2::getInstances(){
    return instances;
}


// ----------------

void ofxFaceTracker2::drawDebug(int x, int y) const{
    drawDebug(x, y, info.inputWidth, info.inputHeight);
}

void ofxFaceTracker2::drawDebug(int x, int y, int _w, int _h) const{
    if(failed) {
        return;
    }
    
    ofPushMatrix();
    ofPushStyle(); {
        
        ofTranslate(x,y);
        
        if(ofGetRectMode() == OF_RECTMODE_CENTER){
            ofTranslate(-_w/2, -_h/2);
        }
        
        if(_w != info.inputWidth || _h != info.inputHeight){
            ofScale((float)_w/info.inputWidth, (float)_h/info.inputHeight);
        }
        
        for (auto instance : getInstances()){
            ofNoFill();
            
            instance.getLandmarks().getImageFeature(ofxFaceTracker2Landmarks::LEFT_EYE).draw();
            instance.getLandmarks().getImageFeature(ofxFaceTracker2Landmarks::RIGHT_EYE).draw();
            instance.getLandmarks().getImageFeature(ofxFaceTracker2Landmarks::LEFT_EYEBROW).draw();
            instance.getLandmarks().getImageFeature(ofxFaceTracker2Landmarks::RIGHT_EYEBROW).draw();
            instance.getLandmarks().getImageFeature(ofxFaceTracker2Landmarks::NOSE_BRIDGE).draw();
            instance.getLandmarks().getImageFeature(ofxFaceTracker2Landmarks::NOSE_BASE).draw();
            instance.getLandmarks().getImageFeature(ofxFaceTracker2Landmarks::INNER_MOUTH).draw();
            instance.getLandmarks().getImageFeature(ofxFaceTracker2Landmarks::OUTER_MOUTH).draw();
            instance.getLandmarks().getImageFeature(ofxFaceTracker2Landmarks::JAW).draw();
            
            auto rect = instance.getBoundingBox();
            auto p = rect.getTopLeft();
            ofSetColor(255);
            ofDrawBitmapStringHighlight("face "+ofToString(instance.getLabel()), p.x+4, p.y+14);
            
            ofPushStyle();
            ofSetColor(255,0,0);
            ofNoFill();
            ofDrawRectangle(rect);
            ofPopStyle();
        }
        
    } ofPopStyle();
    ofPopMatrix();
}

void ofxFaceTracker2::drawDebugPose() {
    for(auto instance : getInstances()){
        ofPushView();
        ofPushStyle();
        instance.loadPoseMatrix();
        
        ofSetColor(255,0,0);
        ofDrawLine(0,0,0, 100,0,0);
        ofSetColor(0,255,0);
        ofDrawLine(0,0,0, 0,100,0);
        ofSetColor(0,0,255);
        ofDrawLine(0,0,0, 0,0,-100);
        ofPopStyle();
        ofPopView();
    }
}

void ofxFaceTracker2::setFaceOrientation(ofOrientation orientation){
	this->imageRotation = ofOrientationToDegrees(orientation);
}

void ofxFaceTracker2::setFaceRotation(float rotation){
	this->imageRotation = rotation;
}

void ofxFaceTracker2::setFaceDetectorImageSize(int numPixels) {
	this->faceDetectorImageSize = numPixels;
}

void ofxFaceTracker2::setLandmarkDetectorImageSize(int numPixels){
	this->landmarkDetectorImageSize = numPixels;
}

void ofxFaceTracker2::setThreaded(bool threaded){
	this->threaded = threaded;
}

int ofxFaceTracker2::getThreadFps()const{
	return thread_fps;
}

int ofxFaceTracker2::size() const {
	return instances.size();
}


void ofxFaceTracker2::rotate_90n(cv::Mat &src, cv::Mat &dst, int angle)
{
	if(angle == 270 || angle == -90){
		// Rotate clockwise 270 degrees
		cv::flip(src.t(), dst, 0);
	}else if(angle == 180 || angle == -180){
		// Rotate clockwise 180 degrees
		cv::flip(src, dst, -1);
	}else if(angle == 90 || angle == -270){
		// Rotate clockwise 90 degrees
		cv::flip(src.t(), dst, 1);
	}else if(angle == 360 || angle == 0){
		if(src.data != dst.data){
			src.copyTo(dst);
		}
	}
}
