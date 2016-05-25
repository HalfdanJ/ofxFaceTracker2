#pragma once
#include "ofConstants.h"
#include "ofGraphics.h"
#include "ofxCv.h"

#include "ofxFaceTracker2Landmarks.h"

#include <dlib/image_processing.h>


class ofxFaceTracker2Instance {
public:
    ofxFaceTracker2Instance(int label,
                            dlib::full_object_detection shape,
                            dlib::rectangle rectangle,
                            ofxFaceTracker2InputInfo & info);
    
    /// Returns unique label for face. Labels are persistent until face disappears
    int getLabel();
    
    /// Transforms a 3D point in pose coordinate space to 2D point in screen space
    ofVec2f transformPosePosition(ofVec3f p);
    
    /// Load the pose matrix into OpenGL, this allows you to draw 3D objects in the heads coordinate system
    void loadPoseMatrix();
    
    /// Load the pose OpenGL projection matrix
    void loadPoseProjectionMatrix();
    
    /// Get the matrix for the heads pose
    ofMatrix4x4 getPoseMatrix();
    
    /// Get the bounding box
    ofRectangle getBoundingBox() const;
    
    /// Returns landmarks object for accessing landmark details
    ofxFaceTracker2Landmarks & getLandmarks();
    
private:
    int label;
    ofxFaceTracker2Landmarks landmarks;
    dlib::rectangle rectangle;
    
    bool poseCalculated;
    cv::Mat1d poseProjection;
    cv::Mat poservec, posetvec;
    ofxFaceTracker2InputInfo & info;
    
    void calculatePoseMatrix();
};

