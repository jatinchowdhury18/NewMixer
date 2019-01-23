#include "Track.h"

using namespace TrackConstants;

Track::Track (File& file, String name, String shortName, int x, int y, Colour colour) : 
    name (name),
    shortName (shortName),
    trackColour (colour)
{
    processor = new TrackProcessor (file);
    processor->addListener (this);

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
    processor->addListener (this);

    setBounds (x, y, width, width);
    setBroughtToFrontOnMouseClick (true);

    setTooltip (name);
    startTimer (50);
}

Track::Track (int64 sampleLength, int64 startSample, String name, String shortName, int x, int y, Colour colour) :
    name (name),
    shortName (shortName),
    trackColour (colour)
{
    processor = new TrackProcessor (sampleLength, startSample);
    processor->addListener (this);

    setBounds (x, y, width, width);
    setBroughtToFrontOnMouseClick (true);

    setTooltip (name);
    startTimer (50);
}

Track::~Track()
{
    processor->removeListener (this);
}

void Track::timerCallback()
{
    repaint(); 

    if (autoHelper.isRecording())
        autoHelper.addAutoPoint (getX(), getY(), diameter);
    else if (isPlaying && autoHelper.isRecorded())
    {
        int x = getX();
        int y = getY();

        autoHelper.getPoint (x, y, diameter);
        setTopLeftPosition (x, y);

        trackMoved();
    }
}

void Track::newLoop()
{
    autoHelper.setRecordingStatus();
    processor->setRecordingStatus();

    MessageManagerLock mml;
    repaint();
}

void Track::trackMoved()
{
    const int radius = width / 2;
    processor->trackMoved (getX() + radius,  getY() + radius, (int) diameter, false);
}

void Track::paintCircle (Graphics& g, float pos, bool darken)
{
    g.setColour (darken ? trackColour.withAlpha (darkAlpha) : trackColour);

    g.fillEllipse (pos, pos, diameter, diameter);
}

void Track::paintName (Graphics& g, float pos, bool darken)
{
    Colour nameColour = trackColour.contrasting();
    g.setColour (darken ? nameColour.withAlpha (darkAlpha) : nameColour);
    g.setFont (diameter * 0.36f);

    g.drawFittedText (shortName, (int) pos, (int) pos, (int) diameter, (int) diameter, Justification::centred, 1);
}

void Track::paintMeter (Graphics& g, bool  darken)
{
    g.setColour (darken ? trackColour.withAlpha (darkAlpha) : trackColour);

    float rmsFactor = 1.0f + processor->getRMSLevel() * 5.0f;

    if (rmsFactor > 20.0f)
        rmsFactor = 0.0f;

    for (float factor = 1.0f; factor < rmsFactor; factor += 0.1f)
    {
        const float pos = (width - factor * diameter) / 2.0f;
        g.drawEllipse (pos, pos, factor * diameter, factor * diameter, diameter / 40.0f);
    }
}

void Track::paintRing (Graphics& g, float pos, Colour colour)
{
    g.setColour (colour);

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

    if (isPlaying)
        paintMeter (g, darken);

    paintName (g, pos, darken);

    if (autoHelper.armed() || processor->isArmed())
        paintRing (g, pos, Colours::deeppink);
    else if (autoHelper.isRecording() || processor->isRecording())
        paintRing (g, pos, Colours::red);
    else if (isSelected) //highlight selected track
        paintRing (g, pos, Colours::goldenrod);

    if (processor->getIsMute())
        paintMute (g, pos, darken);
}

void Track::resized()
{ 
    trackMoved();
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

    resized();
}

void Track::changeSize()
{
    const float initValue = diameter; 

    if (KeyPress::isKeyCurrentlyDown (KeyPress::upKey))
        setSizeConstrained (initValue, 1.0f); //up
    else if (KeyPress::isKeyCurrentlyDown (KeyPress::downKey))
        setSizeConstrained (initValue, -1.0f); //down

    resized();
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
        m.addItem (TrackCmds::recordAutomation, String ("Automate"), ! (autoHelper.armed() || autoHelper.isRecording()));
        if (processor->isInputTrack())
            m.addItem (TrackCmds::recordInput, String ("Record"), ! (processor->isArmed() || processor->isRecording()));

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

    case TrackCmds::recordAutomation:
        track->autoHelper.arm();
        track->repaint();
        return;

    case TrackCmds::recordInput:
        track->processor->arm();
        track->repaint();
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
    
    trackMoved();
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

    return true;
}

bool Track::toggleMute()
{
    processor->setMute (! processor->getIsMute());
    repaint();
    return true;
}

void Track::togglePlay()
{
    isPlaying = ! isPlaying;

    processor->rewind();

    if (isPlaying)
        autoHelper.setRecordingStatus();
    else
        autoHelper.throwAway();
}

void Track::changeColour (int index)
{ 
    trackColour = colours.getColour (index);
    repaint();
}
