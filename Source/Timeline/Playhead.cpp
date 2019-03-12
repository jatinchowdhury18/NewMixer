#include "Playhead.h"

Playhead::Playhead (TrackBase* trackProc) : proc (trackProc)
{
    startTimer (10);
}

void Playhead::timerCallback()
{
    repaint();
}

void Playhead::paint (Graphics& g)
{
    g.setColour (Colours::dodgerblue);

    auto curPos = (float) pos;
    if (! mouseIsDragging)
    {
        auto posFraction = (float) proc->getStartSample() / (float) proc->getLengthSamples();
        curPos = (float) (posFraction * getWidth() + getX());
        pos = roundToInt (curPos);
    }

    g.drawLine ((float) pos, (float) getY(), (float) pos, (float) getBottom(), 3.0f);
}

void Playhead::mouseMove (const MouseEvent& e)
{
    const int r = 10;
    if (Range<int> ((int) pos - r, (int) pos + r).contains (e.x))
        setMouseCursor (MouseCursor::LeftRightResizeCursor);
    else
        setMouseCursor (MouseCursor::NormalCursor);
}

void Playhead::mouseDrag (const MouseEvent& e)
{
    mouseIsDragging = true;
    pos = e.x;

    setMouseCursor (MouseCursor::LeftRightResizeCursor);
}

void Playhead::mouseUp (const MouseEvent& e)
{
    mouseIsDragging = false;

    pos = e.x;
    auto posFraction = (float) pos / getBounds().getWidth();
    auto trackPosition = (int64) (posFraction * proc->getLengthSamples());

    proc->setStartSample (trackPosition);
}
