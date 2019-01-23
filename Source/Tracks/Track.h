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
    };

    Track (File& file, String name, String shortName, int x, int y, Colour colour);
    Track (MemoryInputStream* input, String name, String shortName, int x, int y, Colour colour);
    ~Track();

    TrackProcessor* getProcessor() const { return processor; }

    void paint (Graphics& g) override;
    void resized() override;

    bool getIsSelected() { return isSelected; }
    void setSelected (bool selected) { isSelected = selected; }

    bool doKeyPressed (const KeyPress& key);

    bool isSoloed() { return processor->getSoloed() == TrackProcessor::SoloState::thisTrack; }
    void setSoloed (TrackProcessor::SoloState state) { processor->setSoloed (state); }

    bool toggleMute();
    void togglePlay();

    void timerCallback() override;
    void trackMoved();
    void newLoop() override;

private:
    void paintCircle (Graphics& g, float pos, bool darken);
    void paintName (Graphics& g, float pos, bool darken);
    void paintMeter (Graphics& g, bool darken);
    void paintRing (Graphics& g, float pos, Colour colour);
    void paintMute (Graphics& g, float pos, bool darken);

    bool hitTest (int x, int y) override;
    void mouseDown (const MouseEvent& e) override;
    void mouseDrag (const MouseEvent& e) override;
    void mouseUp (const MouseEvent& e) override;

    static void rightClickCallback (int result, Track* track);

    void changeSize (const MouseEvent& e);
    void changeSize();
    void changePosition (const MouseEvent& e);
    void changePosition();
    void changeColour (int index);

    void setPositionConstrained (Point<int> pos);
    void setSizeConstrained (float oldSize, float change);

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
