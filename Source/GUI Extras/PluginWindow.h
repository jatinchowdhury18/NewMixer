#ifndef PLUGINWINDOW_H_INCLUDED
#define PLUGINWINDOW_H_INCLUDED

#include "JuceHeader.h"

class PluginWindow : public DocumentWindow
{
public:
    PluginWindow (AudioProcessorEditor* editor);
    
    void closeButtonPressed() override;

private:
    AudioProcessorEditor* editor;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginWindow)
};

#endif //PLUGINWINDOW_H_INCLUDED
