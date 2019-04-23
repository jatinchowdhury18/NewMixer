#include "TrackPluginMenu.h"
#include "Data Managing/PluginManager.h"
#include "Track.h"

namespace
{
    constexpr float lineLength = 40.0f;
    constexpr float lineThick = 5.0f;

    enum
    {
        pluginBaseID = 0x2345,
        newPluginID = 0x1212,
    
        width = 80,
        height = 80,
    };
}

AddPluginComponent::AddPluginComponent (Track* track) :
    PopupMenu::CustomComponent (false),
    track (track)
{}

void AddPluginComponent::getIdealSize (int& idealWidth, int& idealHeight)
{
    idealWidth =  width;
    idealHeight = height;
}

void AddPluginComponent::paint (Graphics& g)
{
    g.fillAll (Colours::black);
    g.setColour (Colours::darkred);

    auto b = getBounds().toFloat();

    g.drawLine ((b.getWidth() - lineLength) / 2.0f, b.getHeight() / 2.0f,
                (b.getWidth() + lineLength) / 2.0f, b.getHeight() / 2.0f,
                lineThick);
    
    g.drawLine (b.getWidth() / 2.0f, (b.getHeight() - lineLength) / 2.0f,
                b.getWidth() / 2.0f, (b.getHeight() + lineLength) / 2.0f,
                lineThick);
    
    // if (isMouseOver)
    // {
    //     g.setColour (Colours::goldenrod);

    //     g.drawRect (10, 10, 60, 60, 2);
    // }
}

void AddPluginComponent::mouseDown (const MouseEvent& /*e*/)
{
    std::unique_ptr<KnownPluginList::PluginTree> pluginTree (PluginManager::getInstanceWithoutCreating()->getPluginList()
                                                            .createTree(KnownPluginList::SortMethod::defaultOrder));
    auto plugins = pluginTree->plugins;

    PopupMenu m;
    for (auto plugin : plugins)
        m.addItem (plugin->uid, plugin->name);
    
    const int result = m.show();
    for (auto plugin : plugins)
    {
        if (plugin->uid == result)
        {
            track->getProcessor()->getPluginChain()->addPlugin (plugin);
            track->openPluginWindow (track->getProcessor()->getPluginChain()->getNumPlugins()-1);
            triggerMenuItem();
        }
    }
}

PluginComponent::PluginComponent (Track* track, String name, int index) :
    PopupMenu::CustomComponent (false),
    track (track),
    name (name),
    index (index)
{}

void PluginComponent::getIdealSize (int& idealWidth, int& idealHeight)
{
    idealWidth =  width;
    idealHeight = height;
}

void PluginComponent::paint (Graphics& g)
{
    g.fillAll (Colours::black);
    g.setColour (Colours::blue);

    g.drawFittedText (name, 0, (getHeight() - (int) lineLength) / 2,
                      getWidth(), (int) lineLength, Justification::centred, 1);
    
    // if (isMouseOver)
    // {
    //     g.setColour (Colours::goldenrod);

    //     g.drawRect (10, 10, 60, 60, 2);
    // }
}

void PluginComponent::mouseDown (const MouseEvent& e)
{
    if (e.mods.isPopupMenu())
    {
        PopupMenu m;
        m.addItem (1, "Remove");

        int result = m.show();

        if (result == 1)
        {
            track->closePluginWindow();
            track->getProcessor()->getPluginChain()->removePlugin (index);
            triggerMenuItem();
        }
    }
}

void PluginComponent::mouseDrag (const MouseEvent& /*e*/)
{
    isDragging = true;
}

void PluginComponent::mouseUp (const MouseEvent& e)
{
    if (isDragging)
    {
    
    }
    else if (! e.mods.isPopupMenu())
    {
        track->openPluginWindow (index);
        triggerMenuItem();
    }

    isDragging = false;
}

TrackPluginMenu::TrackPluginMenu (Track* track) :
    track (track)
{
    auto pluginChain = track->getProcessor()->getPluginChain();

    for (int i = 0; i < pluginChain->getNumPlugins(); i++)
    {
        addCustomItem (pluginBaseID + i, new PluginComponent (track, pluginChain->getPluginName (i), i));   
    }

    addCustomItem (newPluginID, new AddPluginComponent (track));
}
