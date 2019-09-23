#ifndef PLUGINWINDOW_H_INCLUDED
#define PLUGINWINDOW_H_INCLUDED

#include "Track.h"

class PluginContainerComponent : public Component
{
public:
    PluginContainerComponent (AudioPluginInstance* plugin);
    ~PluginContainerComponent();

    void resized() override;
    void paint (Graphics& g) override;
    void childBoundsChanged (Component* child) override;

private:
    void setupProgramsBox (AudioPluginInstance* plugin);

    std::unique_ptr<AudioProcessorEditor> editor;

    ComboBox programsBox;
    TextButton bypassButton;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginContainerComponent)
};

class PluginWindow : public DocumentWindow
{
public:
    PluginWindow (AudioPluginInstance* plugin, Track* track);
    
    void closeButtonPressed() override;

private:
    PluginContainerComponent comp;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginWindow)
};

#endif //PLUGINWINDOW_H_INCLUDED
