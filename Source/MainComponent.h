#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "Tracks/Track.h"
#include "Tracks/MasterTrack.h"
#include "GUI Extras/Colours.h"
#include "GUI Extras/Settings.h"

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
        buttonWidth = 80,
        buttonHeight = 25,

        tooltipTime = 300,
    };

    enum Cmds
    {
        newRecordTrack = 0x2346,
    };

    //==============================================================================
    MainComponent();
    ~MainComponent();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;

    void mouseDown (const MouseEvent& /*event*/) override;
    void clearSelectedTrack();

    bool keyPressed (const KeyPress& key) override;
    void togglePlay();

private:
    void initSettings();
    void addTracks (String stemsToUse);

    static void rightClickCallback (int result, MainComponent* mc);
    void addRecordingTrack();

    //==============================================================================
    std::unique_ptr<MasterTrack> master;
    OwnedArray<Track> tracks;

    TrackColours trackColours;
    std::unique_ptr<TooltipWindow> tooltipWindow;

    TextButton settingsButton;
    std::unique_ptr<SettingsWindow> settingsWindow;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
