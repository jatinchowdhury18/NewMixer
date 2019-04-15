#ifndef MASTERTRACKPROCESSOR_H_INCLUDED
#define MASTERTRACKPROCESSOR_H_INCLUDED

#include "Track.h"

class MasterTrackProcessor : public AudioProcessorGraph
{
public:
    using AudioGraphIOProcessor = AudioProcessorGraph::AudioGraphIOProcessor;
    using Node = AudioProcessorGraph::Node;

    MasterTrackProcessor (OwnedArray<Track>& tracks);
    ~MasterTrackProcessor();

    AudioDeviceManager& getDeviceManager() { return deviceManager; }

    void processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiBuffer) override;

    void togglePlay();
    bool getIsPlaying() { return isPlaying; }

    void addTrack (Track* track);
    void removeTrack (Track* track);

private:
    void connectTracks();

    AudioDeviceManager deviceManager;
    AudioProcessorPlayer player;

    Node::Ptr audioOutputNode;
    Node::Ptr audioInputNode;
    ReferenceCountedArray<Node> trackNodes;

    bool isPlaying = true;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MasterTrackProcessor)
};

#endif //MASTERTRACKPROCESSOR_H_INCLUDED
