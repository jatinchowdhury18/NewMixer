#include "Track.h"
#include "ActionHelper.h"

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

void Track::mouseDown (const MouseEvent& e)
{
    getParentComponent()->mouseDown (e);

    isSelected = true;
    repaint();

    if (e.mods.isPopupMenu())
        ActionHelper::rightClickMenu (this);
}

void Track::mouseDrag (const MouseEvent& e)
{
    if (e.mods.isAltDown())  //Change volume
        ActionHelper::changeSize (this, e);
    else                      // Normal drag
        ActionHelper::changePosition (this, e);

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
        ActionHelper::changeSize (this);
    else if (key == KeyPress::createFromDescription ("m"))  //Mute Track
        return toggleMute();
    else                                                    // Normal move
        ActionHelper::changePosition (this);

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
