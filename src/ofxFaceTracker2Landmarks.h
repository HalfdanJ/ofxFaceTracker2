#pragma once
//#include "ofConstants.h"
//#include "ofGraphics.h"
#include "ofMain.h"
#include "ofxCv.h"

#include "ofxFaceTracker2InputInfo.h"

#include <dlib/image_processing.h>

class ofxFaceTracker2Landmarks {
public:
    
    enum Feature {
        LEFT_EYE_TOP, RIGHT_EYE_TOP,
        
        LEFT_EYEBROW, RIGHT_EYEBROW,
        LEFT_EYE, RIGHT_EYE,
        LEFT_JAW, RIGHT_JAW, JAW,
        OUTER_MOUTH, INNER_MOUTH,
        NOSE_BRIDGE, NOSE_BASE,
        FACE_OUTLINE, ALL_FEATURES
    };
    
    ofxFaceTracker2Landmarks(dlib::full_object_detection shape, ofxFaceTracker2InputInfo & info);
    
    /// Get specific 2D image point from the 68 landmarks coordinates
    /// Coordinate is returned in input image size
    glm::vec2 getImagePoint(int i) const;
    
    /// Get a list of 2D points with all 68 face landmarks found.
    /// Coordinates are returned in input image size
    std::vector<glm::vec2> getImagePoints() const;
    
    /// Like getImagePoints, just returns the points as cv:Point2f
    std::vector<cv::Point2f> getCvImagePoints() const;
    
    /// Get poly line of a feature
    ofPolyline getImageFeature(Feature feature) const;
    
    ofMesh getImageMesh() const;
    
    template <class T> ofMesh getMesh(std::vector<T> points) const;
    
private:
    dlib::full_object_detection shape;
    ofxFaceTracker2InputInfo & info;
    
    static std::vector<int> consecutive(int start, int end);
    static  std::vector<int> getFeatureIndices(Feature feature);
    
    template <class T>
    ofPolyline getFeature(Feature feature,  std::vector<T> points) const;
    
};
