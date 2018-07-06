#include "ofxFaceTracker2Landmarks.h"
using namespace std;

ofxFaceTracker2Landmarks::ofxFaceTracker2Landmarks(dlib::full_object_detection shape, ofxFaceTracker2InputInfo & info) : shape(shape), info(info){
}




glm::vec2 ofxFaceTracker2Landmarks::getImagePoint(int i) const {
    glm::vec4 p = glm::vec4(shape.part(i).x(), shape.part(i).y(), 0.f, 1.f);
    p = info.rotationMatrix * p;

    return glm::vec2(p.x, p.y);
}

vector<glm::vec2> ofxFaceTracker2Landmarks::getImagePoints() const {
    int n = shape.num_parts();
    vector<glm::vec2> imagePoints(n);
    for(int i = 0; i < n; i++) {
        imagePoints[i] = getImagePoint(i);
    }
    return imagePoints;
}

vector<cv::Point2f> ofxFaceTracker2Landmarks::getCvImagePoints() const {
    int n = shape.num_parts();
    vector<cv::Point2f> imagePoints(n);
    for(int i = 0; i < n; i++) {
        imagePoints[i] = ofxCv::toCv(getImagePoint(i));
    }
    return imagePoints;
}



ofPolyline ofxFaceTracker2Landmarks::getImageFeature(Feature feature) const {
    return getFeature(feature, getImagePoints());
}

ofMesh ofxFaceTracker2Landmarks::getImageMesh() const{
    return getMesh(getCvImagePoints());
}


vector<int> ofxFaceTracker2Landmarks::getFeatureIndices(Feature feature) {
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
        case INNER_MOUTH: return consecutive(60, 68);
        case NOSE_BRIDGE: return consecutive(27, 31);
        case NOSE_BASE: return consecutive(31, 36);
        case FACE_OUTLINE: {
            static int faceOutline[] = {17,18,19,20,21,22,23,24,25,26, 16,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0};
            return vector<int>(faceOutline, faceOutline + 27);
        }
        case ALL_FEATURES: return consecutive(0, 68);
    }
}


template <class T>
ofPolyline ofxFaceTracker2Landmarks::getFeature(Feature feature, vector<T> points) const {
    ofPolyline polyline;
    vector<int> indices = getFeatureIndices(feature);
    for(int i = 0; i < indices.size(); i++) {
        int cur = indices[i];
        glm::vec2 pt = toGlm(points[cur]);
        polyline.addVertex({points[cur].x, points[cur].y, 0.f});
    }
    switch(feature) {
        case LEFT_EYE:
        case RIGHT_EYE:
        case OUTER_MOUTH:
        case INNER_MOUTH:
        case FACE_OUTLINE:
            polyline.close();
            break;
        default:
            break;
    }
    
    return polyline;
}

vector<int> ofxFaceTracker2Landmarks::consecutive(int start, int end) {
    int n = end - start;
    vector<int> result(n);
    for(int i = 0; i < n; i++) {
        result[i] = start + i;
    }
    return result;
}

template <class T>
ofMesh ofxFaceTracker2Landmarks::getMesh(vector<T> points) const {
    cv::Rect rect(0, 0, info.inputWidth, info.inputHeight);
    cv::Subdiv2D subdiv(rect);
    
    for(int i=0;i<points.size();i++){
        if( rect.contains(points[i]) ){
            subdiv.insert(points[i]);
        }
    }
    
    vector<cv::Vec6f> triangleList;
    subdiv.getTriangleList(triangleList);
    
    ofMesh mesh;
    mesh.setMode(OF_PRIMITIVE_TRIANGLES);
    
    for( size_t i = 0; i < triangleList.size(); i++ )
    {
        cv::Vec6f t = triangleList[i];
        
        cv::Point2f pt1 = cv::Point(cvRound(t[0]), cvRound(t[1]));
        cv::Point2f pt2 = cv::Point(cvRound(t[2]), cvRound(t[3]));
        cv::Point2f pt3 = cv::Point(cvRound(t[4]), cvRound(t[5]));
        
        // Draw rectangles completely inside the image.
        if ( rect.contains(pt1) && rect.contains(pt2) && rect.contains(pt3))
        {
            glm::vec2 pt1V = ofxCv::toOf(pt1);
            glm::vec2 pt2V = ofxCv::toOf(pt2);
            glm::vec2 pt3V = ofxCv::toOf(pt3);

            mesh.addVertex({pt1.x, pt1.y, 0.f});
            mesh.addVertex({pt2.x, pt2.y, 0.f});
            mesh.addVertex({pt3.x, pt3.y, 0.f});
        }
    }
    return mesh;
    
}

