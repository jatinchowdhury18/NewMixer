#include "Track.h"

using namespace TrackConstants;

Track::Track (File& file, String name, String shortName, int x, int y, Colour colour) : 
    name (name),
    shortName (shortName),
    trackColour (colour)
{
    processor = new TrackProcessor (file);

    setBounds (x, y, width, width);
    setBroughtToFrontOnMouseClick (true);

    setTooltip (name);

    startTimer (50);
}

Track::Track (MemoryInputStream* input, String name, String shortName, int x, int y, Colour colour) : 
    name (name),
    shortName (shortName),
    trackColour (colour)
{
    processor = new TrackProcessor (input);

    setBounds (x, y, width, width);
    setBroughtToFrontOnMouseClick (true);

    setTooltip (name);
    startTimer (50);
}

void Track::paintCircle (Graphics& g, float pos, bool darken)
{
    g.setColour (darken ? trackColour.withAlpha (darkAlpha) : trackColour);

    g.fillEllipse (pos, pos, diameter, diameter);
}

void Track::paintName (Graphics& g, float pos, bool darken)
{
    g.setColour (darken ? trackColour.contrasting().withAlpha (darkAlpha) : trackColour.contrasting());
    g.setFont (diameter * 0.36f);

    g.drawFittedText (shortName, (int) pos, (int) pos, (int) diameter, (int) diameter, Justification::centred, 1);
}

void Track::paintMeter (Graphics& g, bool  darken)
{
    g.setColour (darken ? trackColour.withAlpha (darkAlpha) : trackColour);

    float rmsFactor = 1.0f + processor->getRMSLevel() / 4.0f;
    const float pos = (width - rmsFactor * diameter) / 2.0f;
    g.drawEllipse (pos, pos, rmsFactor * diameter, rmsFactor * diameter, diameter / 30.0f);
}

void Track::paintSelected (Graphics& g, float pos)
{
    g.setColour (Colours::goldenrod);

    g.drawEllipse (pos, pos, diameter, diameter, diameter / 20.0f);
}

void Track::paintMute (Graphics& g, float pos, bool darken)
{
    g.setColour (darken ? Colours::goldenrod.withAlpha (darkAlpha) : Colours::goldenrod);

    auto offset = (diameter / 2.0f) * (MathConstants<float>::sqrt2 - 1.0f) / MathConstants<float>::sqrt2;

    g.drawLine (pos + offset, pos + offset, pos + diameter - offset, pos + diameter - offset, diameter / 20.0f);
}

void Track::paint (Graphics& g)
{
    //If a track is soloed, darken all non-soloed tracks
    const bool darken = processor->getSoloed() == TrackProcessor::SoloState::otherTrack;
    const float pos = (width - diameter) / 2.0f;

    paintCircle (g, pos, darken);
    paintMeter (g, darken);
    paintName (g, pos, darken);

    if (isSelected) //highlight selected track
        paintSelected (g, pos);

    if (processor->getIsMute())
        paintMute (g, pos, darken);
}

void Track::resized()
{ 
    const int radius = width / 2;
    processor->trackMoved (getX() + radius,  getY() + radius, width, false);
}

bool Track::hitTest (int x, int y)
{
    bool hit = Component::hitTest (x, y);
    if (! hit)
        return hit;

    //Only return true if mouse is inside circle
    const int xCenter = x - getWidth() / 2;
    const int yCenter = y - getWidth() / 2;
    const int radius = (int) diameter / 2;

    if (xCenter * xCenter + yCenter * yCenter <= radius * radius)
        return true;
    else
        return false;
}

void Track::changeSize (const MouseEvent& e)
{
    e.source.enableUnboundedMouseMovement (true);
    const float initValue = diameter;
    const int curY = e.getDistanceFromDragStartY(); 

    if (curY < lastDragLocation)
        setSizeConstrained (initValue, 1.0f); //up
    else if (curY > lastDragLocation)
        setSizeConstrained (initValue, -1.0f); //down

    isDragging = true;
    lastDragLocation = curY;
}

void Track::changeSize()
{
    const float initValue = diameter; 

    if (KeyPress::isKeyCurrentlyDown (KeyPress::upKey))
        setSizeConstrained (initValue, 1.0f); //up
    else if (KeyPress::isKeyCurrentlyDown (KeyPress::downKey))
        setSizeConstrained (initValue, -1.0f); //down
}

void Track::setSizeConstrained (float oldSize, float change)
{
    float newSize = jlimit<float> (minDiameter, maxDiameter, (oldSize + change));

    if (newSize == oldSize)
        return;

    diameter = newSize;
    setPositionConstrained (getPosition());
}

void Track::changePosition (const MouseEvent& e)
{
    Point<int> newPos = e.getEventRelativeTo (getParentComponent()).position.toInt();
    newPos.x -= width / 2;
    newPos.y -= width / 2;

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
    const int halfWidth = width / 2;
    pos.x = jlimit<int> (-halfWidth, getParentWidth() - halfWidth, pos.x);
    pos.y = jlimit<int> (-halfWidth, getParentHeight() - halfWidth, pos.y);

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
    if (e.mods.isAltDown())  //Change volume
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
    const int radius = width / 2;
    processor->trackMoved (getX() + radius,  getY() + radius, width, true);
}

bool Track::doKeyPressed (const KeyPress& key)
{
    if (! isSelected)
        return false;

    if (key.getModifiers().isAltDown())                    //Change volume
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
