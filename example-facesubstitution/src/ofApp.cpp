#include "ofApp.h"

using namespace ofxCv;

void ofApp::setup() {
    
	ofSetVerticalSync(true);
	cam.setup(1280, 720);
	clone.setup(1280, 720);

	camTracker.setup("model/shape_predictor_68_face_landmarks.dat");
	srcTracker.setup("model/shape_predictor_68_face_landmarks.dat");
	ofFbo::Settings settings;
	settings.width = cam.getWidth();
	settings.height = cam.getHeight();
	maskFbo.allocate(settings);
	srcFbo.allocate(settings);

	faces.allowExt("jpg");
	faces.allowExt("png");
	faces.listDir("faces");
	currentFace = 0;
	if(faces.size()!=0){
		loadFace(faces.getPath(currentFace));
	}
}

void ofApp::update() {
	cam.update();
	if(cam.isFrameNew()) {
		camMesh.clearTexCoords();
		camTracker.update(toCv(cam));
		
		vector<ofxFaceTracker2Instance> instances = camTracker.getInstances();
		if (instances.size() > 0) {
			ofxFaceTracker2Instance camTarget = instances[0];
			std::vector<glm::vec2> targetPoints = camTarget.getLandmarks().getImagePoints();
			targetMesh.update_vertices(targetPoints);
			targetMesh.update_uvs(srcPoints);

			maskFbo.begin();
			ofClear(0, 255);
			targetMesh.draw();
			maskFbo.end();
			srcFbo.begin();
			ofClear(0, 255);
			src.bind();
			targetMesh.draw();
			src.unbind();
			srcFbo.end();
		}

		
		
		clone.setStrength(16);
		clone.update(srcFbo.getTextureReference(), cam.getTextureReference(), maskFbo.getTextureReference());

	}
}

void ofApp::draw() {
	ofSetColor(255);
	
		//cam.draw(0, 0);
	if(src.getWidth() > 0) {
        clone.draw(0, 0);

	} else {
		cam.draw(0, 0);
	}

}

void ofApp::loadFace(string face){
	ofLog(OF_LOG_NOTICE, "loading"+ofToString(face));
	src.loadImage(face);
	if(src.getWidth() > 0) {
		while (srcPoints.size() == 0) {
			srcTracker.update(toCv(src));
			vector<ofxFaceTracker2Instance>  instances = srcTracker.getInstances();
			ofLog(OF_LOG_NOTICE, "size f instance"+ofToString(instances.size()));
			if (instances.size() > 0) {
				ofxFaceTracker2Instance instance = instances[0];
				srcPoints = instance.getLandmarks().getImagePoints();
				targetMesh.update_uvs(srcPoints);
			}
			ofLog(OF_LOG_NOTICE, "vector size"+ofToString(srcPoints.size()));
		}
	}
}
