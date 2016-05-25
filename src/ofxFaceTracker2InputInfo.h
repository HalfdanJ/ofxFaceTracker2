#pragma once

#include "ofConstants.h"
#include "ofMatrix4x4.h"

struct ofxFaceTracker2InputInfo {
    ofMatrix4x4 rotationMatrix;
    int inputWidth, inputHeight;
    int landmarkWidth, landmarkHeight;
    int imageRotation;
    
    ofxFaceTracker2InputInfo(){};
    
    ofxFaceTracker2InputInfo(int inputWidth, int inputHeight,int landmarkWidth, int landmarkHeight, int imageRotation)
    : inputWidth(inputWidth)
    , inputHeight(inputHeight)
    , landmarkWidth(landmarkWidth)
    , landmarkHeight(landmarkHeight)
    , imageRotation(imageRotation) {
        if(imageRotation == 90){
            rotationMatrix.makeIdentityMatrix();
            rotationMatrix.translate(-landmarkWidth,0,0);
            rotationMatrix.scale(((float)inputWidth / landmarkHeight), ((float)inputWidth / landmarkHeight), 1);
            rotationMatrix.rotate(-90, 0,0,1);
        } else if(imageRotation == 270){
            rotationMatrix.makeIdentityMatrix();
            rotationMatrix.translate(0, -landmarkHeight,0);
            rotationMatrix.scale(((float)inputWidth / landmarkHeight), ((float)inputWidth / landmarkHeight), 1);
            rotationMatrix.rotate(90, 0,0,1);
        } else if(imageRotation == 180){
            rotationMatrix.makeIdentityMatrix();
            rotationMatrix.translate(-landmarkWidth, -landmarkHeight,0);
            rotationMatrix.scale(((float)inputWidth / landmarkWidth), ((float)inputWidth / landmarkWidth), 1);
            rotationMatrix.rotate(180, 0,0,1);
        } else {
            rotationMatrix.makeIdentityMatrix();
            rotationMatrix.scale(((float)inputWidth / landmarkWidth), ((float)inputWidth / landmarkWidth), 1);
        }
    }
};