#include "PluginWindow.h"
#include "WindowHelper.h"

enum
{
    pluginX = 400,
    pluginY = 300,

    minTopBarWidth = 300,
    topBarHeight = 50,
    bypassHeight = 25,
    bypassWidth = 75,
};

PluginWindow::PluginWindow (AudioPluginInstance* plugin, Track* track) :
    DocumentWindow (track != nullptr ? track->getName() : "Master",
                    Colours::lightgrey, DocumentWindow::closeButton),
    comp (plugin)
{
    WindowHelper::setupDefaultDocumentWindow (*this, &comp);
    setTopLeftPosition (pluginX, pluginY);
    setResizable (false, false);
    setVisible (true);
    setAlwaysOnTop (true);

    setName (plugin->getPluginDescription().name + "-" + (track != nullptr ? track->getName() : "Master"));
}

void PluginWindow::closeButtonPressed()
{
    setVisible (false);
}

//=======================================================================
PluginContainerComponent::PluginContainerComponent (AudioPluginInstance* plugin) :
    editor (plugin->createEditor())
{
    addAndMakeVisible (editor.get());
    resized();

    auto setupButton = [this] (Button& button, String text = {}, std::function<void()> onClick = {})
    {
        button.setButtonText (text);
        button.setColour (TextButton::buttonColourId, Colours::transparentBlack);
        button.setColour (TextButton::buttonOnColourId, Colours::antiquewhite);
        button.setColour (TextButton::textColourOffId, Colours::antiquewhite);
        button.setColour (TextButton::textColourOnId, Colours::black);
        button.setColour (ComboBox::outlineColourId, Colours::antiquewhite);
        button.setClickingTogglesState (true);
        button.onClick = onClick;
        addAndMakeVisible (button);
    };

    setupButton (bypassButton, "BYPASS", [this, plugin]
        { 
            if (auto* bypassParam = plugin->getBypassParameter())
                bypassParam->setValueNotifyingHost (bypassButton.getToggleState());
        });

    setupProgramsBox (plugin);
}

PluginContainerComponent::~PluginContainerComponent()
{  
}

void PluginContainerComponent::setupProgramsBox (AudioPluginInstance* plugin)
{
    programsBox.setColour (ComboBox::backgroundColourId, Colours::transparentBlack);
    programsBox.setColour (ComboBox::outlineColourId, Colours::antiquewhite);
    programsBox.setColour (ComboBox::textColourId, Colours::antiquewhite);
    programsBox.setColour (ComboBox::arrowColourId, Colours::antiquewhite);
    addAndMakeVisible (programsBox);

    for (int i = 0; i < plugin->getNumPrograms(); ++i)
        programsBox.addItem (plugin->getProgramName (i), i+1);

    programsBox.setSelectedItemIndex (plugin->getCurrentProgram(), dontSendNotification);
    programsBox.onChange = [this, plugin] { plugin->setCurrentProgram (programsBox.getSelectedItemIndex()); };
}

void PluginContainerComponent::childBoundsChanged (Component* child)
{
    if (child == editor.get())
        resized();
}

void PluginContainerComponent::resized()
{
    auto pluginWidth = editor->getWidth();
    auto pluginHeight = editor->getHeight();

    setSize (jmax<int> (pluginWidth, minTopBarWidth), pluginHeight + topBarHeight);
    editor->setBounds (pluginWidth > minTopBarWidth ?  0 :  (minTopBarWidth - pluginWidth) / 2, topBarHeight,
                       pluginWidth, pluginHeight);

    bypassButton.setBounds (5, (topBarHeight - bypassHeight) / 2, bypassWidth, bypassHeight);
    programsBox.setBounds (bypassButton.getRight() + 10, bypassButton.getY(), 2*bypassWidth, bypassHeight);
}

void PluginContainerComponent::paint (Graphics& g)
{
    g.fillAll (Colours::grey);

    g.setColour (Colours::darkgrey.darker());
    g.fillRect (0, 0, getWidth(), topBarHeight);
}
