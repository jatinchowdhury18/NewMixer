#ifndef COLOURS_H_INCLUDED
#define COLOURS_H_INCLUDED

#include "JuceHeader.h"

class TrackColours
{
public:
    TrackColours()
    {
        trackColours.add (Colours::darkred);       colourNames.add (String ("Red"));
        trackColours.add (Colours::darkgreen);     colourNames.add (String ("Green1"));
        trackColours.add (Colours::khaki);         colourNames.add (String ("Yellow"));
        trackColours.add (Colours::deeppink);      colourNames.add (String ("Pink"));
        trackColours.add (Colours::cadetblue);     colourNames.add (String ("Blue1"));
        trackColours.add (Colours::lawngreen);     colourNames.add (String ("Green2"));
        trackColours.add (Colours::silver);        colourNames.add (String ("Silver"));
        trackColours.add (Colours::coral);         colourNames.add (String ("Blue2"));
        trackColours.add (Colours::rebeccapurple); colourNames.add (String ("Purple"));
        trackColours.add (Colours::saddlebrown);   colourNames.add (String ("Brown"));
    }

    Colour getColour (int index)
    {
        int corrIndex = index % trackColours.size();
        return trackColours[corrIndex];
    }

    int getNumColours() { return trackColours.size(); }

    String getColourName (int index) { return colourNames[index]; }

private:
    Array<Colour> trackColours;
    StringArray colourNames;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TrackColours)
};

#endif //COLOURS_H_INCLUDED
