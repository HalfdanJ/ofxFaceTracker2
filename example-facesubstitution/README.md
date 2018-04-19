# ofxFaceTracker2 - Face Substitution
This code provides an example of using the [FaceSubstitution](https://github.com/arturoc/FaceSubstitution) library with ofxFaceTracker2.

#Instructions

- Download [shape_predictor_68_face_landmarks.dat](://github.com/AKSHAYUBHAT/TensorFace/blob/master/openface/models/dlib/shape_predictor_68_face_landmarks.dat) and place in bin/data
- Modify replace the following OfxCv addon files in your IDE/editor of choice:
	- [Kalman.cpp](https://gist.github.com/ThomasColliers/375c65a3afd529bd1b9f28fe48f94cd3#file-kalman-cpp)
	- [Kalman.h](https://gist.github.com/ThomasColliers/375c65a3afd529bd1b9f28fe48f94cd3#file-kalman-h)
- See [issue #25](https://github.com/HalfdanJ/ofxFaceTracker2/issues/25) for information about this replacement.

#Thanks

To [Thomas Colliers](https://github.com/ThomasColliers) for sharing his solution in the issue above :)

