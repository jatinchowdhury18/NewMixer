#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "Tracks/Track.h"
#include "Tracks/MasterTrack.h"

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent : public Component,
                      public DragAndDropTarget,
                      public DragAndDropContainer
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;

    bool isInterestedInDragSource (const SourceDetails& dragSourceDetails) override { return true; }
    void itemDropped (const SourceDetails& dragSourceDetails) override;

private:
    //==============================================================================
    ScopedPointer<MasterTrack> master;
    ScopedPointer<Track> track;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
