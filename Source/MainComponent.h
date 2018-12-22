#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "Tracks/Track.h"
#include "Tracks/MasterTrack.h"

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent : public Component
{
public:
    enum
    {
        width = 900,
        height = 600,
    };

    //==============================================================================
    MainComponent();
    ~MainComponent();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;

private:
    //==============================================================================
    ScopedPointer<MasterTrack> master;
    OwnedArray<Track> tracks;

    ScopedPointer<TooltipWindow> tooltipWindow;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
