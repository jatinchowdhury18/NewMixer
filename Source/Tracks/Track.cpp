#include "Track.h"
#include "../Processors/TrackProcessor.h"
#include "../Processors/InputTrackProcessor.h"
#include "ActionHelper.h"
#include "PaintHelper.h"

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
    processor = new InputTrackProcessor (sampleLength, startSample);
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
    
    auto* inputProcessor = dynamic_cast<InputTrackProcessor*> (processor);
    if (inputProcessor != nullptr)
        inputProcessor->setRecordingStatus();

    MessageManagerLock mml;
    repaint();
}

void Track::trackRename()
{
    renameWindow.reset (new TrackRenameWindow (name));
    renameWindow->getComp()->addListener (this);
}

void Track::trackNameChanged (String newName, String newShortName)
{
    if (newName.isNotEmpty())
        name = newName;
    if (newShortName.isNotEmpty())
        shortName = newShortName;

    repaint();
    renameWindow->getComp()->removeListener (this);
}

void Track::trackMoved()
{
    const int radius = width / 2;
    processor->trackMoved (getX() + radius,  getY() + radius, (int) diameter, false);
}

void Track::paint (Graphics& g) { PaintHelper::paint (this, g); }
void Track::resized() { trackMoved(); }

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
