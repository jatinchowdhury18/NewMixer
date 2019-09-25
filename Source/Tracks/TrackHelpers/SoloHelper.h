#ifndef SOLOHELPER_H_INCLUDED
#define SOLOHELPER_H_INCLUDED

#include "Track.h"

class SoloHelper
{
public:
    static void soloButtonPressed (const OwnedArray<Track>& tracks)
    {
        bool aTrackIsSoloed = setSelectedTrackSolo (tracks);
        setOtherTracksSolo (tracks, aTrackIsSoloed);
    }

private:
    static bool setSelectedTrackSolo (const OwnedArray<Track>& tracks)
    {
        for (auto track : tracks)
        {
            if (track->getIsSelected())
            {
                if (track->isSoloed())
                {
                    track->setSoloed (TrackBase::SoloState::noTracks);
                    return false;
                }
                else
                {
                    track->setSoloed (TrackBase::SoloState::thisTrack);
                    return true;
                }
            }
        }

        return false;
    }

    static void setOtherTracksSolo (const OwnedArray<Track>& tracks, bool aTrackIsSoloed)
    {
        for (auto track : tracks)
        {
            if (! track->getIsSelected())
                track->setSoloed (aTrackIsSoloed ? TrackBase::SoloState::otherTrack
                                                 : TrackBase::SoloState::noTracks);
        }
    }
};

//==================================================================
class SoloAction : public UndoableAction
{
public:
    SoloAction (const OwnedArray<Track>& tracks) :
        tracks (tracks)
    {}
    ~SoloAction() {}

    bool perform() override
    {
        if (tracks.isEmpty())
            return false;

        SoloHelper::soloButtonPressed (tracks);
        return true;
    }

    bool undo() override
    {
        return perform();
    }

private:
    const OwnedArray<Track>& tracks;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SoloAction)
};

#endif //SOLOHELPER_H_INCLUDED
