#ifndef TRACKRENAMEWINDOW_H_INCLUDED
#define TRACKRENAMEWINDOW_H_INCLUDED

#include "JuceHeader.h"

class TrackRenameComponent : public Component
{
public:
    TrackRenameComponent (DocumentWindow* win);

    void paint (Graphics& g) override { g.fillAll (Colours::black); }
    void resized() override;
    bool keyPressed (const KeyPress& key) override;

    class Listener
    {
    public:
        virtual ~Listener() {}
        virtual void trackNameChanged (String name, String shortName) {}
    };

    void addListener (Listener* listener) { listeners.add (listener); }
    void removeListener (Listener* listener) { listeners.remove (listener); }
    void setFocused() { nameEditor.grabKeyboardFocus(); }

private:
    void setName();

    ListenerList<Listener> listeners;

    Label nameLabel;
    Label shortLabel;

    TextEditor nameEditor;
    TextEditor shortEditor;

    TextButton setButton;
    
    DocumentWindow* window; 

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TrackRenameComponent)
};

class TrackRenameWindow : public DocumentWindow
{
public:
    TrackRenameWindow (const String name);
    void closeButtonPressed() override { setVisible (false); }
    TrackRenameComponent* getComp() { return comp.get(); }

private:
    std::unique_ptr<TrackRenameComponent> comp;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TrackRenameWindow)
};

#endif //TRACKRENAMEWINDOW_H_INCLUDED
