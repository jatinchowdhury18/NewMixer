#!/bin/sh

# Detect os type
unameOut="$(uname -s)"
case "${unameOut}" in
    Linux*)     machine=Linux;;
    Darwin*)    machine=Mac;;
    CYGWIN*)    machine=Cygwin;;
    MINGW*)     machine=MinGw;;
    *)          machine="UNKNOWN:${unameOut}"
esac

# Set correct build path
if [ "${machine}" == "MinGw" ]; then
    if [ "$1" == "--release" ]; then
        run=Builds/VisualStudio2017/x64/Release/App/NewMixer
    else
        run=Builds/VisualStudio2017/x64/Debug/App/NewMixer
    fi
elif [ "${machine}" == "Mac" ]; then
    if [ "$1" == "--release" ]; then
        run=Builds/MacOSX/build/Release/NewMixer.app/Contents/MacOS/NewMixer
    else
        run=Builds/MacOSX/build/Debug/NewMixer.app/Contents/MacOS/NewMixer
    fi
fi

# Get correct argument for mode
if [ "$1" != "--release" ]; then
    mode=$1
else
    mode=$2
fi

# select mode, and print
if [ "${mode}" != "" ]; then
    if [ "${mode}" == "--unit-tests" ]; then
        echo "Running NewMixer Unit Tests..."
    elif [ "${mode}" == "Test" ]; then
        echo "Running NewMixer Test Mode..."
    elif [ "${mode}" == "Bridge" ]; then
        echo "Running NewMixer Bridge Mode..."
    elif [ "${mode}" == "Chorus" ]; then
        echo "Running NewMixer JDWAS Mode..."
    fi
    ${run} $1
else
    ${run}
fi
