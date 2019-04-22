#include "PluginEffectsChain.h"
#include "Data Managing/PluginManager.h"

void PluginEffectsChain::prepareToPlay (double sampleRate, int maxBlockSize)
{
    setRateAndBufferSizeDetails (sampleRate, maxBlockSize);

    for (auto plugin : pluginList)
        plugin->prepareToPlay (sampleRate, maxBlockSize);
}

void PluginEffectsChain::releaseResources()
{
    for (auto plugin : pluginList)
        plugin->releaseResources();
}

void PluginEffectsChain::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    for (auto plugin : pluginList)
        plugin->processBlock (buffer, midiMessages);
}

void PluginEffectsChain::addPlugin (const PluginDescription* description, int index)
{
    auto& pluginFormatManager = PluginManager::getInstance()->getPluginFormatManager();

    String errorMessage;
    auto* plugin = pluginFormatManager.createPluginInstance (*description, getSampleRate(), getBlockSize(), errorMessage);

    if (index < 0)
        pluginList.add (plugin);
    else
        pluginList.insert (jmin (index, pluginList.size()-1), plugin);
}

void PluginEffectsChain::movePlugin (int oldIndex, int newIndex)
{
    auto* plugin = pluginList.removeAndReturn (oldIndex);
    pluginList.insert (newIndex, plugin);
}

void PluginEffectsChain::removePlugin (int index)
{
    pluginList.remove (index);
}

AudioProcessorEditor* PluginEffectsChain::getPluginEditor (int index)
{
    return pluginList[index]->createEditorIfNeeded();
}
