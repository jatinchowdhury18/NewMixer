#include "Marker.h"
/*
Marker::Marker (AudioPlayer* player, MarkerType type) :
    player (player),
    type (type)
{}

void Marker::paint (Graphics& g, Rectangle<float> bounds)
{
    float pos = (float) markPos;
    if (! mouseIsDragging)
    {
        auto playerPosition = type == LoopStart ? player->getLoopStart() : player->getLoopEnd();
        auto posFraction = (float) playerPosition / (float) player->getAudioLength();
        pos = posFraction * bounds.getWidth() + bounds.getX();
        markPos = roundToInt (pos);
    }

    g.drawLine (pos, bounds.getY(), pos, bounds.getBottom(), 2.0f);
}

bool Marker::mouseOver (const MouseEvent& event)
{
    const int r = 10;
    if (Range<int> ((int) markPos - r, (int) markPos + r).contains (event.x))
        mouseIsOver = true;
    else
        mouseIsOver = false;

    return mouseIsOver;
}

void Marker::dragging (const MouseEvent& event)
{
    mouseIsDragging = true;
    markPos = event.x;

    player->setLoopMarker (-1, type == LoopStart);
}

void Marker::endDrag (const MouseEvent& event, Rectangle<float> bounds)
{
    mouseIsDragging = false;

    markPos = event.x;
    auto posFraction = (float) markPos / bounds.getWidth();
    auto playerPosition = (int64) (posFraction * player->getAudioLength());

    player->setLoopMarker (playerPosition, type == LoopStart);
}
*/