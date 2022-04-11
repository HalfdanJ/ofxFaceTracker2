# ofxFaceTracker2 - Face Substitution
This code provides an example of using the [FaceSubstitution](https://github.com/arturoc/FaceSubstitution) library with ofxFaceTracker2.
It is based on [samhains](https://github.com/samhains)'s [implementation](https://github.com/HalfdanJ/ofxFaceTracker2/pull/42) and modified with a new kalman filter and `glm::vec` commands to work with oF 0.10 and up.

Tested only on oF 0.11.1 on MacOS

# Pre-Instructions
As with all openFrameworks projects, it is best to use the projectGenerator, import this project's root folder, check that all addons from addons.make are displayed and installed and then generate the project.

# Instructions

- Download [shape_predictor_68_face_landmarks.dat](://github.com/AKSHAYUBHAT/TensorFace/blob/master/openface/models/dlib/shape_predictor_68_face_landmarks.dat) and place in bin/data
- Use the files in the `lib` folder to replace the Kalman.h and Kalman.cpp in your [ofxCV](https://github.com/kylemcdonald/ofxCv) in your addons folder in `ofxCv/libs/ofxCv/include/ofxCv/Kalman.h` and `ofxCv/libs/ofxCv/src/Kalman.cpp`. This project was running on ofxCv-master commit` 209a3986a8f8e1928c7a11f6e1c5d8ea5406e639`.



# Thanks

To [Thomas Colliers](https://github.com/ThomasColliers) for sharing his solution in the issue above :)
To [nondescriptpointer](https://github.com/nondescriptpointer/) for reuploading the updated Kalman filter and TargetMesh code from this thread [issue #25](https://github.com/HalfdanJ/ofxFaceTracker2/issues/25)
