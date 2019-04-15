#include "TrackMeter.h"
#include "Track.h"

namespace
{
    constexpr float skewFactor = 3.5f;
    constexpr float circleSpacing = 0.08f;
    constexpr float circleThickFactor = 60.0f;

    enum
    {
        repaintRate = 100, //Hz
    };
}

TrackMeter::TrackMeter (Track* track) : track (track)
{
    setInterceptsMouseClicks (false, false);
    startTimerHz (repaintRate);
}

void TrackMeter::paint (Graphics& g)
{
    if (! track->getIsPlaying())
        return;

    bool darken = track->getProcessor()->getSoloed() == TrackBase::SoloState::otherTrack;
    g.setColour (darken ? track->getColour().withAlpha (TrackConstants::darkAlpha) : track->getColour());

    float rmsFactor = 1.0f + std::expf (std::logf (track->getProcessor()->getRMSLevel()) / skewFactor);

    for (float factor = 1.0f; factor < rmsFactor; factor += circleSpacing)
    {
        const float pos = (TrackConstants::width - factor * track->getDiameter()) / 2.0f;
        g.drawEllipse (pos, pos, factor * track->getDiameter(),
                       factor * track->getDiameter(), track->getDiameter() / circleThickFactor);
    }
}
