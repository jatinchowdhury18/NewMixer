#ifndef PLAYHEAD_H_INCLUDED
#define PLAYHEAD_H_INCLUDED

#include "Processors/TrackBase.h"

class Playhead : public Component,
                 private Timer
{
public:
    Playhead (TrackBase* trackProc);

    void timerCallback() override;

    void paint (Graphics& g) override;

    void mouseMove (const MouseEvent& event) override;
    void mouseDrag (const MouseEvent& event) override;
    void mouseUp (const MouseEvent& event) override;

private:
    TrackBase* proc;

    int64 pos;

    bool mouseIsDragging = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Playhead)
};

#endif //PLAYHEAD_H_INCLUDED
