/*
 ofxFaceTracker provides an interface to Jason Saragih's FaceTracker library.
 
 getImagePoint()/getImageMesh() are in image space. This means that all the
 points will line up with the pixel coordinates of the image you fed into
 ofxFaceTracker.
 
 getObjectPoint()/getObjectMesh() are in 3d object space. This is a product of
 the mean mesh with only the expression applied. There is no rotation or
 translation applied to the object space.
 
 getMeanObjectPoint()/getMeanObjectMesh() are also in 3d object space. However,
 there is no expression applied to the mesh.
 */

#pragma once

#include "ofxCv.h"
//#include "ofxDelaunay.h"

#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/image_processing/generic_image.h>
#include <dlib/image_processing.h>
#include <dlib/serialize.h>
#include <dlib/opencv.h>

#define MAX_FACES 20


class ofxFaceTracker2 : public ofThread {
public:
	ofxFaceTracker2();
    ~ofxFaceTracker2();

    void setup();
    
    /// Update the trackers input image
	bool update(cv::Mat image);

    template <class T>
    bool update(T& image){
        update(ofxCv::toCv(image));
    }
    
    /// Draw a debug drawing of the detected face
    void draw(int x=0, int y=0, int w=-1, int h=-1) const;

    /// Draw a debug drawing of the pose of the detected face
    void drawPose(int face=0);

    /// Stop the background tracker thread
    void stop();
    
    /// Get number of detected faces
	int size() const;
    
    /// Returns the fps the background tracker thread is running with
    int getThreadFps()const;

    /// Get a list of 2D points in screen space with all the face landmarks found
	vector<ofVec2f> getImagePoints(int face=0) const;

    /// Like getImagePoints, just returns the points as cv:Point2f
    vector<cv::Point2f> getCvImagePoints(int face=0) const;

    /// Get specific 2D image point
    ofVec2f getImagePoint(int i, int face=0) const;

    
    ofMesh getImageMesh() const;
	
    template <class T> ofMesh getMesh(vector<T> points) const;
	
    
    enum Feature {
        LEFT_EYE_TOP, RIGHT_EYE_TOP,
        
		LEFT_EYEBROW, RIGHT_EYEBROW,
		LEFT_EYE, RIGHT_EYE,
		LEFT_JAW, RIGHT_JAW, JAW,
		OUTER_MOUTH, INNER_MOUTH,
		NOSE_BRIDGE, NOSE_BASE,
		FACE_OUTLINE, ALL_FEATURES
	};

    /// Get poly line of a feature
    ofPolyline getImageFeature(Feature feature, int face=0) const;
    
    /// Transforms a 3D point in pose coordinate space to 2D point in screen space
    ofVec2f transformPosePosition(ofVec3f p, int face=0);
    
    /// Load the pose matrix into OpenGL, this allows you to draw 3D objects in the heads coordinate system
    void loadPoseMatrix(int face=0);
    
    /// Load the pose OpenGL projection matrix
    void loadPoseProjectionMatrix();

    /// Get the matrix for the heads pose
    ofMatrix4x4 getPoseMatrix(int face=0);

    /// Set the image size the facedetector should work on.
    /// The value is the number of pixels the image should be resized to (preserving the aspect ratio)
    /// This is the slowest algorithm, and should be set quite low.
	void setFaceDetectorImageSize(int numPixels);
    
    /// Set the image size of the landmark detector.
    /// The value is the number of pixels the image should be resized to (preserving the aspect ratio)
    /// Can be a lot higher then the face detector
    /// Default is -1, that means no resizing happens (use native resolution from input image)
    void setLandmarkDetectorImageSize(int numPixels);
    
    // Set the orienation of the faces. Usefull on mobile where the camera is rotated
    void setFaceOrientation(ofOrientation orientation);

    // Set the rotation in degrees of the faces. Usefull on mobile where the camera is rotated
    void setFaceRotation(float rotation);
    
protected:
    void calculatePoseMatrix(int face=0);
    void calculateIntrinsics();
    
    bool intrinsicsCalculated;
    
    bool poseCalculated[MAX_FACES];
    cv::Mat1d poseProjection[MAX_FACES];
    ofxCv::Intrinsics intrinsics;
    cv::Mat poservec[MAX_FACES];
    cv::Mat posetvec[MAX_FACES];
    
    static vector<int> getFeatureIndices(Feature feature);
    static std::vector<int> consecutive(int start, int end);
    
    void threadedFunction();
    void rotate_90n(cv::Mat &src, cv::Mat &dst, int angle);

    bool threaded;
    std::mutex mutex;
    bool imageDirty;

    dlib::frontal_face_detector detector;
    dlib::shape_predictor sp;
    
    std::vector<dlib::rectangle> facesRects;
    std::vector< dlib::full_object_detection > facesObjects;
    
    template <class T> ofPolyline getFeature(Feature feature, vector<T> points) const;
    
    void exitEvent(ofEventArgs& e);
    
	bool failed;

    int imageRotation;

    int faceDetectorImageSize;
    int landmarkDetectorImageSize;
    int inputWidth, inputHeight;
    
    int numFaces;
    
    ofMatrix4x4 landmarkRotationMatrix;

    cv::Mat im, gray;
    cv::Mat threadGray;
    int thread_fps;
};

template <class T>
ofPolyline ofxFaceTracker2::getFeature(Feature feature, vector<T> points) const {
	ofPolyline polyline;
	if(!failed) {
		vector<int> indices = getFeatureIndices(feature);
		for(int i = 0; i < indices.size(); i++) {
			int cur = indices[i];
//			if(useInvisible || getVisibility(cur)) {
				polyline.addVertex(points[cur]);
//			}
		}
		switch(feature) {
			case LEFT_EYE:
			case RIGHT_EYE:
			case OUTER_MOUTH:
			case INNER_MOUTH:
			case FACE_OUTLINE:
				polyline.close();
		}
	}
	return polyline;
}

