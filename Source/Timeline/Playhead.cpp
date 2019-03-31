#include "Playhead.h"
#include "Track.h"

Playhead::Playhead (Array<TrackBase*>& trackProcs) : procs (trackProcs)
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
        auto posFraction = procs.isEmpty() ? 0.0f : (float) procs[0]->getStartSample() / (float) procs[0]->getLengthSamples();
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

    pos = jmax (e.x, 0);
    auto posFraction = (float) pos / getBounds().getWidth();
    auto trackPosition = (int64) (posFraction * procs[0]->getLengthSamples());

    listeners.call (&Playhead::Listener::playheadMoving, trackPosition);
}

void Playhead::mouseUp (const MouseEvent& e)
{
    mouseIsDragging = false;

    listeners.call (&Playhead::Listener::playheadMoving, -1);

    pos = jmax (e.x, 0);
    auto posFraction = (float) pos / getBounds().getWidth();
    auto trackPosition = (int64) (posFraction * procs[0]->getLengthSamples());

    for (auto proc : procs)
        proc->setStartSample (trackPosition);
}
