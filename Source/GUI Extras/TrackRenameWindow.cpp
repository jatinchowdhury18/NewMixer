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
    setWantsKeyboardFocus (true);

    Colour myColour = Colours::lightgrey;

    nameLabel.setText (String ("Name:"), dontSendNotification);
    nameLabel.setColour (Label::textColourId, myColour);
    addAndMakeVisible (nameLabel);

    shortLabel.setText (String ("Abbr:"), dontSendNotification);
    shortLabel.setColour (Label::textColourId, myColour);
    addAndMakeVisible (shortLabel);

    auto setupEditor = [this, myColour] (TextEditor& editor)
    {
        editor.setMultiLine (false);
        editor.setEscapeAndReturnKeysConsumed (false);
        editor.setColour (TextEditor::textColourId, myColour);
        editor.setColour (CaretComponent::caretColourId, myColour);
        editor.setColour (TextEditor::highlightColourId, myColour);
        editor.setColour (TextEditor::highlightedTextColourId, findColour (TextEditor::backgroundColourId));
        editor.setColour (TextEditor::outlineColourId, myColour);
        editor.setColour (TextEditor::focusedOutlineColourId, myColour);
        addAndMakeVisible (editor);
    };

    setupEditor (nameEditor);
    setupEditor (shortEditor);

    setButton.setButtonText (String ("Set"));
    setButton.setColour (TextButton::textColourOffId, myColour);
    setButton.setColour (ComboBox::outlineColourId, myColour);
    setButton.onClick = [this] () { setName(); };
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

bool TrackRenameComponent::keyPressed (const KeyPress& key)
{
    if (key == KeyPress::returnKey)
    {
        setName();
        return true;
    }

    return false;
}

void TrackRenameComponent::setName()
{
    listeners.call (&Listener::trackNameChanged, nameEditor.getText(), shortEditor.getText());
    window->closeButtonPressed();
}
