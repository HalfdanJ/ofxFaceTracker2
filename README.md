# ofxFaceTracker2
This addon for [openFrameworks](http://openframeworks.cc) is implementing the face detection and face landmark detection algorithm from [dlib](http://dlib.net/). 
The structure of addon is heavily inspired by [ofxFaceTracker](https://github.com/kylemcdonald/ofxFaceTracker), but does have some significant changes, so its not directly compatible with it. 

## Features
- **Face detection** - supports multiple faces. This runs by default on a background thread. 
- **Face landmark detection** - runs [very fast](http://blog.dlib.net/2014/08/real-time-face-pose-estimation.html), and can easily run on high resolution video in realtime. Returns a list of 68 points describing different parts of the face.
- **Pose estimation** - estimate the 3d position and orientation of the head. Allows you to draw easily in a 3d space relative to the heads position and orientation, and to easily estimate the direction a head is facing. 

## Platforms
Currently ofxFaceTracker2 is build and tested on **OSX** and **Android**, but DLIB is crossplatform, and should be possible to [build](http://dlib.net/compile.html) for all other major platforms. The current builds are done with [Apothecary](https://github.com/openframeworks/openFrameworks/tree/master/scripts/apothecary)

## Comparison to [ofxFaceTracker](https://github.com/kylemcdonald/ofxFaceTracker)
The algorithms doing the tracking are very different in the two addons. In the old **ofxFaceTracker** addon based on Jason Saragih's FaceTracker library, the face detection and landmark detection happended in the same algorithm. 

In **ofxFaceTracker2**, the two stages happens seperately. The face detection is considerably slower then previously, but can easily run on a background thread, on the other hand is the landmark detection (so finding the actualy details in the face), faster and more robust. Running in multithreaded mode (default mode) will give you a better performance with this addon, and better accuracy.

Another big difference is this addon supports multiple faces, whereas the old addon only supports one face at the time. 

## Installation
- The addon depends on [ofxCv addon](https://github.com/kylemcdonald/ofxCv).
- All apps needs a landsmarks training data file. It can be downloaded here [shape_predictor_68_face_landmarks.dat](http://sourceforge.net/projects/dclib/files/dlib/v18.10/shape_predictor_68_face_landmarks.dat.bz2), and should be put unzipped in `bin/data` folder of your app or examples. 

