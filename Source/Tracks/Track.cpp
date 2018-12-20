#include "Track.h"

Track::Track (File& file, String name, int x, int y) : name (name)
{
    processor = new TrackProcessor (file);

    setTopLeftPosition (x, y);

    setTooltip (name);
}

void Track::paint (Graphics& g)
{
    g.setColour (Colours::darkred);

    auto diameter = (float) getWidth();
    g.fillEllipse (0.f, 0.f, diameter, diameter);
}

void Track::mouseDrag (const MouseEvent& e)
{
    if (e.mods.isCtrlDown())
    {
        e.source.enableUnboundedMouseMovement (true);
        const int initValue = getWidth();
        int changeVal = 0;
        const int curY = e.getDistanceFromDragStartY();
        const int stepSize = 1;  

        if (curY < lastDragLocation)
            changeVal = stepSize; //up
        else if (curY > lastDragLocation)
            changeVal = -stepSize; //down

        int newSize = jlimit<int> (minWidth, maxWidth, initValue + changeVal);

        if (newSize == initValue)
            return;

        setSize (newSize, newSize);
        isDragging = true;
        lastDragLocation = curY;

        return;
    }

    DragAndDropContainer* dragContainer = DragAndDropContainer::findParentDragContainerFor (this); 

    if (! dragContainer->isDragAndDropActive())
    {
        dragContainer->startDragging (name, this);
    }
}

void Track::mouseUp (const MouseEvent& /*e*/)
{
    if (isDragging)
    {
        isDragging = false;
        lastDragLocation = 0;
    }
}
