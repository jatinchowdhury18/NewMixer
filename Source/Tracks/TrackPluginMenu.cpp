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

        outlineThick = 5,
    };

    enum PluginActions
    {
        remove = 1,
        bypass,
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
    g.setColour (Colours::red);

    auto b = getBounds().toFloat();

    g.drawLine ((b.getWidth() - lineLength) / 2.0f, b.getHeight() / 2.0f,
                (b.getWidth() + lineLength) / 2.0f, b.getHeight() / 2.0f,
                lineThick);
    
    g.drawLine (b.getWidth() / 2.0f, (b.getHeight() - lineLength) / 2.0f,
                b.getWidth() / 2.0f, (b.getHeight() + lineLength) / 2.0f,
                lineThick);
    
    if (isItemHighlighted())
    {
        g.setColour (Colours::goldenrod);
        g.drawRect (0, 0, getWidth(), getHeight(), outlineThick);
    }
}

void AddPluginComponent::mouseDown (const MouseEvent& /*e*/)
{
    std::unique_ptr<KnownPluginList::PluginTree> pluginTree (KnownPluginList::createTree (PluginManager::getInstanceWithoutCreating()->getPluginList().getTypes(),
                                                                                          KnownPluginList::SortMethod::defaultOrder));
    auto plugins = pluginTree->plugins;

    PopupMenu m;
    for (auto plugin : plugins)
        m.addItem (plugin.uid, plugin.name);
    
    const int result = m.show();
    for (auto plugin : plugins)
    {
        if (plugin.uid == result)
        {
            track->getProcessor()->getPluginChain()->addPlugin (&plugin);
            track->openPluginWindow (track->getProcessor()->getPluginChain()->getNumPlugins()-1);
            PopupMenu::dismissAllActiveMenus();
        }
    }
}

PluginComponent::PluginComponent (Track* track, String name, int index, TrackPluginMenu* parentMenu) :
    PopupMenu::CustomComponent (false),
    track (track),
    name (name),
    index (index),
    parent (parentMenu)
{}

void PluginComponent::getIdealSize (int& idealWidth, int& idealHeight)
{
    idealWidth =  width;
    idealHeight = height;
}

void PluginComponent::paint (Graphics& g)
{
    g.fillAll (Colours::black);

    if (isBypassed())
        g.setColour (Colours::grey);
    else
        g.setColour (Colours::orange);

    g.setFont (Font (18.0f).boldened());
    g.drawFittedText (name, 0, (getHeight() - (int) lineLength) / 2,
                      getWidth(), (int) lineLength, Justification::centred, 1);
    
    if (isItemHighlighted())
    {
        g.setColour (Colours::goldenrod);
        g.drawRect (0, 0, getWidth(), getHeight(), outlineThick);
    }
}

void PluginComponent::mouseDown (const MouseEvent& e)
{
    if (e.mods.isPopupMenu())
    {
        PopupMenu m;
        m.addItem (PluginActions::remove, "Remove");
        m.addItem (PluginActions::bypass, "Bypass", true, isBypassed());

        int result = m.show();

        switch (result)
        {
        case PluginActions::remove:
            track->closePluginWindow();
            track->getProcessor()->getPluginChain()->removePlugin (index);
            triggerMenuItem();
            break;

        case PluginActions::bypass:
            track->getProcessor()->getPluginChain()->toggleBypassPlugin (index);
            break;
        }
    }
}

bool PluginComponent::isBypassed() const
{
    return track->getProcessor()->getPluginChain()->isPluginBypassed (index);
}

void PluginComponent::mouseDrag (const MouseEvent& /*e*/)
{
    isDragging = true;
}

void PluginComponent::mouseUp (const MouseEvent& e)
{
    if (isDragging)
    {
        int newIndex = e.y > 0 ? index + e.y / height : index + (e.y - height) / height;
        newIndex = jmin (jmax (newIndex, 0), track->getProcessor()->getPluginChain()->getNumPlugins()-1);
        track->getProcessor()->getPluginChain()->movePlugin (index, newIndex);
        index = newIndex;

        PopupMenu::dismissAllActiveMenus();
        parent->refresh();
        const auto b = track->getScreenBounds();
        parent->showAt (Rectangle<int> (b.getCentreX(), b.getCentreY(), 0, 0));
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
    refresh();
}

void TrackPluginMenu::refresh()
{
    clear();

    auto pluginChain = track->getProcessor()->getPluginChain();
    for (int i = 0; i < pluginChain->getNumPlugins(); i++)
    {
        addCustomItem (pluginBaseID + i, std::make_unique<PluginComponent> (track, pluginChain->getPluginName (i), i, this));   
    }
    addCustomItem (newPluginID, std::make_unique<AddPluginComponent> (track));
}
