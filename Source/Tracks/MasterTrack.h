#ifndef MASTERTRACK_H_INCLUDED
#define MASTERTRACK_H_INCLUDED

#include "JuceHeader.h"
#include "Track.h"

class MasterTrack : public AudioProcessorGraph
{
public:
    using AudioGraphIOProcessor = AudioProcessorGraph::AudioGraphIOProcessor;
    using Node = AudioProcessorGraph::Node;

    MasterTrack (Track* track);
    ~MasterTrack();


private:
    AudioDeviceManager deviceManager;
    AudioProcessorPlayer player;

    Node::Ptr audioOutputNode;
    Node::Ptr trackNode;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MasterTrack)
};

#endif //MASTERTRACK_H_INCLUDED
