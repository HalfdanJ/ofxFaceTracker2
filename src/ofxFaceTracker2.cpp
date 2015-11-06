#include "ofxFaceTracker2.h"


using namespace ofxCv;
using namespace cv;

// can be compiled with OpenMP for even faster threaded execution

#define it at<int>
#define db at<double>


ofxFaceTracker2::ofxFaceTracker2()
:rescale(1)
,landmarkRescale(1)
,failed(true)
,threaded(true)
,rotation(0)
{
}

ofxFaceTracker2::~ofxFaceTracker2(){
    if(isThreadRunning()) {
        ofLog(OF_LOG_ERROR, "ofxFaceTrackerThreaded :: Thread was not stopped. You must call the trackers waitForThread() in ofApp::exit() or exit() of class that holds this object.");
    }
}

void ofxFaceTracker2::setup() {
    detector = dlib::get_frontal_face_detector();
    
    dlib::deserialize(ofToDataPath("shape_predictor_68_face_landmarks.dat",true)) >> sp;
    
    fps = thread_fps = 0;
    
    if(threaded){
        startThread(false);
    }
}

bool ofxFaceTracker2::update(Mat image) {
    clock_t start = clock() ;

  
	if(rescale == 1) {
		im = image;
	} else {
		resize(image, im, cv::Size(landmarkRescale * image.cols, landmarkRescale * image.rows));
	}
    
    if(rotation){
        rotate_90n(im, im, rotation);
    }
    
    w = im.cols;
    h = im.rows;

    if(threaded){
        mutex.lock();
    }
    
	if(im.type() == CV_8UC3) {
		cvtColor(im, gray, CV_RGB2GRAY);
	} else if(im.type() == CV_8UC1) {
		im.copyTo(gray);
	}
    imageDirty = true;
    
//    dlib::cv_image<dlib::rgb_pixel> cvimg(im);

    dlib::cv_image<unsigned char> cvimg(gray);
    if(!threaded){
        facesRects = detector(cvimg);
    }
    
    if(facesRects.size() == 0){
        failed = true;
    } else {
        failed = false;
        
        facesObjects.clear();
        for (unsigned long j = 0; j < facesRects.size(); ++j)
        {
            dlib::full_object_detection shape = sp(cvimg, facesRects[j]);
            facesObjects.push_back(shape);
        }
    }
    
    if(threaded){
        mutex.unlock();
    }

    clock_t end = clock() ;
    double elapsed_time = (end-start)/(double)CLOCKS_PER_SEC ;
    fps = 1.0/elapsed_time;
    
    /*
    frames_one_sec++;
    if( ofGetElapsedTimeMillis() - one_second_time >= 1000){
        fps = frames_one_sec;
        thread_fps = thread_frames_one_sec;
        frames_one_sec = thread_frames_one_sec = 0;
        one_second_time = ofGetElapsedTimeMillis();
    }*/
    
    return !failed;
}

void ofxFaceTracker2::rotate_90n(cv::Mat &src, cv::Mat &dst, int angle)
{
    //dst.create(src.rows, src.cols, src.type());
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

void ofxFaceTracker2::threadedFunction(){
    while(isThreadRunning()) {
        if(imageDirty){
            clock_t start = clock() ;

            double s = landmarkRescale / rescale;

            imageDirty = false;
            mutex.lock();
            if(s == 1){
                gray.copyTo(threadGray);
            } else {
                resize(gray, threadGray, cv::Size(1.0/s * gray.cols, 1.0/s * gray.rows));
            }
            
            
            mutex.unlock();
            
            dlib::cv_image<unsigned char> cvimg(threadGray);
            std::vector<dlib::rectangle> _dets = detector(cvimg);
            
            mutex.lock();
            if(s != 1){
                facesRects.clear();
                for(int i=0;i<_dets.size();i++){
                    facesRects.push_back(dlib::rectangle( _dets[i].left()*s,
                                                         _dets[i].top()*s,
                                                         _dets[i].right()*s,
                                                         _dets[i].bottom()*s));
                }
            } else {
                facesRects = _dets;
            }
            mutex.unlock();
            
            clock_t end = clock() ;
            double elapsed_time = (end-start)/(double)CLOCKS_PER_SEC ;
            thread_fps = 1.0/elapsed_time;
            
            
        }
        yield();
    }
}
void ofxFaceTracker2::draw(int x, int y) const{
    draw(x,y, w, h);
}
void ofxFaceTracker2::draw(int x, int y, int _w, int _h) const{
    if(failed) {
        return;
    }
    
    ofPushMatrix();
    ofTranslate(x,y);
    
    if(ofGetRectMode() == OF_RECTMODE_CENTER){
        ofTranslate(-_w/2, -_h/2);
    }
    
    if(_w != w || _h != h){
        ofScale((float)_w/w, (float)_h/h);
    }
    
    ofPushStyle();
    for (int j = 0; j < numFaces(); ++j){
        ofNoFill();

		ofPushStyle();
		ofSetRectMode(OF_RECTMODE_CORNER);
        ofDrawRectangle(facesRects[j].tl_corner().x() / landmarkRescale,
                        facesRects[j].tl_corner().y() / landmarkRescale,
                        facesRects[j].width() / landmarkRescale,
                        facesRects[j].height() / landmarkRescale);
        ofPopStyle();
        
        getImageFeature(LEFT_EYE).draw();
        getImageFeature(RIGHT_EYE).draw();
        getImageFeature(LEFT_EYEBROW).draw();
        getImageFeature(RIGHT_EYEBROW).draw();
        getImageFeature(NOSE_BRIDGE).draw();
        getImageFeature(NOSE_BASE).draw();
        getImageFeature(INNER_MOUTH).draw();
        getImageFeature(OUTER_MOUTH).draw();
        getImageFeature(JAW).draw();
    }
    
    ofPopStyle();
    ofPopMatrix();
}

void ofxFaceTracker2::setRotation(int rotation){
    this->rotation = rotation;
}

void ofxFaceTracker2::setRescale(float rescale) {
    this->rescale = rescale;
}

void ofxFaceTracker2::setLandmarkRescale(float rescale){
    this->landmarkRescale = rescale;
}

int ofxFaceTracker2::getFps()const{
    return fps;
}
int ofxFaceTracker2::getThreadFps()const{
    return thread_fps;
}

int ofxFaceTracker2::size() const {
    if(failed) return 0;
    return facesObjects[0].num_parts();
    
}

int ofxFaceTracker2::numFaces()const{
    return facesRects.size();
}

ofVec2f ofxFaceTracker2::getImagePoint(int i, int face) const {
    if(failed) {
        return ofVec2f();
    }
    
    return ofVec2f(facesObjects[face].part(i).x() / landmarkRescale,
                   facesObjects[face].part(i).y() / landmarkRescale);
}

vector<ofVec2f> ofxFaceTracker2::getImagePoints(int face) const {
    int n = size();
    vector<ofVec2f> imagePoints(n);
    for(int i = 0; i < n; i++) {
        imagePoints[i] = getImagePoint(i);
    }
    return imagePoints;
}


ofPolyline ofxFaceTracker2::getImageFeature(Feature feature, int face) const {
    return getFeature(feature, getImagePoints(face));
}

vector<int> ofxFaceTracker2::consecutive(int start, int end) {
    int n = end - start;
    vector<int> result(n);
    for(int i = 0; i < n; i++) {
        result[i] = start + i;
    }
    return result;
}

vector<int> ofxFaceTracker2::getFeatureIndices(Feature feature) {
    switch(feature) {
        case LEFT_EYE_TOP: return consecutive(36, 40);
        case RIGHT_EYE_TOP: return consecutive(42, 46);
        case LEFT_JAW: return consecutive(0, 9);
        case RIGHT_JAW: return consecutive(8, 17);
        case JAW: return consecutive(0, 17);
        case LEFT_EYEBROW: return consecutive(17, 22);
        case RIGHT_EYEBROW: return consecutive(22, 27);
        case LEFT_EYE: return consecutive(36, 42);
        case RIGHT_EYE: return consecutive(42, 48);
        case OUTER_MOUTH: return consecutive(48, 60);
        case INNER_MOUTH: {
            static int innerMouth[] = {48,60,61,62,54,63,64,65};
            return vector<int>(innerMouth, innerMouth + 8);
        }
        case NOSE_BRIDGE: return consecutive(27, 31);
        case NOSE_BASE: return consecutive(31, 36);
        case FACE_OUTLINE: {
            static int faceOutline[] = {17,18,19,20,21,22,23,24,25,26, 16,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0};
            return vector<int>(faceOutline, faceOutline + 27);
        }
        case ALL_FEATURES: return consecutive(0, 66);
    }
}
