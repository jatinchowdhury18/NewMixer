#ifndef MASTERTRACKPROCESSOR_H_INCLUDED
#define MASTERTRACKPROCESSOR_H_INCLUDED

#include "Track.h"

struct ExportInfo
{
public:
    enum ExportFormat
    {
        WAV = 0x3232,
        AIFF,
    };

    ExportFormat format = WAV;
    int bitDepth = 32;
    double sampleRate = 44100.0;
    File exportFile;
    int samplesPerBlock = 256;
    int64 lengthSamples;

    static String getStringForFormat (ExportFormat format)
    {
        if (format == WAV)
            return "wav";
        else if (format == AIFF)
            return "aiff";
        
        return "";
    }
};

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

    void prepareToExport (ExportInfo exportInfo);
    void restoreAfterExporting();
    AudioSampleBuffer getAudioBuffer (int bufferLength);
    void exportToFile (ExportInfo exportInfo, ThreadWithProgressWindow* progress);

    class Listener
    {
    public:
        virtual ~Listener() {}
        virtual void exportCompleted() {}
    };

    void addListener (Listener* listener) { exportListeners.add (listener); }
    void removeListener (Listener* listener) { exportListeners.remove (listener); }

private:
    void connectTracks();

    ListenerList<Listener> exportListeners;

    AudioFormatManager formatManager;
    AudioDeviceManager deviceManager;
    AudioProcessorPlayer player;

    Node::Ptr audioOutputNode;
    Node::Ptr audioInputNode;
    ReferenceCountedArray<Node> trackNodes;
    OwnedArray<Track>& tracks;

    bool isPlaying = true;
    bool isExporting = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MasterTrackProcessor)
};

#endif //MASTERTRACKPROCESSOR_H_INCLUDED
