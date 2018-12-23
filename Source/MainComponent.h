#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "Tracks/Track.h"
#include "Tracks/MasterTrack.h"
#include "GUI Extras/Colours.h"

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

        tooltipTime = 500,
    };

    //==============================================================================
    MainComponent();
    ~MainComponent();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;

    void mouseDown (const MouseEvent& /*event*/) override;
    void clearSelectedTrack();

private:
    //==============================================================================
    ScopedPointer<MasterTrack> master;
    OwnedArray<Track> tracks;

    TrackColours trackColours;
    ScopedPointer<TooltipWindow> tooltipWindow;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
