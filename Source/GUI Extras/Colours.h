#ifndef COLOURS_H_INCLUDED
#define COLOURS_H_INCLUDED

#include "JuceHeader.h"

class TrackColours
{
public:
    TrackColours();
    ~TrackColours() { clearSingletonInstance(); }

    Colour getColour (int index);

    int getNumColours() { return trackColours.size(); }

    String getColourName (int index) { return colourNames[index]; }

    JUCE_DECLARE_SINGLETON (TrackColours, false)

private:
    Array<Colour> trackColours;
    StringArray colourNames;
};

#endif //COLOURS_H_INCLUDED
