#pragma once

#include "JuceHeader.h"
#include "Track.h"
#include "MasterTrack.h"
#include "Colours.h"
#include "Settings.h"
#include "Timeline/WaveformViewer.h"

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/

namespace MainConstants
{
    constexpr float heightFactor = 0.8f;
    constexpr float buttonWidthFactor = 0.08f;
    constexpr float buttonHeightFactor = 0.05f;
}

class MainComponent : public Component,
                      public Track::Listener
{
public:
    enum
    {
        width = 900,
        height = 600,

        tooltipTime = 300,
    };

    enum Cmds
    {
        newRecordTrack = 0x2346,
        newFileTrack,
    };

    //==============================================================================
    MainComponent (String mode = {});
    ~MainComponent();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;

    void mouseDown (const MouseEvent& /*event*/) override;
    bool keyPressed (const KeyPress& key) override;

    OwnedArray<Track>& getTracks() { return tracks; }
    MasterTrack* getMaster() { return master.get(); }
    Colour getNextColour() { return trackColours.getColour (tracks.size()); }
    WaveformViewer* getWaveform() { return waveformView.get(); }

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

    void deleteSelectedTrack() override;
    void duplicateSelectedTrack() override;
    void soloSelectedTrack() override;

    //==============================================================================
    std::unique_ptr<MasterTrack> master;
    OwnedArray<Track> tracks;

    std::unique_ptr<WaveformViewer> waveformView;

    TrackColours trackColours;
    std::unique_ptr<TooltipWindow> tooltipWindow;

    TextButton settingsButton;
    std::unique_ptr<SettingsWindow> settingsWindow;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
