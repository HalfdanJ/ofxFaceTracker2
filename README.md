# ofxFaceTracker2
This addon for [openFrameworks](http://openframeworks.cc) is implementing the face detection and face landmark detection algorithm from [dlib](http://dlib.net/). 
The structure of this addon is heavily inspired by [ofxFaceTracker](https://github.com/kylemcdonald/ofxFaceTracker). But, it does have some significant changes so it's not directly compatible with it. 

## Features
- **Face detection** - supports multiple faces. This runs on a background thread, by default. 
- **Face landmark detection** - runs [very fast](http://blog.dlib.net/2014/08/real-time-face-pose-estimation.html), and can easily run on high resolution video in realtime. Returns a list of 68 points describing different parts of the face.
- **Pose estimation** - estimates the 3d position and orientation of the head. Allows you to easily draw in a 3d space relative to a head's position and orientation, and estimate the direction a head is facing. 

## Platforms
Currently ofxFaceTracker2 is built and tested on **OSX** and **Android**, but dlib is crossplatform, so it should be possible to [build](http://dlib.net/compile.html) for all other major platforms. The current builds are done with [Apothecary](https://github.com/openframeworks/openFrameworks/tree/master/scripts/apothecary)

## Comparison to [ofxFaceTracker](https://github.com/kylemcdonald/ofxFaceTracker)
The algorithms doing the tracking are very different. In the old **ofxFaceTracker** addon based on Jason Saragih's FaceTracker library, the face detection and landmark detection happens in the same algorithm. 

In **ofxFaceTracker2**, the two stages happen seperately. The face detection in ofxFT2 is considerably slower then ofxFT, but it can easily run on a background thread. The landmark detection (finding the actual details on the face) is faster and more robust. Running ofxFT2 in multithreaded mode (default mode) will give you a better performance and better accuracy in comparison to ofxFT.

Another big difference is this addon supports multiple faces, whereas the old addon only supports one face at the time. 

## Installation
- The addon depends on [ofxCv addon](https://github.com/kylemcdonald/ofxCv).
- All apps need a landsmarks training data file. It can either be downloaded using the command `sh download-model.sh`, or manually be downloaded here [shape_predictor_68_face_landmarks.dat](http://sourceforge.net/projects/dclib/files/dlib/v18.10/shape_predictor_68_face_landmarks.dat.bz2), and should be put unzipped in `model` for the examples to work, or in `bin/data` folder of your app. 

