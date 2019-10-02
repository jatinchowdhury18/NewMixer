#ifndef CHANGETRACKVOLUME_H_INCLUDED
#define CHANGETRACKVOLUME_H_INCLUDED

#include "ActionHelper.h"
#include "TrackActionHelper.h"
#include "MainComponent.h"

class ChangeTrackVolume : public UndoableAction
{
public:
    ChangeTrackVolume (MainComponent* mc, String uuid, float change) :
        mc (mc),
        uuid (uuid),
        change (change)
    {
    }

    ~ChangeTrackVolume() {}

    bool perform() override
    {
        auto track = ActionHelper::getTrackWithUuid (mc, uuid);
        if (track == nullptr)
            return false;

        origSize = track->getDiameter();

        TrackActionHelper::setSizeConstrained (track, track->getDiameter(), change);
        return true;
    }

    bool undo() override
    {
        auto track = ActionHelper::getTrackWithUuid (mc, uuid);
        if (track == nullptr)
            return false;

        TrackActionHelper::setSizeConstrained (track, origSize, 0);
        return true;
    }

    UndoableAction* createCoalescedAction (UndoableAction* nextAction) override
    {
        auto nextChangeVolume = dynamic_cast<ChangeTrackVolume*> (nextAction);

        if (nextChangeVolume == nullptr) // next action is not a ChangeTrackVolume
            return nullptr;

        if (uuid != nextChangeVolume->uuid) //action is on wrong track
            return nullptr;

        auto newChangeVolume = new ChangeTrackVolume (mc, uuid, nextChangeVolume->change + change);
        newChangeVolume->origSize = origSize;
        return newChangeVolume;
    }

private:
    MainComponent* mc;
    const String uuid;
    float origSize;
    float change;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ChangeTrackVolume)
};

#endif //CHANGETRACKVOLUME_H_INCLUDED
