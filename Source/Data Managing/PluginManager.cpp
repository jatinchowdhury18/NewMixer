#include "PluginManager.h"

#include <unordered_map>

PluginManager::PluginManager()
{
    pluginFormatManager.addDefaultFormats();

    // Set up the known plugin list
    pluginList.addChangeListener (this);
    pluginScan();

    pluginsWindow.reset (new PluginListWindow (pluginFormatManager, pluginList));
}

void PluginManager::pluginScan()
{
    std::unordered_map<String, AudioPluginFormat*> allPlugins;
    for (int i = 0; i < pluginFormatManager.getNumFormats(); i++)
    {

        auto* format = pluginFormatManager.getFormat (i);
        std::cout << format->getName() <<std::endl;

        //Folder for testing plugins
        File testPluginsFolder = File::getCurrentWorkingDirectory();
        while (testPluginsFolder.getFileName() != "NewMixer")
            testPluginsFolder = testPluginsFolder.getParentDirectory();

    #if JUCE_WINDOWS
        testPluginsFolder = testPluginsFolder.getChildFile ("TestPlugins").getChildFile ("Win");    
    #endif
    #if JUCE_MAC
        testPluginsFolder = testPluginsFolder.getChildFile ("TestPlugins").getChildFile ("Mac");
    #endif
        
        std::cout << testPluginsFolder.getFullPathName() << std::endl;
        FileSearchPath defaultSearchpaths = FileSearchPath (testPluginsFolder.getFullPathName());
        StringArray results = pluginFormatManager.getFormat (i)->searchPathsForPlugins (defaultSearchpaths, true);

        for (auto path : results)
            allPlugins.emplace (path, format);
    }

    OwnedArray<PluginDescription> types; //this empty Array is needed as scanAndAddFile requires a reference to one
    int index = 0;
    for (auto path : allPlugins)
    {
        index++;
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

#if JUCE_DEBUG
#include "UnitTests/PluginLoadingTest.h"

static PluginLoadingTest pluginTest;
#endif
