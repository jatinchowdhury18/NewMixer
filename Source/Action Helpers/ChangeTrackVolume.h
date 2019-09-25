#ifndef CHANGETRACKVOLUME_H_INCLUDED
#define CHANGETRACKVOLUME_H_INCLUDED

#include "TrackActionHelper.h"

class ChangeTrackVolume : public UndoableAction
{
public:
    ChangeTrackVolume (Track* track, float change) :
        track (track),
        origSize (track->getDiameter()),
        change (change)
    {
    }

    ~ChangeTrackVolume() {}

    bool perform() override
    {
        TrackActionHelper::setSizeConstrained (track, track->getDiameter(), change);
        return true;
    }

    bool undo() override
    {
        TrackActionHelper::setSizeConstrained (track, origSize, 0);
        return true;
    }

    UndoableAction* createCoalescedAction (UndoableAction* nextAction) override
    {
        auto nextChangeVolume = dynamic_cast<ChangeTrackVolume*> (nextAction);

        if (nextChangeVolume == nullptr) // next action is not a ChangeTrackVolume
            return nullptr;

        auto newChangeVolume = new ChangeTrackVolume (track, nextChangeVolume->change + change);
        newChangeVolume->origSize = origSize;
        return newChangeVolume;
    }

private:
    Track* track;
    float origSize;
    float change;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ChangeTrackVolume)
};

#endif //CHANGETRACKVOLUME_H_INCLUDED
