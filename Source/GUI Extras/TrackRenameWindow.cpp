#include "TrackRenameWindow.h"

enum
{
    yBuffer = 10,
    xBuffer = 10,
    height = 25,
    smallWidth = 50,
    editorWidth = 200,
};

TrackRenameWindow::TrackRenameWindow (const String name) : 
    DocumentWindow (name, Desktop::getInstance().getDefaultLookAndFeel()
    .findColour (ResizableWindow::backgroundColourId),
    DocumentWindow::closeButton)
{
    comp.reset (new TrackRenameComponent (this));

    setUsingNativeTitleBar (true);
    setContentOwned (comp.get(), true);

    setResizable (false, false);
    centreWithSize (getWidth(), getHeight());

    setVisible (true);
}

TrackRenameComponent::TrackRenameComponent (DocumentWindow* win) :
    window (win)
{
    setSize (xBuffer + smallWidth + editorWidth, height * 3 + yBuffer * 4);

    nameLabel.setText (String ("Name:"), dontSendNotification);
    addAndMakeVisible (nameLabel);

    shortLabel.setText (String ("Abbr:"), dontSendNotification);
    addAndMakeVisible (shortLabel);

    nameEditor.setMultiLine (false);
    addAndMakeVisible (nameEditor);

    shortEditor.setMultiLine (false);
    addAndMakeVisible (shortEditor);

    setButton.setButtonText (String ("Set"));
    setButton.onClick = [this] () 
    { 
        listeners.call (&Listener::trackNameChanged, nameEditor.getText(), shortEditor.getText());
        window->closeButtonPressed(); 
    };
    addAndMakeVisible (setButton);
}

void TrackRenameComponent::resized()
{
    nameLabel.setBounds (0, yBuffer, smallWidth, height);
    shortLabel.setBounds (0, height + yBuffer * 2, smallWidth, height);

    nameEditor.setBounds (smallWidth, yBuffer, editorWidth, height);
    shortEditor.setBounds (smallWidth, height + yBuffer * 2, editorWidth, height);

    setButton.setBounds ((getWidth() - smallWidth) / 2, height * 2 + yBuffer * 3, smallWidth, height);
}
