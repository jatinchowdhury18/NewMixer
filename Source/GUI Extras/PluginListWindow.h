#ifndef PLUGINLISTWINDOW_H_INCLUDED
#define PLUGINLISTWINDOW_H_INCLUDED

#include "JuceHeader.h"

class PluginListWindow : public DocumentWindow
{
public:
    PluginListWindow (AudioPluginFormatManager& formatManager, KnownPluginList& knownPluginList);
    
    void closeButtonPressed() override;

private:
    PluginListComponent* pluginListComponent;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginListWindow)
};

#endif //PLUGINLISTWINDOW_H_INCLUDED
