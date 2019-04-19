#include "PluginManager.h"

PluginManager::PluginManager()
{
    pluginFormatManager.addDefaultFormats();
    //pluginFormatManager.addFormat (new audioworks::AudioWorksSynthsAudioPluginFormat());

    // Set up the known plugin list
    //knownPluginList.setCustomScanner (new audioworks::ChildProcessPluginScanner());
    pluginList.addChangeListener (this);

    // Parse saved known plugins list
    //auto fileName = getSaveFolderPath().getChildFile (DataManagerConstants::knownPluginListFilename);

    //knownPluginFileLock.enterRead();
    //ScopedPointer<XmlElement> element = XmlDocument::parse (fileName);
    //knownPluginFileLock.exitRead();

    //if (element != nullptr && element->hasTagName (DataManagerConstants::knownPluginsId))
    //    knownPluginList.recreateFromXml (*element);

    pluginsWindow.reset (new PluginListWindow (pluginFormatManager, pluginList));
}

void PluginManager::showPluginListWindow()
{
    if (pluginsWindow.get() != nullptr)
    {
        pluginsWindow->setVisible (true);
        pluginsWindow->toFront (true);
    }
}
