#ifndef PAINTHELPER_H_INCLUDED
#define PAINTHELPER_H_INCLUDED

#include "Track.h"

class PaintHelper
{
public:
    static void paint (Track* track, Graphics& g)
    {
        //If a track is soloed, darken all non-soloed tracks
        const bool darken = track->getProcessor()->getSoloed() == TrackBase::SoloState::otherTrack;
        const float pos = (TrackConstants::width - track->getDiameter()) / 2.0f;

        paintCircle (track, g, pos, darken);

        if (track->getIsPlaying())
            paintMeter (track, g, darken);

        paintName (track, g, pos, darken);
        
        auto* inputProcessor = dynamic_cast<InputTrackProcessor*> (track->getProcessor());
        bool armed = inputProcessor == nullptr ? track->getAutoHelper().armed()
                     : track->getAutoHelper().armed() || inputProcessor->isArmed();
        bool recording = inputProcessor == nullptr ? track->getAutoHelper().isRecording()
                         : track->getAutoHelper().isRecording() || inputProcessor->isRecording();
        
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

    static void paintName (Track* track, Graphics& g, float pos, bool darken)
    {
        Colour nameColour = track->getColour().contrasting();
        g.setColour (darken ? nameColour.withAlpha (TrackConstants::darkAlpha) : nameColour);
        g.setFont (track->getDiameter() * 0.36f);

        g.drawFittedText (track->getShortName(), (int) pos, (int) pos, 
                          (int) track->getDiameter(), (int) track->getDiameter(), Justification::centred, 1);
    }

    static void paintMeter (Track* track, Graphics& g, bool darken)
    {
        g.setColour (darken ? track->getColour().withAlpha (TrackConstants::darkAlpha) : track->getColour());

        float rmsFactor = 1.0f + std::expf (std::logf (track->getProcessor()->getRMSLevel()) / 2.5f);

        for (float factor = 1.0f; factor < rmsFactor; factor += 0.1f)
        {
            const float pos = (TrackConstants::width - factor * track->getDiameter()) / 2.0f;
            g.drawEllipse (pos, pos, factor * track->getDiameter(),
                           factor * track->getDiameter(), track->getDiameter() / 40.0f);
        }
    }

    static void paintRing (Track* track, Graphics& g, float pos, Colour colour)
    {
        g.setColour (colour);
        g.drawEllipse (pos, pos, track->getDiameter(), track->getDiameter(), track->getDiameter() / 20.0f);
    }

    static void paintMute (Track* track, Graphics& g, float pos, bool darken)
    {
        g.setColour (darken ? Colours::goldenrod.withAlpha (TrackConstants::darkAlpha) : Colours::goldenrod);

        auto offset = (track->getDiameter() / 2.0f) * (MathConstants<float>::sqrt2 - 1.0f) / MathConstants<float>::sqrt2;

        g.drawLine (pos + offset, pos + offset, pos + track->getDiameter() - offset,
                    pos + track->getDiameter() - offset, track->getDiameter() / 20.0f);
    }

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PaintHelper)
};

#endif //PAINTHELPER_H_INCLUDED
