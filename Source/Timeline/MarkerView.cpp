#include "MarkerView.h"
/*
MarkerView::MarkerView (AudioPlayer* player) :
    player (player),
    startMark (player, LoopStart),
    endMark (player, LoopEnd)
{
    setBounds (0, 0, 600, 400);
}

void MarkerView::paint (Graphics& g)
{
    g.setColour (Colours::dodgerblue);

    startMark.paint (g, getBounds().toFloat());
    endMark.paint (g, getBounds().toFloat());
}

void MarkerView::mouseMove (const MouseEvent& event)
{
    auto overStart = startMark.mouseOver (event);
    auto overEnd = endMark.mouseOver (event);

    if (overStart || overEnd)
        setMouseCursor (MouseCursor::LeftRightResizeCursor);
    else
        setMouseCursor (MouseCursor::NormalCursor);
}

void MarkerView::mouseDrag (const MouseEvent& event)
{
    if (startMark.isOver())
        startMark.dragging (event);
    else if (endMark.isOver())
        endMark.dragging (event);
}

void MarkerView::mouseUp (const MouseEvent& event)
{
    if (startMark.isDragging())
        startMark.endDrag (event, getBounds().toFloat());
    else if (endMark.isDragging())
        endMark.endDrag (event, getBounds().toFloat());

    repaint();
}
*/