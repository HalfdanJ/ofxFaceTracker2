#! /bin/bash
#
# DLIB
#
# uses CMake

# define the version
VER=18.18

# tools for git use
GIT_URL="https://github.com/davisking/dlib"
#GIT_TAG="v$VER"
GIT_TAG=

FORMULA_TYPES=( "osx" "android")


# download the source code and unpack it into LIB_NAME
function download() {
	curl -Lk http://dlib.net/files/dlib-$VER.tar.bz2 -o dlib-$VER.tar.bz2
	tar -xjvf dlib-$VER.tar.bz2
	mv dlib-$VER dlib
	rm dlib-$VER.tar.bz2

}

# prepare the build environment, executed inside the lib src dir
function prepare() {
   	if [ "$TYPE" == "android" ] ; then
   		cp -rf $FORMULA_DIR/jni/  jni
	fi
}

# executed inside the lib src dir
function build() {
  if [ "$TYPE" == "osx" ] ; then
		cd dlib
		#if [ -d  builds ]; then
		mkdir build
		#fi
		cd build

		cmake ..
		cmake --build . --config Release
		cd ../../
	elif [ "$TYPE" == "android" ] ; then
		${NDK_ROOT}/ndk-build -j4 NDK_DEBUG=0 NDK_PROJECT_PATH=.
	fi
}
# executed inside the lib src dir, first arg $1 is the dest libs dir root
function copy() {
    # headers
	if [ -d $1/include ]; then
	    rm -rf $1/include
	fi
	mkdir -p $1/include

	if [ "$TYPE" == "osx" ] ; then
	    cp -vr dlib/ $1/include/dlib
	    rm -rf $1/include/dlib/build
	    rm -rf $1/include/dlib/test
	    rm  $1/include/dlib/all_gui.cpp

	    cp -v dlib/build/libdlib.a $1/lib/osx/libdlib.a
	elif [ "$TYPE" == "android" ] ; then
	    cp -vr dlib/ $1/include/dlib
			rm -rf $1/include/dlib/build
	    rm -rf $1/include/dlib/test
	    rm  $1/include/dlib/all_gui.cpp

		mkdir -p $1/lib/android/armeabi-v7a/
	    cp  obj/local/armeabi-v7a/libdlib.a $1/lib/android/armeabi-v7a/
		mkdir -p $1/lib/android/x86/
	    cp  obj/local/x86/libdlib.a $1/lib/android/x86/
	fi
}

# executed inside the lib src dir
function clean() {
	if [ "$TYPE" == "osx" ] ; then

   		cd dlib/build
   		cmake clean .
   		cd ..
   		rm -rf build
		cd ..

	elif [ "$TYPE" == "android" ] ; then
		rm -rf obj
	fi



}
