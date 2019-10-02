#include "AutomationHelper.h"
#include "MainComponent.h"
#include "Track.h"
#include "RecordAutomation.h"

void AutoHelper::recordAutoPoint (Component* parent, float x, float y, float diameter, int64 sample)
{
    auto track = dynamic_cast<Track*> (parent);
    
    if (track != nullptr)
    {
        auto mc = dynamic_cast<MainComponent*> (track->getParentComponent());
        auto& undoManager = mc->getUndoManager();

        if (! (undoManager.getCurrentTransactionName() == "Recording Automation" || undoManager.getCurrentTransactionName().contains ("Moving Track")))
            undoManager.beginNewTransaction ("Recording Automation");

        undoManager.perform (new RecordAutomation (mc, track->getUuid(), x, y, diameter, sample));
    }
}

void AutoHelper::addAutoPoint (float x, float y, float diameter, int64 sample)
{
    const auto lastPoint = points.getLast();

    if (lastPoint != nullptr)
    {
        if (lastPoint->x == x && lastPoint->y == y && lastPoint->diameter)
            return;
    }

    points.add (new AutoPoint (x, y, diameter, sample));
    numPoints++;
}

void AutoHelper::getPoint (float& x, float& y, float& diameter, int64 curSample)
{
    if (numPoints <= 0)
        return;

    int newPoint = curPoint;
    for (int i = 0; i < numPoints; i++)
    {
        int lastPoint = newPoint;
        newPoint++;

        if (newPoint >= numPoints)
            newPoint %= numPoints;

        if (points[lastPoint]->sample <= curSample && points[newPoint]->sample >= curSample)
        {
            x = points[curPoint]->x;
            y = points[curPoint]->y;
            diameter = points[curPoint]->diameter;

            curPoint = newPoint;
            break;
        }
    }
}

void AutoHelper::setRecordingStatus()
{
    if (armedForRecording)
    {
        points.clear();
        numPoints = 0;

        armedForRecording = false;
        recording = true;
        return;
    }
    if (recording)
    {
        recording = false;

        if (numPoints > 1)
            automationRecorded = true;
        else
            throwAway();

        return;
    }
}

void AutoHelper::throwAway()
{
    points.clear();
    numPoints = 0;
    curPoint = 0;

    if (armedForRecording)
        armedForRecording = false;

    if (recording)
        recording = false;

    if (automationRecorded)
        automationRecorded = false;
}
