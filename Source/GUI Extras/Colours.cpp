#include "Colours.h"

JUCE_IMPLEMENT_SINGLETON (TrackColours)

TrackColours::TrackColours()
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

Colour TrackColours::getColour (int index)
{
    int corrIndex = index % trackColours.size();
    return trackColours[corrIndex];
}
