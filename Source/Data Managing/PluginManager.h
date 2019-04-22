#ifndef PLUGINMANAGER_H_INCLUDED
#define PLUGINMANAGER_H_INCLUDED

#include "GUI Extras/PluginListWindow.h"

class PluginManager : private ChangeListener
{
public:
    PluginManager();
    ~PluginManager() { clearSingletonInstance(); }

    void changeListenerCallback (ChangeBroadcaster* /*source*/) override {}

    void showPluginListWindow();
    KnownPluginList& getPluginList() { return pluginList; }
    AudioPluginFormatManager& getPluginFormatManager() { return pluginFormatManager; }

    JUCE_DECLARE_SINGLETON (PluginManager, false)

private:
    void pluginScan();

    AudioPluginFormatManager pluginFormatManager;
    KnownPluginList pluginList;
    std::unique_ptr<PluginListWindow> pluginsWindow;
};

#endif //PLUGINMANAGER_H_INCLUDED
