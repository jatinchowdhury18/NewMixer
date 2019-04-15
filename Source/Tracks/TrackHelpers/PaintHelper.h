#ifndef PAINTHELPER_H_INCLUDED
#define PAINTHELPER_H_INCLUDED

#include "Track.h"

namespace TrackPaintConstants
{
    constexpr float darkenFactor = 0.25f;
    constexpr float timeThickness = 0.88f;
    constexpr float nameFont = 0.36f;
    constexpr float lineThickness = 20.0f;
}

class PaintHelper
{
public:
    static void paint (Track* track, Graphics& g)
    {
        //If a track is soloed, darken all non-soloed tracks
        const bool darken = track->getProcessor()->getSoloed() == TrackBase::SoloState::otherTrack;
        const float pos = (TrackConstants::width - track->getDiameter()) / 2.0f;

        paintCircle (track, g, pos, darken);

        paintTimeline (track, g, pos, darken);

        paintName (track, g, pos, darken);
        
        auto* inputProcessor = dynamic_cast<InputTrackProcessor*> (track->getProcessor());
        bool armed = inputProcessor == nullptr ? track->getAutoHelper()->armed()
                     : track->getAutoHelper()->armed() || inputProcessor->isArmed();
        bool recording = inputProcessor == nullptr ? track->getAutoHelper()->isRecording()
                         : track->getAutoHelper()->isRecording() || inputProcessor->isRecording();
        
        if (armed)
            paintRing (track, g, pos, Colours::deeppink);
        else if (recording)
            paintRing (track, g, pos, Colours::red);
        else if (track->getIsSelected()) //highlight selected track
            paintRing (track, g, pos, Colours::goldenrod);

        if (track->getProcessor()->getIsMute())
            paintMute (track, g, pos, darken);
    }
    
    static void paintCircle (Track* track, Graphics& g, float pos, bool darken)
    {
        g.setColour (darken ? track->getColour().withAlpha (TrackConstants::darkAlpha) : track->getColour());
        g.fillEllipse (pos, pos, track->getDiameter(), track->getDiameter());
    }

    static void paintTimeline (Track* track, Graphics& g, float pos, bool darken)
    {
        Colour timeColour = Colours::goldenrod.darker (TrackPaintConstants::darkenFactor);
        g.setColour (darken ? timeColour.withAlpha (TrackConstants::darkAlpha) : timeColour);

        float loopFraction = (float) track->getProcessor()->getStartSample() / (float) track->getProcessor()->getLengthSamples();

        const auto playheadPos = track->getPlayheadPos();
        if (playheadPos >= 0)
            loopFraction = (float) playheadPos / (float) track->getProcessor()->getLengthSamples();

        Path p;
        p.addPieSegment (pos, pos, track->getDiameter(), track->getDiameter(),
                         0.0f, loopFraction * MathConstants<float>::twoPi, TrackPaintConstants::timeThickness);
        g.fillPath (p);
    }

    static void paintName (Track* track, Graphics& g, float pos, bool darken)
    {
        Colour nameColour = track->getColour().contrasting();
        g.setColour (darken ? nameColour.withAlpha (TrackConstants::darkAlpha) : nameColour);
        g.setFont (track->getDiameter() * TrackPaintConstants::nameFont);

        g.drawFittedText (track->getShortName(), (int) pos, (int) pos, 
                          (int) track->getDiameter(), (int) track->getDiameter(), Justification::centred, 1);
    }

    static void paintRing (Track* track, Graphics& g, float pos, Colour colour)
    {
        g.setColour (colour);
        g.drawEllipse (pos, pos, track->getDiameter(), track->getDiameter(), track->getDiameter() / TrackPaintConstants::lineThickness);
    }

    static void paintMute (Track* track, Graphics& g, float pos, bool darken)
    {
        g.setColour (darken ? Colours::goldenrod.withAlpha (TrackConstants::darkAlpha) : Colours::goldenrod);

        auto offset = (track->getDiameter() / 2.0f) * (MathConstants<float>::sqrt2 - 1.0f) / MathConstants<float>::sqrt2;

        g.drawLine (pos + offset, pos + offset, pos + track->getDiameter() - offset,
                    pos + track->getDiameter() - offset, track->getDiameter() / TrackPaintConstants::lineThickness);
    }

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PaintHelper)
};

#endif //PAINTHELPER_H_INCLUDED
