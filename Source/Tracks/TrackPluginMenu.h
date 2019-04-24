#ifndef TRACKPLUGINMENU_H_INCLUDED
#define TRACKPLUGINMENU_H_INCLUDED

#include "Track.h"

class AddPluginComponent : public PopupMenu::CustomComponent
{
public:
    AddPluginComponent (Track* track);

    void getIdealSize (int& idealWidth, int& idealHeight) override;
    void paint (Graphics& g) override;

    // void mouseEnter (const MouseEvent& /*e*/) override { isMouseOver = true; }
    // void mouseExit (const MouseEvent& /*e*/) override { isMouseOver = false; }

    void mouseDown (const MouseEvent& e) override;

private:
    Track* track;
    // bool isMouseOver = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AddPluginComponent)
};

class PluginComponent : public PopupMenu::CustomComponent
{
public:
    PluginComponent (Track* track, String name, int index, TrackPluginMenu* parentMenu);

    void getIdealSize (int& idealWidth, int& idealHeight) override;
    void paint (Graphics& g) override;

    void mouseDown (const MouseEvent& e) override;
    void mouseDrag (const MouseEvent& e) override;
    void mouseUp (const MouseEvent& e) override;

private:
    bool isBypassed() const;

    Track* track;
    String name;
    int index;
    
    TrackPluginMenu* parent;

    bool isDragging = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginComponent)
};

class TrackPluginMenu : public PopupMenu
{
public:
    TrackPluginMenu (Track* track);
    void refresh();

private:
    Track* track;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TrackPluginMenu)
};

#endif //TRACKPLUGINMENU_H_INCLUDED
