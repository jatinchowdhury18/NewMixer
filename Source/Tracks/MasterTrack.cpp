#include "MasterTrack.h"

MasterTrack::MasterTrack (OwnedArray<Track>& tracks)
{
    deviceManager.initialiseWithDefaultDevices (0, 2);
    deviceManager.addAudioCallback (&player);

    audioOutputNode = addNode (new AudioGraphIOProcessor (AudioGraphIOProcessor::audioOutputNode));
    audioOutputNode->getProcessor()->setPlayConfigDetails (2, 2, getSampleRate(), getBlockSize());

    for (auto track : tracks)
        trackNodes.add (addNode (track->getProcessor()));
    connectTracks();

    player.setProcessor (this);
}

MasterTrack::~MasterTrack()
{
    deviceManager.removeAudioCallback (&player);
}

void MasterTrack::connectTracks()
{
    for (auto trackNode : trackNodes)
    {
        for (int channel = 0; channel < 2; ++channel)
        {
            addConnection ({ { trackNode->nodeID,       channel },
                             { audioOutputNode->nodeID, channel } });
        }
    }
}
