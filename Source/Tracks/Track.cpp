#include "Track.h"

Track::Track (File& file, String name, int x, int y, Colour colour) : 
    name (name),
    trackColour (colour)
{
    processor = new TrackProcessor (file);

    setTopLeftPosition (x, y);

    setTooltip (name);
}

void Track::paint (Graphics& g)
{
    auto diameter = (float) getWidth();

    g.setColour (trackColour);
    g.fillEllipse (0.f, 0.f, diameter, diameter);

    if (isSelected)
    {
        g.setColour (Colours::goldenrod);
        g.drawEllipse (0.0f, 0.0f, diameter, diameter, 2.5f);
    }
}

void Track::changeSize (const MouseEvent& e)
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
}

void Track::changePosition (const MouseEvent& e)
{
    Point<int> newPos = e.getEventRelativeTo (getParentComponent()).position.toInt();
    newPos.x -= getWidth() / 2;
    newPos.y -= getWidth() / 2;

    setTopLeftPosition (newPos);
    resized();
}

void Track::mouseDown (const MouseEvent& e)
{
    getParentComponent()->mouseDown (e);

    isSelected = true;
    repaint();
}

void Track::mouseDrag (const MouseEvent& e)
{
    if (e.mods.isCtrlDown())  //Change volume
        changeSize (e);
    else                      // Normal drag
        changePosition (e);

    getParentComponent()->repaint();
}

void Track::mouseUp (const MouseEvent& /*e*/)
{
    if (isDragging)
    {
        isDragging = false;
        lastDragLocation = 0;
    }
}
