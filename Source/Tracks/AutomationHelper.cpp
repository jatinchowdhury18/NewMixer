#include "AutomationHelper.h"

void AutoHelper::addAutoPoint (int x, int y, float diameter)
{
    points.add (new AutoPoint (x, y, diameter));

    numPoints++;
}

void AutoHelper::getPoint (int& x, int& y, float& diameter)
{
    x = points[curPoint]->x;
    y = points[curPoint]->y;
    diameter = points[curPoint]->diameter;

    curPoint++;
    if (curPoint >= numPoints)
        curPoint = 0;
}

void AutoHelper::setRecordingStatus()
{
    if (armedForRecording)
    {
        points.clear();

        armedForRecording = false;
        recording = true;
        return;
    }
    if (recording)
    {
        recording = false;
        automationRecorded = true;
        return;
    }
}

void AutoHelper::throwAway()
{
    points.clear();

    if (armedForRecording)
        armedForRecording = false;

    if (recording)
        recording = false;
}
