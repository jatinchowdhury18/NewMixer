#ifndef TRACKBASE_H_INCLUDED
#define TRACKBASE_H_INCLUDED

#include "ProcessorBase.h"
#include "Effects/GainProcessor.h"
#include "Effects/PanProcessor.h"
#include "Effects/DelayProcessor.h"
#include "Effects/DistanceProcessor.h"
#include "Effects/ReverbProcessor.h"

class TrackBase : public ProcessorBase
{
public:
    enum SoloState
    {
        thisTrack,
        otherTrack,
        noTracks,
    };
    
    TrackBase (String name);
    
    void initProcessors();
    
    void prepareToPlay (double sampleRate, int maximumExpectedSamplesPerBlock) override;
    void releaseResources() override;
    virtual void processBlock (AudioBuffer<float> &buffer, MidiBuffer &midiMessages) override;
    
    void trackMoved (int x, int y, int width);
    
    void setMute (bool mute) { isMute = mute; }
    bool getIsMute() { return isMute; }
    
    SoloState getSoloed() { return soloState; }
    void setSoloed (SoloState state) { soloState = state; }
    
    void rewind() { readerStartSample = 0; }
    float getRMSLevel() { return jmin<float> (lastRMS, 1.0f); }
    
    virtual int64 getLengthSamples() const = 0;
    int64 getStartSample() const { return readerStartSample; }
    
    class Listener
    {
    public:
        virtual ~Listener() {}
        virtual void newLoop() {}
    };
    
    void addListener (Listener* listener) { listeners.add (listener); }
    void removeListener (Listener* listener) { listeners.remove (listener); }

protected:
    ListenerList<Listener> listeners;
    
    int64 readerStartSample = 0;
    
private:
    Array<AudioProcessor*> processors;
    std::unique_ptr<GainProcessor> gainProcessor;
    std::unique_ptr<DelayProcessor> delayProcessor;
    std::unique_ptr<PanProcessor> panProcessor;
    std::unique_ptr<DistanceProcessor> distProcessor;

    void updateGain (int width);
    void updateDelay (int x, int y);
    void updatePan (int x);
    void updateDist (int y);
    
    bool isMute = true;
    SoloState soloState = noTracks;
    float lastRMS = 0.0f;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TrackBase)
};

#endif // TRACKBASE_H_INCLUDED
