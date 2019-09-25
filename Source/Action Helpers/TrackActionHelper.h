#ifndef TRACKACTIONHELPER_H_INCLUDED
#define TRACKACTIONHELPER_H_INCLUDED

#include "Track.h"

class TrackActionHelper
{
public:
    using TrackCmds = Track::TrackCmds;

    static void rightClickMenu (Track* track);
    static void rightClickCallback (int result, Track* track);
    static bool doKeyPressed (Track* track, const KeyPress& key);

    static void toggleMute (Track* track);
    static void changeSize (Track* track, const MouseEvent& e);
    static void changeSize (Track* track);
    static void changePosition (Track* track, const MouseEvent& e);
    static void changePosition (Track* track);
    static void changeColour (Track* track, int index);

    static void setRelPosition (Track* track, Point<int> pos);
    static void setPositionConstrained (Track* track, Point<int> pos);
    static void setSizeConstrained (Track* track, float oldSize, float change);

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TrackActionHelper)
};

#endif //TRACKACTIONHELPER_H_INCLUDED
