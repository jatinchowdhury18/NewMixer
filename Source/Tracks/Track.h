#ifndef TRACK_H_INCLUDED
#define TRACK_H_INCLUDED

#include "JuceHeader.h"
#include "../Processors/TrackProcessor.h"

class Track : public Component,
              public SettableTooltipClient
{
public:
    enum
    {
        maxWidth = 50,
        minWidth = 5,
    };

    Track (File& file, String name, int x, int y);

    TrackProcessor* getProcessor() const { return processor; }

    void paint (Graphics& g) override;
    void resized() override { processor->trackMoved (getX(),  getY(), getWidth()); }

    void setSelected (bool selected) { isSelected = selected; }

private:
    void mouseDown (const MouseEvent& e) override;
    void mouseDrag (const MouseEvent& e) override;
    void mouseUp (const MouseEvent& e) override;

    void changeSize (const MouseEvent& e);
    void changePosition (const MouseEvent& e);

    String name;

    bool isDragging = false;
    int lastDragLocation = 0;

    bool isSelected = false;

    ScopedPointer<TrackProcessor> processor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Track)
};

#endif //TRACK_H_INCLUDED
