#pragma once

#include "ofxFaceTracker2.h"
#include "ofMain.h"
#include "ofMesh.h"

using namespace ofxCv;

class TargetMesh : public ofMesh {
    private:
        ofxCv::KalmanPosition positions[86];
        bool preloaded;
    public:
        TargetMesh();
        void update_vertices(const std::vector<glm::vec2> &points);
        void update_uvs(const std::vector<glm::vec2> &points);
        int framesUnseen;
        void render();
};
