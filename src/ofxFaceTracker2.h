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
    
	bool update(cv::Mat image);

    template <class T>
    bool update(T& image){
        update(ofxCv::toCv(image));
    }
    
    void draw(int x=0, int y=0, int w=-1, int h=-1) const;
    void drawPose(int face=0);
    
    void stop();
    
	//virtual void reset();
	
    /// Get number of detected faces
	int size() const;
    
    int getThreadFps()const;
    //bool getFound() const;
	/*bool getHaarFound() const;
	int getAge() const;
	virtual bool getVisibility(int i) const;*/
    
	vector<ofVec2f> getImagePoints(int face=0) const;
    vector<cv::Point2f> getCvImagePoints(int face=0) const;
/*vector<ofVec3f> getObjectPoints() const;
	vector<ofVec3f> getMeanObjectPoints() const;
	*/
	virtual ofVec2f getImagePoint(int i, int face=0) const;
	/*virtual ofVec3f getObjectPoint(int i) const;
	virtual ofVec3f getMeanObjectPoint(int i) const;
	*/
	ofMesh getImageMesh() const;
	/*ofMesh getObjectMesh() const;
	ofMesh getMeanObjectMesh() const;*/
	template <class T> ofMesh getMesh(vector<T> points) const;
	/*
	virtual const cv::Mat& getObjectPointsMat() const;
	
	virtual ofRectangle getHaarRectangle() const;
	virtual ofVec2f getPosition() const; // pixels
	virtual float getScale() const; // arbitrary units
	virtual ofVec3f getOrientation() const; // radians
	ofMatrix4x4 getRotationMatrix() const;
	
	enum Direction {
		FACING_FORWARD,
		FACING_LEFT, FACING_RIGHT,
		FACING_UNKNOWN
	};
	Direction getDirection() const;
	*/
	enum Feature {
        LEFT_EYE_TOP, RIGHT_EYE_TOP,
        
		LEFT_EYEBROW, RIGHT_EYEBROW,
		LEFT_EYE, RIGHT_EYE,
		LEFT_JAW, RIGHT_JAW, JAW,
		OUTER_MOUTH, INNER_MOUTH,
		NOSE_BRIDGE, NOSE_BASE,
		FACE_OUTLINE, ALL_FEATURES
	};
	ofPolyline getImageFeature(Feature feature, int face=0) const;
	/*ofPolyline getObjectFeature(Feature feature) const;
	ofPolyline getMeanObjectFeature(Feature feature) const;
	
	enum Gesture {
		MOUTH_WIDTH, MOUTH_HEIGHT,
		LEFT_EYEBROW_HEIGHT, RIGHT_EYEBROW_HEIGHT,
		LEFT_EYE_OPENNESS, RIGHT_EYE_OPENNESS,
		JAW_OPENNESS,
		NOSTRIL_FLARE
	};
	float getGesture(Gesture gesture) const;
	*/
    
    ofNode getPoseNode(int face=0);
    ofVec2f transformPosePosition(ofVec3f p, int face=0);
    void applyPoseMatrix(int face=0);

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
    
/*	void setIterations(int iterations);
	void setClamp(float clamp);
	void setTolerance(float tolerance);
	void setAttempts(int attempts);
	void setUseInvisible(bool useInvisible);
	void setHaarMinSize(float minSize);
	*/
protected:
    void calculatePoseMatrix(int face=0);
    bool poseCalculated[MAX_FACES];
    cv::Mat1d poseProjection[MAX_FACES];
    ofxCv::Intrinsics intrinsics[MAX_FACES];
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
    
//	void addTriangleIndices(ofMesh& mesh) const;
    template <class T> ofPolyline getFeature(Feature feature, vector<T> points) const;
    
    void exitEvent(ofEventArgs& e);
    
	bool failed;

    int imageRotation;

    int faceDetectorImageSize;
    int landmarkDetectorImageSize;
    int inputWidth, inputHeight;
    
    int numFaces;
    
    ofMatrix4x4 landmarkRotationMatrix;
/*	int frameSkip;
	
	vector<int> wSize1, wSize2;
	int iterations;
	int attempts;
	double clamp, tolerance;
	bool useInvisible;
	
	FACETRACKER::Tracker tracker;
	cv::Mat tri, con;
	*/
	cv::Mat im, gray;
    cv::Mat threadGray;
/*	cv::Mat objectPoints;
     */
    
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

