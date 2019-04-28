#ifndef PLUGINEFFECTSCHAIN_H_INCLUDED
#define PLUGINEFFECTSCHAIN_H_INCLUDED

#include "ProcessorBase.h"

class PluginEffectsChain : public ProcessorBase
{
public:
    PluginEffectsChain() : ProcessorBase ("Effects Chain") {}

    void prepareToPlay (double sampleRate, int maxBlockSize) override;
    void releaseResources() override;
    void processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages) override;

    int getNumPlugins() const { return pluginList.size(); }
    void addPlugin (const PluginDescription* pluginDescription, int index = -1);
    void movePlugin (int oldIndex, int newIndex);
    void removePlugin (int index);
    void toggleBypassPlugin (int index);
    bool isPluginBypassed (int index);
    String getPluginName (int index) const { return pluginList[index]->getName(); }

    AudioProcessorEditor* getPluginEditor (int index);
    OwnedArray<AudioPluginInstance>& getPluginList() { return pluginList; }

private:
    OwnedArray<AudioPluginInstance> pluginList;
    Array<bool> bypasses;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginEffectsChain)
};

#endif //PLUGINEFFECTSCHAIN_H_INCLUDED
