#include "Track.h"

Track::Track (File& file, String name, int x, int y, Colour colour) : 
    name (name),
    trackColour (colour)
{
    processor = new TrackProcessor (file);

    setBounds (x, y, defaultWidth, defaultWidth);
    setBroughtToFrontOnMouseClick (true);

    setTooltip (name);
}

void Track::paint (Graphics& g)
{
    auto diameter = (float) getWidth();

    if (processor->getIsMute() || processor->getSoloed() == TrackProcessor::SoloState::otherTrack)
        g.setColour (trackColour.withAlpha (0.5f)); // Make colour faded if track is muted
    else
        g.setColour (trackColour); //normal colour

    g.fillEllipse (0.f, 0.f, diameter, diameter);

    if (isSelected) //highlight selected track
    {
        g.setColour (Colours::goldenrod);
        g.drawEllipse (0.0f, 0.0f, diameter, diameter, 2.5f);
    }

    if (processor->getSoloed() == TrackProcessor::SoloState::thisTrack)
    {
        g.setColour (Colours::goldenrod);
        g.setFont (diameter);
        g.drawFittedText (String ("S"), 0, 0, (int) diameter, (int) diameter, Justification::centred, 1);
    }
}

void Track::resized()
{ 
    const int radius = getWidth() / 2;
    processor->trackMoved (getX() + radius,  getY() + radius, getWidth());
}

void Track::changeSize (const MouseEvent& e)
{
    e.source.enableUnboundedMouseMovement (true);
    const int initValue = getWidth();
    int changeVal = 0;
    const int curY = e.getDistanceFromDragStartY();
    const int stepSize = 1;  

    if (curY < lastDragLocation)
        changeVal = stepSize; //up
    else if (curY > lastDragLocation)
        changeVal = -stepSize; //down

    setSizeConstrained (initValue, changeVal);

    isDragging = true;
    lastDragLocation = curY;
}

void Track::changeSize()
{
    const int initValue = getWidth();
    int changeVal = 0;
    const int stepSize = 1;  

    if (KeyPress::isKeyCurrentlyDown (KeyPress::upKey))
        changeVal = stepSize; //up
    else if (KeyPress::isKeyCurrentlyDown (KeyPress::downKey))
        changeVal = -stepSize; //down

    setSizeConstrained (initValue, changeVal);
}

void Track::setSizeConstrained (int oldSize, int change)
{
    int newSize = jlimit<int> (minWidth, maxWidth, oldSize + change);

    if (newSize == oldSize)
        return;

    setSize (newSize, newSize);
    setPositionConstrained (getPosition());
}

void Track::changePosition (const MouseEvent& e)
{
    Point<int> newPos = e.getEventRelativeTo (getParentComponent()).position.toInt();
    newPos.x -= getWidth() / 2;
    newPos.y -= getWidth() / 2;

    setPositionConstrained (newPos);
    resized();
}

void Track::changePosition()
{
    Point<int> pos = getBoundsInParent().getTopLeft();

    const int changeVal = 10;

    if (KeyPress::isKeyCurrentlyDown (KeyPress::upKey))
        pos.y -= changeVal; //up
    if (KeyPress::isKeyCurrentlyDown (KeyPress::downKey))
        pos.y += changeVal; //down
    if (KeyPress::isKeyCurrentlyDown (KeyPress::leftKey))
        pos.x -= changeVal; //left
    if (KeyPress::isKeyCurrentlyDown (KeyPress::rightKey))
        pos.x += changeVal; //right

    setPositionConstrained (pos);
    resized();
}

void Track::setPositionConstrained (Point<int> pos)
{
    const int radius = getWidth() / 2;
    pos.x = jlimit<int> (-radius, getParentWidth() - radius, pos.x);
    pos.y = jlimit<int> (-radius, getParentHeight() - radius, pos.y);

    setTopLeftPosition (pos);
}

void Track::mouseDown (const MouseEvent& e)
{
    getParentComponent()->mouseDown (e);

    isSelected = true;
    repaint();

    if (e.mods.isPopupMenu())
    {
        PopupMenu m;

        PopupMenu colorMenu;
        for (int ind = 1; ind <= colours.getNumColours(); ind++)  
            colorMenu.addItem (ind, colours.getColourName (ind - 1), true, trackColour == colours.getColour (ind - 1));

        m.addItem (TrackCmds::mute, String ("Mute"), true, processor->getIsMute());
        m.addItem (TrackCmds::solo, String ("Solo"), true, isSoloed());
        m.addSubMenu (String ("Change Colour"), colorMenu);

        m.showMenuAsync (PopupMenu::Options(), ModalCallbackFunction::forComponent (rightClickCallback, this));
    }
}

void Track::rightClickCallback (int result, Track* track)
{
    switch (result)
    {
    case 0: //Nothing selected
        return;

    case TrackCmds::mute:
        track->toggleMute();
        return;

    case TrackCmds::solo:
        track->getParentComponent()->keyPressed (KeyPress::createFromDescription ("s"));
        return;

    default: //Change Colour
        track->changeColour (result - 1);
    }
}

void Track::mouseDrag (const MouseEvent& e)
{
    if (e.mods.isCtrlDown())  //Change volume
        changeSize (e);
    else                      // Normal drag
        changePosition (e);

    getParentComponent()->repaint();
}

void Track::mouseUp (const MouseEvent& /*e*/)
{
    if (isDragging)
    {
        isDragging = false;
        lastDragLocation = 0;
    }
}

bool Track::doKeyPressed (const KeyPress& key)
{
    if (! isSelected)
        return false;

    if (key.getModifiers().isCtrlDown())                    //Change volume
        changeSize();
    else if (key == KeyPress::createFromDescription ("m"))  //Mute Track
        return toggleMute();
    else                                                    // Normal move
        changePosition();

    getParentComponent()->repaint();
    return true;
}

bool Track::toggleMute()
{
    processor->setMute (! processor->getIsMute());
    getParentComponent()->repaint();
    return true;
}

void Track::changeColour (int index)
{ 
    trackColour = colours.getColour (index);
    repaint();
}
