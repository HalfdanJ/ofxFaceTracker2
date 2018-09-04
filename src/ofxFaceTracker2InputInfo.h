#pragma once

#include "ofConstants.h"
#include "glm/glm.hpp"

#include "ofxCv.h"

class ofxFaceTracker2InputInfo {
public:
    int inputWidth, inputHeight;
    int landmarkWidth, landmarkHeight;
    int imageRotation;
    
    // Parameters calculated automatically
    glm::mat4 rotationMatrix;
    ofxCv::Intrinsics intrinsics;
    
    // Constructor
    ofxFaceTracker2InputInfo(){};
    
    ofxFaceTracker2InputInfo(int inputWidth, int inputHeight,int landmarkWidth, int landmarkHeight, int imageRotation)
    : inputWidth(inputWidth)
    , inputHeight(inputHeight)
    , landmarkWidth(landmarkWidth)
    , landmarkHeight(landmarkHeight)
    , imageRotation(imageRotation) {
        // Calculate rotation matrix
        if(imageRotation == 90){
            rotationMatrix = glm::mat4(1.f);
            rotationMatrix = glm::translate(rotationMatrix, glm::vec3((float)-landmarkWidth, 0.f, 0.f));
            rotationMatrix = glm::scale(rotationMatrix, glm::vec3((float)inputWidth / landmarkHeight,
                                                                  (float)inputWidth / landmarkHeight,
                                                                  1.f));
            rotationMatrix = glm::rotate(rotationMatrix, (float)-M_PI/2.0f, glm::vec3(0.f, 0.f, 1.f));
        } else if(imageRotation == 270){
            rotationMatrix = glm::mat4(1.f);
            rotationMatrix = glm::translate(rotationMatrix, glm::vec3(0.f, (float)-landmarkHeight, 0.f));
            rotationMatrix = glm::scale(rotationMatrix, glm::vec3((float)inputWidth / landmarkHeight,
                                                                  (float)inputWidth / landmarkHeight,
                                                                  1.f));
            rotationMatrix = glm::rotate(rotationMatrix, (float)M_PI/2.0f, glm::vec3(0.f, 0.f, 1.f));
        } else if(imageRotation == 180){
            rotationMatrix = glm::mat4(1.f);
            rotationMatrix = glm::translate(rotationMatrix, glm::vec3((float)-landmarkWidth,
                                                                      (float)-landmarkHeight, 0.f));
            rotationMatrix = glm::scale(rotationMatrix, glm::vec3((float)inputWidth / landmarkWidth,
                                                                  (float)inputWidth / landmarkWidth,
                                                                  1.f));
            rotationMatrix = glm::rotate(rotationMatrix, (float)M_PI, glm::vec3(0.f, 0.f, 1.f));
        } else {
            rotationMatrix = glm::mat4(1.f);
            rotationMatrix = glm::scale(rotationMatrix, glm::vec3((float)inputWidth / landmarkWidth,
                                                                  (float)inputWidth / landmarkWidth,
                                                                  1.f));
        }
        
        
        // Calculate intrinsics
        float aov = 280;
        float focalLength = inputWidth * ofDegToRad(aov);
        float opticalCenterX = inputWidth/2;
        float opticalCenterY = inputHeight/2;
        
        cv::Mat1d projectionMat = cv::Mat::zeros(3,3,CV_32F);
        projectionMat(0,0) = focalLength;
        projectionMat(1,1) = focalLength;
        projectionMat(0,2) = opticalCenterX;
        projectionMat(1,2) = opticalCenterY;
        projectionMat(2,2) = 1;
        
        cv::Size2i imageSize(inputWidth, inputHeight);
        intrinsics.setup(projectionMat, imageSize);
    }
};
