#include "AutomationHelper.h"
#include "MainComponent.h"
#include "Track.h"

namespace
{
    const Identifier autoPointsType = Identifier ("AutoPoints");
    const Identifier autoPoint = Identifier ("AutoPoint");
    const Identifier pointX = Identifier ("X");
    const Identifier pointY = Identifier ("Y");
    const Identifier pointDiameter = Identifier ("Diameter");
    const Identifier pointSample = Identifier ("Sample");
}

AutoHelper::AutoHelper (Component* parent) :
    autoPointsValueTree (autoPointsType),
    parent (parent)
{
}

AutoHelper::~AutoHelper()
{
}

UndoManager* AutoHelper::getUndoManager()
{
    auto track = dynamic_cast<Track*> (parent);
    if (track == nullptr)
    {
        // The parent track should always exist
        jassertfalse;
        return nullptr;
    }

    auto mc = dynamic_cast<MainComponent*> (track->getParentComponent());
    if (mc == nullptr)
    {
        // The main component should always exist, except for some unit tests
        return nullptr; 
    }

    return &mc->getUndoManager();
}

float AutoHelper::getPointX (const ValueTree& point) const noexcept { return (float) getPointValue (point, pointX); }
float AutoHelper::getPointY (const ValueTree& point) const noexcept { return (float) getPointValue (point, pointY); }
float AutoHelper::getPointDiameter (const ValueTree& point) const noexcept { return (float) getPointValue (point, pointDiameter); }
int64 AutoHelper::getPointSample (const ValueTree& point) const noexcept { return (int64) getPointValue (point, pointSample); }

var AutoHelper::getPointValue (const ValueTree& point, Identifier id) const noexcept
{
    if (point.isValid() && point.getType() == autoPoint)
        return point.getProperty (id);

    return var();
}

void AutoHelper::recordAutoPoint (float x, float y, float diameter, int64 sample)
{
    auto* undoManager = getUndoManager();
    if (! (undoManager->getCurrentTransactionName() == "Recording Automation" || undoManager->getCurrentTransactionName().contains ("Moving Track")))
        undoManager->beginNewTransaction ("Recording Automation");

    addAutoPoint (x, y, diameter, sample);
}

void AutoHelper::addAutoPoint (float x, float y, float diameter, int64 sample)
{
    // create value tree for new point
    ValueTree newPoint (autoPoint);
    newPoint.setProperty (pointX, x, nullptr);
    newPoint.setProperty (pointY, y, nullptr);
    newPoint.setProperty (pointDiameter, diameter, nullptr);
    newPoint.setProperty (pointSample, sample, nullptr);

    // check if same as last point
    const auto lastPoint = autoPointsValueTree.getChild (autoPointsValueTree.getNumChildren() - 1);
    if (lastPoint.isValid())
    {
        if (checkIsSamePoint (lastPoint, newPoint))
            return;
    }

    autoPointsValueTree.appendChild (newPoint, getUndoManager());
}

bool AutoHelper::checkIsSamePoint (const ValueTree& originalPoint, const ValueTree&newPoint)
{
    bool sameX = getPointX (originalPoint) == getPointX (newPoint);
    bool sameY = getPointY (originalPoint) == getPointY (newPoint);
    bool sameDiameter = getPointDiameter (originalPoint) == getPointDiameter (newPoint);

    return sameX && sameY && sameDiameter;
}

void AutoHelper::getPoint (float& x, float& y, float& diameter, int64 curSample)
{
    const auto numPoints = autoPointsValueTree.getNumChildren();

    if (numPoints <= 0)
        return;

    int newPoint = curPoint;
    for (int i = 0; i < numPoints; i++)
    {
        int lastPoint = newPoint;
        newPoint++;

        if (newPoint >= numPoints)
            newPoint %= numPoints;

        if (getPointSample (autoPointsValueTree.getChild (lastPoint)) <= curSample &&
            getPointSample (autoPointsValueTree.getChild (newPoint)) >= curSample)
        {
            const auto& curPointValueTree = autoPointsValueTree.getChild (curPoint);
            x = getPointX (curPointValueTree);
            y = getPointY (curPointValueTree);
            diameter = getPointDiameter (curPointValueTree);

            curPoint = newPoint;
            break;
        }
    }
}

void AutoHelper::setRecordingStatus()
{
    if (armedForRecording)
    {
        auto* undoManager = getUndoManager();
        undoManager->beginNewTransaction ("Recording Automation");
        autoPointsValueTree.removeAllChildren (undoManager);

        armedForRecording = false;
        recording = true;
        return;
    }
    if (recording)
    {
        recording = false;

        if (autoPointsValueTree.getNumChildren() > 1)
            automationRecorded = true;
        else
            throwAway();

        return;
    }
}

void AutoHelper::throwAway()
{
    auto* undoManager = getUndoManager();
    undoManager->beginNewTransaction ("Deleting Automation");
    autoPointsValueTree.removeAllChildren (undoManager);
    curPoint = 0;

    if (armedForRecording)
        armedForRecording = false;

    if (recording)
        recording = false;

    if (automationRecorded)
        automationRecorded = false;
}
