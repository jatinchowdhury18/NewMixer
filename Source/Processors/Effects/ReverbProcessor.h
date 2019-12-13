#ifndef REVERBPROCESSOR_H_INCLUDED
#define REVERBPROCESSOR_H_INCLUDED

#include "ProcessorBase.h"

class ReverbProcessor : public AudioPluginInstance
{
public:
    ReverbProcessor();
    ~ReverbProcessor() {}

    void fillInPluginDescription (PluginDescription& desc) const override;

    void prepareToPlay (double sampleRate, int maxExpectedBlockSize) override;
    void releaseResources() override;
    void processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiBuffer) override;

    void setParameters();

    const String getName() const override { return "Reverb"; }

    double getTailLengthSeconds() const override { return 0.0; }

    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }

    AudioProcessorEditor* createEditor() override { return new GenericAudioProcessorEditor (*this); };
    bool hasEditor() const override { return true; }

    int getNumPrograms() override { return 0; }
    void setCurrentProgram (int /*index*/) override {}
    int getCurrentProgram() override { return 0; }

    const String getProgramName (int /*index*/) override { return {}; }
    void changeProgramName (int /*index*/, const String& /*newName*/) override {}

    void getStateInformation (MemoryBlock& /*destData*/) override {}
    void setStateInformation (const void* /*data*/, int /*sizeInBytes*/) override {}

private:
    AudioProcessorValueTreeState vts;

    float* sizeParam;
    float* dampParam;
    float* widthParam;
    float* dwParam;

    dsp::Reverb::Parameters params;
    dsp::Reverb reverbDsp;

    bool starting = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ReverbProcessor)
};

#endif //REVERBPROCESSOR_H_INCLUDED
