#ifndef MOVETRACK_H_INCLUDED
#define MOVETRACK_H_INCLUDED

#include "ActionHelper.h"
#include "TrackActionHelper.h"

class MoveTrack : public UndoableAction
{
public:
    MoveTrack (MainComponent* mc, String uuid, Point<int> point) :
        mc (mc),
        uuid (uuid),
        point (point)
    {}

    ~MoveTrack() {}

    bool perform() override
    {
        auto track = ActionHelper::getTrackWithUuid (mc, uuid);
        if (track == nullptr)
            return false;

        origPoint = track->getBoundsInParent().getTopLeft();

        TrackActionHelper::setRelPosition (track, point);
        TrackActionHelper::setPositionConstrained (track, point);
        track->trackMoved();
        return true;
    }

    bool undo() override
    {
        auto track = ActionHelper::getTrackWithUuid (mc, uuid);
        if (track == nullptr)
            return false;

        TrackActionHelper::setRelPosition (track, origPoint);
        TrackActionHelper::setPositionConstrained (track, origPoint);
        track->trackMoved();
        return true;
    }

    UndoableAction* createCoalescedAction (UndoableAction* nextAction) override
    {
        auto nextMoveTrack = dynamic_cast<MoveTrack*> (nextAction);

        if (nextMoveTrack == nullptr) // next action is not a MoveTrack
            return nullptr;

        if (uuid != nextMoveTrack->uuid) //action is on wrong track
            return nullptr;

        auto newMoveTrack = new MoveTrack (mc, uuid, nextMoveTrack->point);
        newMoveTrack->origPoint = origPoint;
        
        return newMoveTrack;
    }

private:
    MainComponent* mc;
    const String uuid;
    Point<int> point;
    Point<int> origPoint;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MoveTrack)
};

#endif //MOVETRACK_H_INCLUDED
