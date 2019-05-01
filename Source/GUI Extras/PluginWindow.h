#ifndef PLUGINWINDOW_H_INCLUDED
#define PLUGINWINDOW_H_INCLUDED

#include "Track.h"

class PluginWindow : public DocumentWindow
{
public:
    PluginWindow (AudioPluginInstance* plugin, Track* track);
    
    void closeButtonPressed() override;

private:
    AudioProcessorEditor* editor;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginWindow)
};

#endif //PLUGINWINDOW_H_INCLUDED
