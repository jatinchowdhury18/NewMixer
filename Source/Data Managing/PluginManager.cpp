#include "PluginManager.h"

#include <unordered_map>

JUCE_IMPLEMENT_SINGLETON (PluginManager)

PluginManager::PluginManager()
{
    pluginFormatManager.addDefaultFormats();

    // Set up the known plugin list
    pluginList.addChangeListener (this);
    rescanPlugins();

    pluginsWindow.reset (new PluginListWindow (pluginFormatManager, pluginList));
}

void PluginManager::rescanPlugins()
{
    std::unordered_map<String, AudioPluginFormat*> allPlugins;
    for (int i = 0; i < pluginFormatManager.getNumFormats(); i++)
    {
        auto* format = pluginFormatManager.getFormat (i);

        FileSearchPath defaultSearchpaths = getPluginFolder();        
        StringArray results = pluginFormatManager.getFormat (i)->searchPathsForPlugins (defaultSearchpaths, true);
        for (auto path : results)
            allPlugins.emplace (path, format);
    }

    OwnedArray<PluginDescription> types; //this empty Array is needed as scanAndAddFile requires a reference to one
    for (auto path : allPlugins)
    {
        pluginList.scanAndAddFile (path.first, true, types, *path.second);
    }
}

void PluginManager::showPluginListWindow()
{
    if (pluginsWindow.get() != nullptr)
    {
        pluginsWindow->setVisible (true);
        pluginsWindow->toFront (true);
    }
}

void PluginManager::setPluginFolder (String folder)
{
    File appDataFolder = File::getSpecialLocation (File::userApplicationDataDirectory).getChildFile ("NewMixer");
    File settingsFile = appDataFolder.getChildFile ("Settings.xml");

    if (settingsFile.exists())
    {
        std::unique_ptr<XmlElement> settingsXml (parseXML (settingsFile));
        if (settingsXml.get() != nullptr)
            settingsXml->setAttribute ("pluginsPath", folder);
        
        settingsXml->writeTo (settingsFile, {});
    }
}

FileSearchPath PluginManager::getPluginFolder()
{
# if JUCE_DEBUG
    return getTestPluginFolder();
#else

    File appDataFolder = File::getSpecialLocation (File::userApplicationDataDirectory).getChildFile ("NewMixer");
    if (! appDataFolder.exists())
        appDataFolder.createDirectory();

    File settingsFile = appDataFolder.getChildFile ("Settings.xml");

    if (settingsFile.exists())
    {
        std::unique_ptr<XmlElement> settingsXml (parseXML (settingsFile));
        if (settingsXml.get() != nullptr)
        {
            auto folder = settingsXml->getStringAttribute ("pluginsPath");
            return FileSearchPath (folder);
        }
    }
    
    //Loading path from settings failed
    std::unique_ptr<XmlElement> settingsXml (new XmlElement ("GlobalSettings"));

    FileSearchPath folder;
    for (int i = 0; i < pluginFormatManager.getNumFormats(); i++)
    {
        auto pluginFormat = pluginFormatManager.getFormat (i);
        if (pluginFormat->getName() == "VST" || pluginFormat->getName() == "VST3")
        {
            folder = pluginFormat->getDefaultLocationsToSearch();
            break;
        }
    }
    settingsXml->setAttribute ("pluginsPath", folder.toString());
    settingsXml->writeTo (settingsFile, {});
    return folder;
#endif
}

FileSearchPath PluginManager::getTestPluginFolder()
{
    File testPluginsFolder = File::getCurrentWorkingDirectory();
        while (testPluginsFolder.getFileName() != "NewMixer")
            testPluginsFolder = testPluginsFolder.getParentDirectory();

    #if JUCE_WINDOWS
        testPluginsFolder = testPluginsFolder.getChildFile ("TestPlugins").getChildFile ("Win");    
    #endif
    #if JUCE_MAC
        testPluginsFolder = testPluginsFolder.getChildFile ("TestPlugins").getChildFile ("Mac");
    #endif

    return FileSearchPath (testPluginsFolder.getFullPathName());
}

#if JUCE_DEBUG
#include "UnitTests/PluginLoadingTest.h"

static PluginLoadingTest pluginTest;
#endif
