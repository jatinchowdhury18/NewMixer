#ifndef COLOURS_H_INCLUDED
#define COLOURS_H_INCLUDED

#include "JuceHeader.h"

class TrackColours
{
public:
    TrackColours()
    {
        trackColours.add (Colours::darkred);
        trackColours.add (Colours::darkgreen);
        trackColours.add (Colours::khaki);
        trackColours.add (Colours::deeppink);
        trackColours.add (Colours::cadetblue);
        trackColours.add (Colours::lawngreen);
        trackColours.add (Colours::silver);
        trackColours.add (Colours::coral);
        trackColours.add (Colours::rebeccapurple);
        trackColours.add (Colours::saddlebrown);
    }

    Colour getColour (int index)
    {
        int corrIndex = index % trackColours.size();
        return trackColours[corrIndex];
    }

private:
    Array<Colour> trackColours;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TrackColours)
};

#endif //COLOURS_H_INCLUDED
