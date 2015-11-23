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

FORMULA_TYPES=( "osx" "linux" "android")


# download the source code and unpack it into LIB_NAME
function download() {
	if [ "$TYPE" == "osx" ] ; then
    	curl -Lk http://dlib.net/files/dlib-$VER.tar.bz2 -o dlib-$VER.tar.bz2
    	tar -xjvf dlib-$VER.tar.bz2
		mv dlib-$VER dlib
   		rm dlib-$VER.tar.bz2
    fi
}

# prepare the build environment, executed inside the lib src dir
function prepare() {
    : # noop
}

# executed inside the lib src dir
function build() {
    if [ "$TYPE" == "osx" ] ; then
		cd dlib
		if [ -d  builds ]; then
			mkdir build
		fi
		cd build

		cmake .. 
		cmake --build . --config Release
		cd ../../

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

	fi
    
}
