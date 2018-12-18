#include "MasterTrack.h"

MasterTrack::MasterTrack (Track* track)
{
    deviceManager.initialiseWithDefaultDevices (0, 2);
    deviceManager.addAudioCallback (&player);


    audioOutputNode = addNode (new AudioGraphIOProcessor (AudioGraphIOProcessor::audioOutputNode));

    track->getProcessor()->setPlayConfigDetails (getMainBusNumInputChannels(), getMainBusNumOutputChannels(), getSampleRate(), getBlockSize());

    trackNode = addNode (track->getProcessor());

    for (int channel = 0; channel < 2; ++channel)
    {
        addConnection ({ { trackNode->nodeID, channel },
            { audioOutputNode->nodeID, channel } });
    }

    player.setProcessor (this);
}

MasterTrack::~MasterTrack()
{
    deviceManager.removeAudioCallback (&player);
}
