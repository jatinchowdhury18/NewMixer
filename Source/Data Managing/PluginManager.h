#ifndef PLUGINMANAGER_H_INCLUDED
#define PLUGINMANAGER_H_INCLUDED

#include "GUI Extras/PluginListWindow.h"

class PluginManager : private ChangeListener
{
public:
    PluginManager();

    void changeListenerCallback (ChangeBroadcaster* /*source*/) override {}

    void showPluginListWindow();

private:

    AudioPluginFormatManager pluginFormatManager;
    KnownPluginList pluginList;
    std::unique_ptr<PluginListWindow> pluginsWindow;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginManager)
};

#endif //PLUGINMANAGER_H_INCLUDED
