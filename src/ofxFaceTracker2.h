#pragma once

// Include the parts of openFrameworks that are required
#include "ofConstants.h"
#include "ofThread.h"
#include "ofGraphics.h"

// Include addon dependencies
#include "ofxCv.h"

// Include DLIB
#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/image_processing/generic_image.h>
#include <dlib/image_processing.h>
#include <dlib/serialize.h>
#include <dlib/opencv.h>

#include "ofxFaceTracker2Instance.h"

#define MAX_FACES 20

class ofxFaceTracker2 : public ofThread {
public:
	ofxFaceTracker2();
    ~ofxFaceTracker2();

    void setup(string dataPath="shape_predictor_68_face_landmarks.dat");
    
    /// Update the trackers input image
	bool update(cv::Mat image, cv::Rect roi=cv::Rect(0,0,0,0));

    template <class T>
    bool update(T& image, cv::Rect roi=cv::Rect(0,0,0,0)){
        update(ofxCv::toCv(image), roi);
    }
    
    /// Draw a debug drawing of the detected face
    void draw(int x=0, int y=0) const;
    void draw(int x, int y, int w, int h) const;

    /// Draw a debug drawing of the pose of the detected face
    void drawPose(int face=0);

    /// Stop the background tracker thread (called automatically on app exit)
    void stop();
    
    /// Get number of detected faces
	int size() const;
    
    /// Returns the fps the background tracker thread is running with
    int getThreadFps()const;

    /// Set the image size the facedetector should work on.
    /// The value is the number of pixels the image should be resized to (preserving the aspect ratio)
    /// This is the slowest algorithm, and should be set quite low.
	void setFaceDetectorImageSize(int numPixels);
    
    /// Set the image size of the landmark detector.
    /// The value is the number of pixels the image should be resized to (preserving the aspect ratio)
    /// Can be a lot higher then the face detector
    /// Default is -1, that means no resizing happens (use native resolution from input image)
    void setLandmarkDetectorImageSize(int numPixels);
    
    /// Set the orienation of the faces. Usefull on mobile where the camera is rotated
    void setFaceOrientation(ofOrientation orientation);

    /// Set the rotation in degrees of the faces. Usefull on mobile where the camera is rotated
    void setFaceRotation(float rotation);

    /// Set weather the tracker should run threaded or not
    void setThreaded(bool threaded);
    
    const vector<ofxFaceTracker2Instance> & getInstances() const;
    vector<ofxFaceTracker2Instance> & getInstances();
    
protected:
    vector<ofxFaceTracker2Instance> instances;
    
    ofxCv::Tracker<cv::Rect> faceRectanglesTracker;
    
    void runFaceDetector(bool lockMutex);
    void runLandmarkDetector();
        
    void updateTrackerInstances(vector<dlib::rectangle> rectangles);
    
    void threadedFunction();
    void rotate_90n(cv::Mat &src, cv::Mat &dst, int angle);

    bool threaded;
    ofMutex mutex;
    bool imageDirty;

    dlib::frontal_face_detector faceDetector;
    dlib::shape_predictor sp;
    
    std::vector< dlib::full_object_detection > facesObjects;
    
    void exitEvent(ofEventArgs& e);
    
	bool failed;

    int imageRotation;

    int faceDetectorImageSize;
    int landmarkDetectorImageSize;
    
    int numFaces;
    
    cv::Mat im, gray;
    cv::Mat threadGray;
    int thread_fps;
    
    ofxFaceTracker2InputInfo info;

	cv::Rect roi;
};


