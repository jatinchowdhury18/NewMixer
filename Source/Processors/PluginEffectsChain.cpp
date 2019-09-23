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
    for (int i = 0; i < pluginList.size(); i++)
    {
        if (bypasses[i])
        {
            pluginList[i]->processBlockBypassed (buffer, midiMessages);
            return;
        }
        pluginList[i]->processBlock (buffer, midiMessages);
    }
}

void PluginEffectsChain::addPlugin (const PluginDescription* description, int index)
{
    auto& pluginFormatManager = PluginManager::getInstance()->getPluginFormatManager();

    String errorMessage;
    auto plugin = pluginFormatManager.createPluginInstance (*description, getSampleRate(), getBlockSize(), errorMessage);

    if (index < 0)
    {
        pluginList.add (plugin.release());
        pluginList.getLast()->prepareToPlay (getSampleRate(), getBlockSize());
        bypasses.add (false);
    }
    else
    {
        int corrIndex = jmin (index, pluginList.size()-1);
        pluginList.insert (corrIndex, plugin.release());
        pluginList[index]->prepareToPlay (getSampleRate(), getBlockSize());
        bypasses.insert (corrIndex, false);
    }
}

void PluginEffectsChain::movePlugin (int oldIndex, int newIndex)
{
    pluginList.move (oldIndex, newIndex);
    bypasses.move (oldIndex, newIndex);
}

void PluginEffectsChain::removePlugin (int index)
{
    pluginList[index]->releaseResources();
    pluginList.remove (index);

    bypasses.remove (index);
}

void PluginEffectsChain::toggleBypassPlugin (int index)
{
    auto bypassParameter = pluginList[index]->getBypassParameter();
    if (bypassParameter == nullptr)
        bypasses.set (index, ! bypasses[index]);
    else
        bypassParameter->setValue (! (bool) bypassParameter->getValue());
}

bool PluginEffectsChain::isPluginBypassed (int index)
{
    auto bypassParameter = pluginList[index]->getBypassParameter();
    if (bypassParameter == nullptr)
        return bypasses[index];
    else
        return (bool) bypassParameter->getValue();
}

AudioProcessorEditor* PluginEffectsChain::getPluginEditor (int index)
{
    return pluginList[index]->createEditor();
}
