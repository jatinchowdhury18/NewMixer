# NewMixer 
[![Build Status](https://travis-ci.com/jatinchowdhury18/NewMixer.svg?branch=master)](https://travis-ci.com/jatinchowdhury18/NewMixer)
[![Download Latest](https://img.shields.io/badge/download-latest-blue.svg)](https://github.com/jatinchowdhury18/NewMixer/releases/latest)
[![Github All Releases](https://img.shields.io/github/downloads/jatinchowdhury18/newmixer/total.svg)](https://github.com/jatinchowdhury18/NewMixer/releases/latest)

An audio mixing tool that allows the user to visualize audio sources by their location in space rather than as channels on a mixing board.

This is part of a class project for [Music 256a](https://ccrma.stanford.edu/courses/256a/) at Standford University (CCRMA).

## Instructions for building
  - Install git if necessary
  - Run the following git commands:
    ```bash
    # Clone the repository
    git clone --recursive https://github.com/jatinchowdhury18/NewMixer.git

    # Enter repository
    cd NewMixer

    # Initialize JUCE submodule
    git submodule update --init --recursive
    ```
  - Navigate to "JUCE/extras/Projucer/Builds/"
  - Build the Projucer using XCode (Mac), Visual Studio (Windows), or Makefile (Linux)
- From the Projucer, open NewMixer.jucer
- From the "File" menu, open "Global Paths..."
  - Set "Path To JUCE" to "...\NewMixer\JUCE"
  - Set "JUCE Modules" to "...\NewMixer\JUCE\modules"
  - Close "Global Paths" window
- Select "Save and open in IDE"
- Build NewMixer

## Instructions for using
  - Use the space bar to play/pause (All tracks are muted initially)
  - Each circle represents a track, hover your mouse to see the track name, click to select
    - Press "m" to mute/unmute; "s" to solo/unsolo, or use right-click menu
    - To change the colour or the track name, use right-click menu
    - To record automation, select "Automate" from the right-click menu
  - To create a new input track, right-click and select "New Track"
    - To record, right-click and select record
  - Move a track around the virtual space by dragging or using the arrow keys
  - Change the track volume by pressing "Alt" and dragging

## Rules for collaboration
  - Clone repository
  - Create local develop branch
  - After pushing changes to your develop branch, make a pull request against the remote develop branch
  - Once your changes have been reviewed, and all checks are passing, your code can be merged into master