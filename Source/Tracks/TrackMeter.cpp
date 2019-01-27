#include "TrackMeter.h"
#include "Track.h"

TrackMeter::TrackMeter (Track* track) : track (track)
{
    setInterceptsMouseClicks (false, false);
    startTimer (10);
}

void TrackMeter::paint (Graphics& g)
{
    if (! track->getIsPlaying())
        return;

    bool darken = track->getProcessor()->getSoloed() == TrackBase::SoloState::otherTrack;
    g.setColour (darken ? track->getColour().withAlpha (TrackConstants::darkAlpha) : track->getColour());

    float rmsFactor = 1.0f + std::expf (std::logf (track->getProcessor()->getRMSLevel()) / 3.5f);

    for (float factor = 1.0f; factor < rmsFactor; factor += 0.08f)
    {
        const float pos = (TrackConstants::width - factor * track->getDiameter()) / 2.0f;
        g.drawEllipse (pos, pos, factor * track->getDiameter(),
                       factor * track->getDiameter(), track->getDiameter() / 60.0f);
    }
}
