#include "ofxFaceTracker2Instance.h"


ofxFaceTracker2Instance::ofxFaceTracker2Instance(int label,
                                                 dlib::full_object_detection shape,
                                                 dlib::rectangle rectangle,
                                                 ofxFaceTracker2InputInfo & info)
: landmarks(ofxFaceTracker2Landmarks(shape, info))
, info(info)
{
    this->label = label;
    this->rectangle = rectangle;
    
    
    poseCalculated = false;
}



ofxFaceTracker2Landmarks & ofxFaceTracker2Instance::getLandmarks(){
    return landmarks;
}

ofRectangle ofxFaceTracker2Instance::getBoundingBox() const {
    return ofRectangle(rectangle.left(),
                                  rectangle.top(),
                                  rectangle.width(),
                                  rectangle.height());
}

int ofxFaceTracker2Instance::getLabel(){
    return label;
}

ofVec2f ofxFaceTracker2Instance::transformPosePosition(ofVec3f p){
    if(!poseCalculated){
        calculatePoseMatrix();
    }
    std::vector<cv::Point3f> axes;
    axes.push_back(ofxCv::toCv(p));
    
    std::vector<cv::Point2f> projected_axes;
    cv::projectPoints(axes, poservec, posetvec, poseProjection, cv::noArray(), projected_axes);
    
    return ofxCv::toOf(projected_axes[0]);
}

ofMatrix4x4 ofxFaceTracker2Instance::getPoseMatrix(){
    if(!poseCalculated){
        calculatePoseMatrix();
    }
    
    
    ofMatrix4x4 matrix = ofxCv::makeMatrix(poservec, posetvec);
    matrix.scale(-1, 1, 1);
    return matrix;
}

void ofxFaceTracker2Instance::loadPoseMatrix(){
    if(!poseCalculated){
        calculatePoseMatrix();
    }
    
    ofMatrix4x4 matrix = getPoseMatrix();
    
    loadPoseProjectionMatrix();
    ofLoadMatrix(matrix);
}

void ofxFaceTracker2Instance::loadPoseProjectionMatrix(){
    info.intrinsics.loadProjectionMatrix(1, 200000);
}

// Estimates the heads 3d position and orientation
void ofxFaceTracker2Instance::calculatePoseMatrix(){
    enum FACIAL_FEATURE {
        NOSE=30,
        RIGHT_EYE=36,
        LEFT_EYE=45,
        RIGHT_SIDE=0,
        LEFT_SIDE=16,
        EYEBROW_RIGHT=21,
        EYEBROW_LEFT=22,
        MOUTH_UP=51,
        MOUTH_DOWN=57,
        MOUTH_RIGHT=48,
        MOUTH_LEFT=54,
        SELLION=27,
        MOUTH_CENTER_TOP=62,
        MOUTH_CENTER_BOTTOM=66,
        MENTON=8
    };
    
    
    // Anthropometric for male adult
    // Relative position of various facial feature relative to sellion
    // Values taken from https://en.wikipedia.org/wiki/Human_head
    // Z points forward
    const cv::Point3f P3D_SELLION(0., 0.,0.);
    const cv::Point3f P3D_RIGHT_EYE(-65.5, -5.,-20.);
    const cv::Point3f P3D_LEFT_EYE(65.5, -5.,-20.);
    const cv::Point3f P3D_RIGHT_EAR( -77.5,-6.,-100.);
    const cv::Point3f P3D_LEFT_EAR(77.5,-6.,-100.);
    const cv::Point3f P3D_NOSE( 0.,-48.0,21.0);
    const cv::Point3f P3D_STOMMION( 0.,-75.0,10.0);
    const cv::Point3f P3D_MENTON( 0.,-133.0, 0.);
    
    float aov = 280;
    float focalLength = info.inputWidth * ofDegToRad(aov);
    float opticalCenterX = info.inputWidth/2;
    float opticalCenterY = info.inputHeight/2;
    
    cv::Mat1d projectionMat = cv::Mat::zeros(3,3,CV_32F);
    poseProjection = projectionMat;
    poseProjection(0,0) = focalLength;
    poseProjection(1,1) = focalLength;
    poseProjection(0,2) = opticalCenterX;
    poseProjection(1,2) = opticalCenterY;
    poseProjection(2,2) = 1;
    
    std::vector<cv::Point3f> head_points;
    
    head_points.push_back(P3D_SELLION);
    head_points.push_back(P3D_RIGHT_EYE);
    head_points.push_back(P3D_LEFT_EYE);
    head_points.push_back(P3D_RIGHT_EAR);
    head_points.push_back(P3D_LEFT_EAR);
    head_points.push_back(P3D_MENTON);
    head_points.push_back(P3D_NOSE);
    head_points.push_back(P3D_STOMMION);
    
    std::vector<cv::Point2f> detected_points;
    detected_points.push_back(ofxCv::toCv(getLandmarks().getImagePoint(SELLION)));
    detected_points.push_back(ofxCv::toCv(getLandmarks().getImagePoint(RIGHT_EYE)));
    detected_points.push_back(ofxCv::toCv(getLandmarks().getImagePoint(LEFT_EYE)));
    detected_points.push_back(ofxCv::toCv(getLandmarks().getImagePoint(RIGHT_SIDE)));
    detected_points.push_back(ofxCv::toCv(getLandmarks().getImagePoint(LEFT_SIDE)));
    detected_points.push_back(ofxCv::toCv(getLandmarks().getImagePoint(MENTON)));
    detected_points.push_back(ofxCv::toCv(getLandmarks().getImagePoint(NOSE)));
    
    auto stomion = (ofxCv::toCv(getLandmarks().getImagePoint(MOUTH_CENTER_TOP))
                    + ofxCv::toCv(getLandmarks().getImagePoint(MOUTH_CENTER_BOTTOM))) * 0.5;
    detected_points.push_back(stomion);
    
    // Find the 3D pose of our head
    solvePnP(head_points, detected_points,
             poseProjection, cv::noArray(),
             poservec, posetvec, false,
#ifdef OPENCV3
             cv::SOLVEPNP_ITERATIVE);
#else
			 CV_ITERATIVE);
#endif
    
    // Black magic: The x axis in the rotation vector needs to get flipped.
    double * r = poservec.ptr<double>(0) ;
    r[0] *= -1;
    r[1] *= -1;
    
    poseCalculated = true;
}
