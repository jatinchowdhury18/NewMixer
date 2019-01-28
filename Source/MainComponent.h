#pragma once

#include "JuceHeader.h"
#include "Track.h"
#include "MasterTrack.h"
#include "Colours.h"
#include "Settings.h"

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent : public Component,
                      public Track::Listener
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
    MainComponent (String mode = "Test");
    ~MainComponent();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;

    void mouseDown (const MouseEvent& /*event*/) override;
    void clearSelectedTrack();

    bool keyPressed (const KeyPress& key) override;
    void togglePlay();
    void changeSelect (bool forward);

private:
#if JUCE_DEBUG
    friend class PlayTest;
    friend class AutomationTest;
    friend class AddRemoveTracksTest;
#endif

    void initSettings();
    void addTracks (String stemsToUse);
    void bridgeTracks();
    void chorusTracks();
    void testTracks();
    void setupTrack (const void* sourceData, size_t sourceSize, String name, String shortName);

    static void rightClickCallback (int result, MainComponent* mc, Point<int> p);
    void addRecordingTrack (int x, int y);
    void deleteSelectedTrack() override;

    //==============================================================================
    std::unique_ptr<MasterTrack> master;
    OwnedArray<Track> tracks;

    TrackColours trackColours;
    std::unique_ptr<TooltipWindow> tooltipWindow;

    TextButton settingsButton;
    std::unique_ptr<SettingsWindow> settingsWindow;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
