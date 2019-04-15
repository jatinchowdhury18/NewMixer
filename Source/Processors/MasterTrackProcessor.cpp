#include "MasterTrackProcessor.h"
#include "InputTrackProcessor.h"

MasterTrackProcessor::MasterTrackProcessor (OwnedArray<Track>& tracks)
{
    deviceManager.initialiseWithDefaultDevices (2, 2);
    deviceManager.addAudioCallback (&player);

    audioOutputNode = addNode (new AudioGraphIOProcessor (AudioGraphIOProcessor::audioOutputNode));
    audioOutputNode->getProcessor()->setPlayConfigDetails (2, 2, getSampleRate(), getBlockSize());

    audioInputNode = addNode (new AudioGraphIOProcessor (AudioGraphIOProcessor::audioInputNode));
    audioInputNode->getProcessor()->setPlayConfigDetails (2, 2, getSampleRate(), getBlockSize());

    for (auto track : tracks)
        trackNodes.add (addNode (track->getProcessor()));
    connectTracks();

    player.setProcessor (this);

    togglePlay();
}

MasterTrackProcessor::~MasterTrackProcessor()
{
    deviceManager.removeAudioCallback (&player);
}

void MasterTrackProcessor::connectTracks()
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

void MasterTrackProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiBuffer)
{
    if (! isPlaying)
    {
        buffer.clear();
        return;
    }

    AudioProcessorGraph::processBlock (buffer, midiBuffer);
}

void MasterTrackProcessor::togglePlay()
{
    if (! isPlaying)
        prepareToPlay (getSampleRate(), getBlockSize());
    else
        releaseResources();

    isPlaying = ! isPlaying;
}

void MasterTrackProcessor::addTrack (Track* track)
{
    trackNodes.add (addNode (track->getProcessor()));
    auto trackNode = trackNodes.getLast();

    for (int channel = 0; channel < 2; ++channel)
    {
        addConnection ({ { audioInputNode->nodeID, channel },
                         { trackNode->nodeID,      channel } });

        addConnection ({ { trackNode->nodeID,       channel },
                         { audioOutputNode->nodeID, channel } });
    }
}

void MasterTrackProcessor::removeTrack (Track* track)
{
    ReferenceCountedObjectPtr<Node> nodeToDelete;
    for (int i = 0; i < trackNodes.size(); i++)
        if (track->getProcessor() == trackNodes[i]->getProcessor())
            nodeToDelete = trackNodes.removeAndReturn (i);

    removeNode (nodeToDelete.get());
    nodeToDelete.reset();
}
