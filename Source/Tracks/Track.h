#ifndef TRACK_H_INCLUDED
#define TRACK_H_INCLUDED

#include "JuceHeader.h"

class Track : public Component,
              public SettableTooltipClient
{
public:
    Track();

private:
    void paint (Graphics& g) override;
    //void resized() override;

    void mouseDrag (const MouseEvent& e) override;
    void mouseUp (const MouseEvent& e) override;

    String name = String ("Name");

    bool isDragging = false;
    int lastDragLocation = 0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Track)
};

#endif //TRACK_H_INCLUDED
