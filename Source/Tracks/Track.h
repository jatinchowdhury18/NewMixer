#ifndef TRACK_H_INCLUDED
#define TRACK_H_INCLUDED

#include "../Processors/TrackProcessor.h"
#include "../GUI Extras/Colours.h"

class Track : public Component,
              public SettableTooltipClient
{
public:
    enum
    {
        maxWidth = 100,
        minWidth = 20,
        defaultWidth = 80,
    };

    enum TrackCmds
    {
        solo = 0x1001,
        mute,
    };

    Track (File& file, String name, int x, int y, Colour colour);

    TrackProcessor* getProcessor() const { return processor; }

    void paint (Graphics& g) override;
    void resized() override;

    bool getIsSelected() { return isSelected; }
    void setSelected (bool selected) { isSelected = selected; }

    bool doKeyPressed (const KeyPress& key);

    bool isSoloed() { return processor->getSoloed() == TrackProcessor::SoloState::thisTrack; }
    void setSoloed (TrackProcessor::SoloState state) { processor->setSoloed (state); }

    bool toggleMute();

    void rewind() { processor->rewind(); }

private:
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
    void setSizeConstrained (int oldSize, int change);

    String name;
    Colour trackColour;
    TrackColours colours;

    bool isDragging = false;
    int lastDragLocation = 0;

    bool isSelected = false;

    TrackProcessor* processor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Track)
};

#endif //TRACK_H_INCLUDED
