#pragma once

#include "JuceHeader.h"
#include "Track.h"
#include "MasterTrackProcessor.h"
#include "Colours.h"
#include "Settings.h"
#include "Timeline/WaveformViewer.h"
#include "AutomationPath.h"
#include "Exporting/ExportWindow.h"

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
        newSession = 0x2346,
        openSession,
        saveSession,
        saveSessionAs,
        newRecordTrack,
        newFileTrack,
        exportSession,
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
    OwnedArray<AutomationPath>& getAutoPaths() { return autoPaths; }
    MasterTrackProcessor* getMaster() { return master.get(); }
    Colour getNextColour() { return trackColours.getColour (tracks.size()); }
    WaveformViewer* getWaveform() { return waveformView.get(); }
    std::unique_ptr<ExportWindow>& getExportWindow() { return exportWindow; }

    int64 getSessionLength() const;
    File getSessionFile() const { return sessionDirectory; }
    void setSessionFile (File session) { sessionDirectory = session; }

private:
#if JUCE_DEBUG //Declarations for unit tests
    enum
    {
        numTestTracks = 15,
        numTestAutoPoints = 5000,
    };

    friend class PlayTest;
    friend class AutomationTest;
    friend class AddRemoveTracksTest;
#endif

    void initSettings();
    void initPlayButton();

    void deleteSelectedTrack() override;
    void duplicateSelectedTrack() override;
    void soloSelectedTrack() override;

    //==============================================================================
    std::unique_ptr<MasterTrackProcessor> master;
    OwnedArray<Track> tracks;
    OwnedArray<AutomationPath> autoPaths;

    std::unique_ptr<WaveformViewer> waveformView;

    std::unique_ptr<ExportWindow> exportWindow;

    TrackColours trackColours;
    std::unique_ptr<TooltipWindow> tooltipWindow;

    TextButton settingsButton;
    std::unique_ptr<SettingsWindow> settingsWindow;

    File sessionDirectory;
#if JUCE_IOS || JUCE_ANDROID
    void mouseDoubleClick (const MouseEvent& e) override;

    TextButton playButton;
#endif

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
