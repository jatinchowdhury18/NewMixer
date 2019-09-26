#ifndef RECORDAUTOMATION_H_INCLUDED
#define RECORDAUTOMATION_H_INCLUDED

#include "Track.h"
#include "TrackHelpers/AutomationHelper.h"
#include "MoveTrack.h"

class RecordAutomation : public UndoableAction
{
public:
    RecordAutomation (Track* track, float x, float y, float diameter, int64 sample) :
        track (track)
    {
        autoPoints.add (new AutoHelper::AutoPoint (x, y, diameter, sample));
    }

    RecordAutomation (Track* track, OwnedArray<AutoHelper::AutoPoint>& newAutoPoints) :
        track (track)
    {
        for (int i = 0; i < newAutoPoints.size(); ++i)
            autoPoints.add (newAutoPoints.removeAndReturn (i));
    }

    bool perform() override
    {
        if (autoPoints.isEmpty())
            return false;

        for (auto p : autoPoints)
            track->getAutoHelper()->addAutoPoint (p->x, p->y, p->diameter, p->sample);
        return true;
    }

    bool undo() override
    {
        if (autoPoints.isEmpty())
            return false;

        track->getAutoHelper()->throwAway();
        track->repaint();
        track->getParentComponent()->repaint();
        return true;
    }

    UndoableAction* createCoalescedAction (UndoableAction* nextAction)
    {
        auto nextRecordAuto = dynamic_cast<RecordAutomation*> (nextAction);

        if (nextRecordAuto == nullptr || nextRecordAuto->track != track)
            return nullptr;

        auto newRecordAuto = new RecordAutomation (track, autoPoints);
        for (int i = 0; i < nextRecordAuto->autoPoints.size(); ++i)
            autoPoints.add (nextRecordAuto->autoPoints.removeAndReturn (i));

        return newRecordAuto;
    }

private:
    Track* track;
    OwnedArray<AutoHelper::AutoPoint> autoPoints;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RecordAutomation)
};

#endif //RECORDAUTOMATION_H_INCLUDED
