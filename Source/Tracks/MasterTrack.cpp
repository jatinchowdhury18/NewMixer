#include "MasterTrack.h"

MasterTrack::MasterTrack (OwnedArray<Track>& tracks)
{
    deviceManager.initialiseWithDefaultDevices (2, 2);
    deviceManager.addAudioCallback (&player);

    audioOutputNode = addNode (new AudioGraphIOProcessor (AudioGraphIOProcessor::audioOutputNode));
    audioOutputNode->getProcessor()->setPlayConfigDetails (2, 2, getSampleRate(), getBlockSize());

    for (auto track : tracks)
        trackNodes.add (addNode (track->getProcessor()));
    connectTracks();

    player.setProcessor (this);

    togglePlay();
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

void MasterTrack::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiBuffer)
{
    if (! isPlaying)
    {
        buffer.clear();
        return;
    }

    AudioProcessorGraph::processBlock (buffer, midiBuffer);
}

void MasterTrack::togglePlay()
{
    if (! isPlaying)
        prepareToPlay (getSampleRate(), getBlockSize());
    else
        releaseResources();

    isPlaying = ! isPlaying;
}
