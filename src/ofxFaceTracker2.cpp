#include <time.h>
#include "ofxFaceTracker2.h"


using namespace ofxCv;
using namespace cv;

#define it at<int>
#define db at<double>


ofxFaceTracker2::ofxFaceTracker2()
		:failed(true)
		,threaded(true)
		,imageRotation(0)
		,numFaces(0)
		,landmarkDetectorImageSize(-1)

#ifdef TARGET_ANDROID
		,faceDetectorImageSize(160*120*1.3)
#else
,faceDetectorImageSize(480*360)
#endif

{
}

void ofxFaceTracker2::setup() {
#ifndef __OPTIMIZE__
	ofLogWarning("ofxFaceTracker2")<<"Warning, the facetracker background thread runs very slowly in debug mode!";
#endif

	// Add listener for app exit to properly close thread
	ofAddListener(ofEvents().exit, this, &ofxFaceTracker2::exitEvent);

	// Get dlib's frontal face detector
	detector = dlib::get_frontal_face_detector();

	// Load landmark data file
	ofFile dataFile = ofFile("shape_predictor_68_face_landmarks.dat");
	if(dataFile.exists()){
		dlib::deserialize(dataFile.path()) >> sp;
	} else {
		throw std::runtime_error("\
                                 ofxFaceTracker2: shape_predictor_68_face_landmarks.dat data file not found in bin/data.\
                                 \n\n\
                                 Please download and extract it from http://sourceforge.net/projects/dclib/files/dlib/v18.10/shape_predictor_68_face_landmarks.dat.bz2");
	}

	thread_fps = 0;

	// Start the background thread
	if(threaded){
		startThread();
	}
}


ofxFaceTracker2::~ofxFaceTracker2(){
	if(isThreadRunning()) {
		ofLogError("ofxFaceTrackerThreaded")<<"Tracker was not stopped. You must call the trackers stop() before destroying the tracker object.";
	}
}

void ofxFaceTracker2::stop(){
	if(isThreadRunning()){
		waitForThread();
		stopThread();
	}
}

bool ofxFaceTracker2::update(Mat image, cv::Rect _roi) {
	clock_t start = clock() ;

	inputWidth = image.cols;
	inputHeight = image.rows;


	float aspect = (float)inputHeight/inputWidth;

	if(landmarkDetectorImageSize == -1 || inputWidth*inputHeight <= landmarkDetectorImageSize) {
		im = image;
	} else {
		float scale = sqrt((float) landmarkDetectorImageSize / (inputHeight*inputWidth));
		resize(image, im, cv::Size(), scale,scale, cv::INTER_NEAREST);
	}

	if(imageRotation){
		rotate_90n(im, im, imageRotation);
	}

	if(imageRotation == 90){
		landmarkRotationMatrix.makeIdentityMatrix();
		landmarkRotationMatrix.translate(-im.cols,0,0);
		landmarkRotationMatrix.scale(((float)inputWidth / im.rows), ((float)inputWidth / im.rows), 1);
		landmarkRotationMatrix.rotate(-90, 0,0,1);
	} else if(imageRotation == 270){
		landmarkRotationMatrix.makeIdentityMatrix();
		landmarkRotationMatrix.translate(0,-im.rows,0);
		landmarkRotationMatrix.scale(((float)inputWidth / im.rows), ((float)inputWidth / im.rows), 1);
		landmarkRotationMatrix.rotate(90, 0,0,1);
	} else if(imageRotation == 180){
		landmarkRotationMatrix.makeIdentityMatrix();
		landmarkRotationMatrix.translate(-im.cols,-im.rows,0);
		landmarkRotationMatrix.scale(((float)inputWidth / im.cols), ((float)inputWidth / im.cols), 1);
		landmarkRotationMatrix.rotate(180, 0,0,1);
	} else {
		landmarkRotationMatrix.makeIdentityMatrix();
		landmarkRotationMatrix.scale(((float)inputWidth / im.cols), ((float)inputWidth / im.cols), 1);
	}

	if(threaded){
		mutex.lock();
	}

	roi = _roi;
	if(roi.width == 0 && roi.height == 0){
		roi = cv::Rect(0,0,im.cols, im.rows);
	}

    if(im.type() == CV_8UC1) {
        im.copyTo(gray);
    } else {
        cvtColor(im, gray, CV_RGB2GRAY);
    }
	imageDirty = true;

	dlib::cv_image<unsigned char> dlibimg(gray);
	if(!threaded){
		dlib::cv_image<unsigned char> dlibimgRects(gray(roi));
		facesRects = detector(dlibimgRects);
	}

	if(facesRects.size() == 0){
		failed = true;
		numFaces = 0;
	} else {
		facesObjects.clear();
		failed = false;

		for (unsigned long j = 0; j < facesRects.size(); ++j)
		{
			dlib::full_object_detection shape = sp(dlibimg, facesRects[j]);
			facesObjects.push_back(shape);
		}

		for(int i=0;i<MAX_FACES;i++){
			poseCalculated[i] = false;
		}

		numFaces = facesRects.size();
	}

	if(threaded){
		mutex.unlock();
	}

	if(!intrinsicsCalculated){
		calculateIntrinsics();
	}

	return !failed;

}


void ofxFaceTracker2::threadedFunction(){
	while(isThreadRunning()) {
		if(imageDirty){
			clock_t start = clock() ;

			imageDirty = false;

			mutex.lock();
			float scale = 1;
			roiThread = roi;
			roiThread.x = (int)ofClamp(roiThread.x,0,gray.cols);
			roiThread.y = (int)ofClamp(roiThread.y,0,gray.rows);
			roiThread.width = (int)ofClamp(roiThread.width,1,gray.cols - roiThread.x);
			roiThread.height = (int)ofClamp(roiThread.height,1,gray.rows- roiThread.y);

			if(faceDetectorImageSize == -1 || roiThread.width*roiThread.height <= faceDetectorImageSize){
				gray(roiThread).copyTo(threadGray);
			} else {
				scale = sqrt((float) faceDetectorImageSize / (roiThread.width*roiThread.height));
				resize(gray(roiThread), threadGray, cv::Size(), scale, scale, cv::INTER_NEAREST);
			}


			mutex.unlock();

			dlib::cv_image<unsigned char> cvimg(threadGray);
			std::vector<dlib::rectangle> _dets = detector(cvimg);

			float s = 1.0f/scale;

			mutex.lock();
			if(s != 1 || roiThread.x != 0 || roiThread.y != 0){
				facesRects.clear();
				for(int i=0;i<_dets.size();i++){
					facesRects.push_back(dlib::rectangle(roiThread.x+_dets[i].left()*s,
														 roiThread.y+_dets[i].top()*s,
														 roiThread.x+_dets[i].right()*s,
														 roiThread.y+_dets[i].bottom()*s));
				}
			} else {
				facesRects = _dets;
			}
			mutex.unlock();

			clock_t end = clock() ;
			double elapsed_time = (end-start)/(double)CLOCKS_PER_SEC ;
			thread_fps = 1.0/elapsed_time;
		}
		//yield();
		sleep(5);
	}
}

void ofxFaceTracker2::draw(int x, int y, int _w, int _h) const{
	if(_w == -1) _w = inputWidth;
	if(_h == -1) _h = inputHeight;

	if(failed) {
		return;
	}

	ofPushMatrix();
	ofTranslate(x,y);

	if(ofGetRectMode() == OF_RECTMODE_CENTER){
		ofTranslate(-_w/2, -_h/2);
	}

	if(_w != inputWidth || _h != inputHeight){
		ofScale((float)_w/inputWidth, (float)_h/inputHeight);
	}

	ofPushStyle();
	for (int j = 0; j < size(); ++j){
		ofNoFill();

		getImageFeature(LEFT_EYE).draw();
		getImageFeature(RIGHT_EYE).draw();
		getImageFeature(LEFT_EYEBROW).draw();
		getImageFeature(RIGHT_EYEBROW).draw();
		getImageFeature(NOSE_BRIDGE).draw();
		getImageFeature(NOSE_BASE).draw();
		getImageFeature(INNER_MOUTH).draw();
		getImageFeature(OUTER_MOUTH).draw();
		getImageFeature(JAW).draw();

		auto p = getFaceBoundingBoxes()[j].getTopLeft();
		ofDrawBitmapString("id "+ofToString(j), p.x, p.y);
	}



	ofPopStyle();
	ofPopMatrix();

}

void ofxFaceTracker2::drawPose(int face) {
	if(size() <= face) return;

	ofPushView();
	ofPushStyle();
	loadPoseMatrix(face);

	ofSetColor(255,0,0);
	ofDrawLine(0,0,0, 200,0,0);
	ofSetColor(0,255,0);
	ofDrawLine(0,0,0, 0,100,0);
	ofSetColor(0,0,255);
	ofDrawLine(0,0,0, 0,0,-100);

	ofPopStyle();
	ofPopView();
}

void ofxFaceTracker2::setFaceOrientation(ofOrientation orientation){
	this->imageRotation = ofOrientationToDegrees(orientation);
}

void ofxFaceTracker2::setFaceRotation(float rotation){
	this->imageRotation = rotation;
}

void ofxFaceTracker2::setFaceDetectorImageSize(int numPixels) {
	this->faceDetectorImageSize = numPixels;
}

void ofxFaceTracker2::setLandmarkDetectorImageSize(int numPixels){
	this->landmarkDetectorImageSize = numPixels;
}

void ofxFaceTracker2::setThreaded(bool threaded){
	this->threaded = threaded;
}

int ofxFaceTracker2::getThreadFps()const{
	return thread_fps;
}

int ofxFaceTracker2::size() const {
	return numFaces;
}

vector<ofRectangle> ofxFaceTracker2::getFaceBoundingBoxes() const {
	vector<ofRectangle> ret;
	for(int i=0; i<size(); i++){
		ofRectangle rect = ofRectangle();
		if(facesRects.size() > i){
			rect = ofRectangle(facesRects[i].left(),
							   facesRects[i].top(),
							   facesRects[i].width(),
							   facesRects[i].height());
		}
		ret.push_back(rect);
	}
	return ret;
}

ofVec2f ofxFaceTracker2::getImagePoint(int i, int face) const {
	if(size() <= face) {
		return ofVec2f();
	}

	ofVec3f p = ofVec3f(facesObjects[face].part(i).x(),
						facesObjects[face].part(i).y(),0);
	p = p * landmarkRotationMatrix;

	return ofVec2f(p);
}

vector<ofVec2f> ofxFaceTracker2::getImagePoints(int face) const {
	int n = facesObjects[face].num_parts();
	vector<ofVec2f> imagePoints(n);
	for(int i = 0; i < n; i++) {
		imagePoints[i] = getImagePoint(i);
	}
	return imagePoints;
}

vector<cv::Point2f> ofxFaceTracker2::getCvImagePoints(int face) const {
	int n = facesObjects[face].num_parts();
	vector<cv::Point2f> imagePoints(n);
	for(int i = 0; i < n; i++) {
		imagePoints[i] = ofxCv::toCv(getImagePoint(i, face));
	}
	return imagePoints;
}


ofPolyline ofxFaceTracker2::getImageFeature(Feature feature, int face) const {
	return getFeature(feature, getImagePoints(face));
}

vector<int> ofxFaceTracker2::consecutive(int start, int end) {
	int n = end - start;
	vector<int> result(n);
	for(int i = 0; i < n; i++) {
		result[i] = start + i;
	}
	return result;
}


ofMesh ofxFaceTracker2::getImageMesh() const{
	return getMesh(getCvImagePoints());
}

template <class T>
ofMesh ofxFaceTracker2::getMesh(vector<T> points) const {
	cv::Rect rect(0, 0, inputWidth, inputHeight);
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
			mesh.addVertex(ofxCv::toOf(pt1));
			mesh.addVertex(ofxCv::toOf(pt2));
			mesh.addVertex(ofxCv::toOf(pt3));
		}
	}
	return mesh;

}

vector<int> ofxFaceTracker2::getFeatureIndices(Feature feature) {
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

ofVec2f ofxFaceTracker2::transformPosePosition(ofVec3f p, int face){
	if(size( )<= face) return ofVec3f();

	if(!poseCalculated[face]){
		calculatePoseMatrix(face);
	}
	std::vector<Point3f> axes;
	axes.push_back(ofxCv::toCv(p));

	std::vector<Point2f> projected_axes;
	cv::projectPoints(axes, poservec[face], posetvec[face], poseProjection[face], cv::noArray(), projected_axes);

	return ofxCv::toOf(projected_axes[0]);
}

ofMatrix4x4 ofxFaceTracker2::getPoseMatrix(int face){
	if(size() <= face) return ofMatrix4x4::newIdentityMatrix();

	if(!poseCalculated[face]){
		calculatePoseMatrix(face);
	}


	ofMatrix4x4 matrix = ofxCv::makeMatrix(poservec[face], posetvec[face]);
	matrix.scale(-1, 1, 1);
	return matrix;
}

void ofxFaceTracker2::loadPoseMatrix(int face){
	if(size() <= face) return;

	if(!poseCalculated[face]){
		calculatePoseMatrix(face);
	}

	ofMatrix4x4 matrix = getPoseMatrix(face);

	loadPoseProjectionMatrix();
	ofLoadMatrix(matrix);
}

void ofxFaceTracker2::loadPoseProjectionMatrix(){
	if(!intrinsicsCalculated){
		return;
	}
	intrinsics.loadProjectionMatrix(1, 200000);
}

// Estimates the heads 3d position and orientation
void ofxFaceTracker2::calculatePoseMatrix(int face){
	if(size( )<= face) return;

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

	float aov = 50;
	float focalLength = inputWidth * ofDegToRad(aov);
	float opticalCenterX = inputWidth/2;
	float opticalCenterY = inputHeight/2;

	cv::Mat1d projectionMat = cv::Mat::zeros(3,3,CV_32F);
	poseProjection[face] = projectionMat;
	poseProjection[face](0,0) = focalLength;
	poseProjection[face](1,1) = focalLength;
	poseProjection[face](0,2) = opticalCenterX;
	poseProjection[face](1,2) = opticalCenterY;
	poseProjection[face](2,2) = 1;

	std::vector<Point3f> head_points;

	head_points.push_back(P3D_SELLION);
	head_points.push_back(P3D_RIGHT_EYE);
	head_points.push_back(P3D_LEFT_EYE);
	head_points.push_back(P3D_RIGHT_EAR);
	head_points.push_back(P3D_LEFT_EAR);
	head_points.push_back(P3D_MENTON);
	head_points.push_back(P3D_NOSE);
	head_points.push_back(P3D_STOMMION);

	std::vector<Point2f> detected_points;
	detected_points.push_back(ofxCv::toCv(getImagePoint(SELLION,face)));
	detected_points.push_back(ofxCv::toCv(getImagePoint(RIGHT_EYE,face)));
	detected_points.push_back(ofxCv::toCv(getImagePoint(LEFT_EYE,face)));
	detected_points.push_back(ofxCv::toCv(getImagePoint(RIGHT_SIDE,face)));
	detected_points.push_back(ofxCv::toCv(getImagePoint(LEFT_SIDE,face)));
	detected_points.push_back(ofxCv::toCv(getImagePoint(MENTON,face)));
	detected_points.push_back(ofxCv::toCv(getImagePoint(NOSE,face)));

	auto stomion = (ofxCv::toCv(getImagePoint(MOUTH_CENTER_TOP))
					+ ofxCv::toCv(getImagePoint(MOUTH_CENTER_BOTTOM))) * 0.5;
	detected_points.push_back(stomion);

	// Find the 3D pose of our head
	solvePnP(head_points, detected_points,
			 poseProjection[face], noArray(),
			 poservec[face], posetvec[face], false,
#ifdef OPENCV3
			cv::SOLVEPNP_ITERATIVE);
#else
			 CV_ITERATIVE);
#endif

	// Black magic: The x axis in the rotation vector needs to get flipped.
	double * r = poservec[face].ptr<double>(0) ;
	r[0] *= -1;
	r[1] *= -1;

	poseCalculated[face] = true;
}

void ofxFaceTracker2::calculateIntrinsics(){
	float aov = 50;
	float focalLength = inputWidth * ofDegToRad(aov);
	float opticalCenterX = inputWidth/2;
	float opticalCenterY = inputHeight/2;

	cv::Mat1d projectionMat = cv::Mat::zeros(3,3,CV_32F);
	projectionMat(0,0) = focalLength;
	projectionMat(1,1) = focalLength;
	projectionMat(0,2) = opticalCenterX;
	projectionMat(1,2) = opticalCenterY;
	projectionMat(2,2) = 1;

	Size2i imageSize(inputWidth, inputHeight);
	intrinsics.setup(projectionMat, imageSize);

	intrinsicsCalculated = true;
}

void ofxFaceTracker2::exitEvent(ofEventArgs& e){
	ofRemoveListener(ofEvents().exit, this, &ofxFaceTracker2::exitEvent);
	stop();
}

void ofxFaceTracker2::rotate_90n(cv::Mat &src, cv::Mat &dst, int angle)
{
	if(angle == 270 || angle == -90){
		// Rotate clockwise 270 degrees
		cv::flip(src.t(), dst, 0);
	}else if(angle == 180 || angle == -180){
		// Rotate clockwise 180 degrees
		cv::flip(src, dst, -1);
	}else if(angle == 90 || angle == -270){
		// Rotate clockwise 90 degrees
		cv::flip(src.t(), dst, 1);
	}else if(angle == 360 || angle == 0){
		if(src.data != dst.data){
			src.copyTo(dst);
		}
	}
}
