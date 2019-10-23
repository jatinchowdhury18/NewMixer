#!/bin/sh
if [ -f $1/Mac/CHOW.component ]
    then
    echo "Test Plugins already built!"
else
    echo "Building Test plugin..."
    git submodule update --init
    cd $1/CHOW
    $2 --resave CHOW.jucer
    cd Builds/MacOSX
    xcodebuild -project CHOW.xcodeproj/ clean
    xcodebuild -project CHOW.xcodeproj/ -configuration Release > /dev/null

    echo "Copying test plugin"
    cd $1
    cp -R $1/CHOW/Builds/MacOSX/build/Release/CHOW.component $1/Mac/CHOW.component
    cp -R $1/CHOW/Builds/MacOSX/build/Release/CHOW.vst $1/Mac/CHOW.vst
    cp -R $1/CHOW/Builds/MacOSX/build/Release/CHOW.vst3 $1/Mac/CHOW.vst3
fi
