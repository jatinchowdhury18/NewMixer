#ifndef CHANGETRACKVOLUME_H_INCLUDED
#define CHANGETRACKVOLUME_H_INCLUDED

#include "TrackActionHelper.h"

class ChangeTrackVolume : public UndoableAction
{
public:
    ChangeTrackVolume (Track* track, float change) :
        track (track),
        change (change)
    {}

    ~ChangeTrackVolume() {}

    bool perform() override
    {
        TrackActionHelper::setSizeConstrained (track, track->getDiameter(), change);
        return true;
    }

    bool undo() override
    {
        TrackActionHelper::setSizeConstrained (track, track->getDiameter(), -change);
        return true;
    }

private:
    Track* track;
    float change;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ChangeTrackVolume)
};

#endif //CHANGETRACKVOLUME_H_INCLUDED
