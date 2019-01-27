#ifndef TRACKMETER_H_INCLUDED
#define TRACKMETER_H_INCLUDED

#include "JuceHeader.h"

class Track;
class TrackMeter : public Component,
                   private Timer
{
public:
    TrackMeter (Track* track);

    void paint (Graphics& g) override;

private:
    void timerCallback() override { repaint(); }

    Track* track;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TrackMeter)
};

#endif //TRACKMETER_H_INCLUDED
