#ifndef RECORDAUTOMATION_H_INCLUDED
#define RECORDAUTOMATION_H_INCLUDED

#include "Track.h"
#include "TrackHelpers/AutomationHelper.h"
#include "MoveTrack.h"

class RecordAutomation : public UndoableAction
{
public:
    RecordAutomation (MainComponent* mc, String uuid, float x, float y, float diameter, int64 sample) :
        mc (mc),
        uuid (uuid)
    {
        autoPoints.add (new AutoHelper::AutoPoint (x, y, diameter, sample));
    }

    RecordAutomation (MainComponent* mc, String uuid, OwnedArray<AutoHelper::AutoPoint>& newAutoPoints) :
        mc (mc),
        uuid (uuid)
    {
        for (int i = 0; i < newAutoPoints.size(); ++i)
            autoPoints.add (newAutoPoints.removeAndReturn (i));
    }

    bool perform() override
    {
        if (autoPoints.isEmpty())
            return false;

        auto track = ActionHelper::getTrackWithUuid (mc, uuid);
        if (track == nullptr)
            return false;

        for (auto p : autoPoints)
            track->getAutoHelper()->addAutoPoint (p->x, p->y, p->diameter, p->sample);

        track->trackMoved();
        return true;
    }

    bool undo() override
    {
        if (autoPoints.isEmpty())
            return false;

        auto track = ActionHelper::getTrackWithUuid (mc, uuid);
        if (track == nullptr)
            return false;

        track->getAutoHelper()->throwAway();
        track->repaint();
        track->getParentComponent()->repaint();
        return true;
    }

    UndoableAction* createCoalescedAction (UndoableAction* nextAction)
    {
        auto nextRecordAuto = dynamic_cast<RecordAutomation*> (nextAction);

        if (nextRecordAuto == nullptr)
            return nullptr;

        if (uuid != nextRecordAuto->uuid) //action is on wrong track
            return nullptr;

        auto newRecordAuto = new RecordAutomation (mc, uuid, autoPoints);
        for (int i = 0; i < nextRecordAuto->autoPoints.size(); ++i)
            autoPoints.add (nextRecordAuto->autoPoints.removeAndReturn (i));

        return newRecordAuto;
    }

private:
    MainComponent* mc;
    const String uuid;
    OwnedArray<AutoHelper::AutoPoint> autoPoints;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RecordAutomation)
};

#endif //RECORDAUTOMATION_H_INCLUDED
