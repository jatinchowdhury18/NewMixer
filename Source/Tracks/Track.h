#ifndef TRACK_H_INCLUDED
#define TRACK_H_INCLUDED

#include "../Processors/TrackProcessor.h"
#include "../GUI Extras/Colours.h"
#include "AutomationHelper.h"

namespace TrackConstants
{
    constexpr float darkAlpha = 0.4f;

    constexpr float maxDiameter = 100.0f;
    constexpr float minDiameter = 20.0f;
    constexpr float defaultDiameter = 80.0f;

    enum
    {
        width = 150,
    };
}

class Track : public Component,
              public SettableTooltipClient,
              private Timer,
              private TrackProcessor::Listener
{
public:
    enum TrackCmds
    {
        solo = 0x1001,
        mute,
        recordAutomation,
        recordInput,
    };

    Track (File& file, String name, String shortName, int x, int y, Colour colour);
    Track (MemoryInputStream* input, String name, String shortName, int x, int y, Colour colour);
    Track (int64 sampleLength, int64 startSample, String name, String shortName, int x, int y, Colour colour);
    ~Track();

    TrackProcessor* getProcessor() const { return processor; }

    void paint (Graphics& g) override;
    void resized() override;
    bool hitTest (int x, int y) override;

    bool getIsSelected() { return isSelected; }
    void setSelected (bool selected) { isSelected = selected; }

    bool isSoloed() { return processor->getSoloed() == TrackProcessor::SoloState::thisTrack; }
    void setSoloed (TrackProcessor::SoloState state) { processor->setSoloed (state); }

    bool toggleMute();
    void togglePlay();

    void timerCallback() override;
    void trackMoved();
    void newLoop() override;

    TrackColours& getColours() { return colours; }
    Colour getColour() { return trackColour; }
    void setTrackColour (Colour newColour) { trackColour = newColour; }

    AutoHelper& getAutoHelper() { return autoHelper; }

    void setDiameter (float newD) { diameter = newD; }
    float getDiameter() { return diameter; }

    int& getLastDrag() { return lastDragLocation; }
    void setDragging (bool drag) { isDragging = drag; }

    String getShortName() { return shortName; }

    bool getIsPlaying() { return isPlaying; }

private:
    void mouseDown (const MouseEvent& e) override;
    void mouseDrag (const MouseEvent& e) override;
    void mouseUp (const MouseEvent& e) override;

    String name;
    String shortName;
    Colour trackColour;
    TrackColours colours;

    float diameter = TrackConstants::defaultDiameter;
    bool isDragging = false;
    int lastDragLocation = 0;

    bool isSelected = false;
    bool isPlaying = false;

    TrackProcessor* processor;

    AutoHelper autoHelper;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Track)
};

#endif //TRACK_H_INCLUDED
