#ifndef MOVETRACK_H_INCLUDED
#define MOVETRACK_H_INCLUDED

#include "TrackActionHelper.h"

class MoveTrack : public UndoableAction
{
public:
    MoveTrack (Track* track, Point<int> point) :
        track (track),
        point (point)
    {
        origPoint = track->getBoundsInParent().getTopLeft(); 
    }

    ~MoveTrack() {}

    bool perform() override
    {
        TrackActionHelper::setRelPosition (track, point);
        TrackActionHelper::setPositionConstrained (track, point);
        track->trackMoved();
        return true;
    }

    bool undo() override
    {
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

        auto newMoveTrack = new MoveTrack (track, nextMoveTrack->point);
        newMoveTrack->origPoint = origPoint;

        return newMoveTrack;
    }


private:
    Track* track;
    Point<int> point;
    Point<int> origPoint;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MoveTrack)
};

#endif //MOVETRACK_H_INCLUDED
