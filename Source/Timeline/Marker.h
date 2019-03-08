#ifndef MARKER_H_INCLUDED
#define MARKER_H_INCLUDED

/*
#include "../Processors/AudioPlayer.h"

enum MarkerType
{
    LoopStart,
    LoopEnd,
};

class Marker
{
public:
    Marker (AudioPlayer* player, MarkerType type);

    void paint (Graphics& g, Rectangle<float> bounds);

    bool mouseOver (const MouseEvent& event);
    void dragging (const MouseEvent& event);
    void endDrag (const MouseEvent& event, Rectangle<float> bounds);

    bool isOver() { return mouseIsOver; }
    bool isDragging() { return mouseIsDragging; }

private:
    AudioPlayer* player;

    int64 markPos;
    MarkerType type;

    bool mouseIsOver = false;
    bool mouseIsDragging = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Marker)
};
*/
#endif //MARKER_H_INCLUDED
