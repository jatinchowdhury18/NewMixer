#include "Track.h"
#include "TrackProcessor.h"
#include "InputTrackProcessor.h"
#include "TrackHelpers/ActionHelper.h"
#include "TrackHelpers/PaintHelper.h"

using namespace TrackConstants;

Track::Track (File& file, String name, String shortName, int x, int y, Colour colour) : 
    name (name),
    shortName (shortName),
    trackColour (colour)
{
    processor = new TrackProcessor (file);
    processor->addListener (this);

    initialise (x, y);
}

Track::Track (MemoryInputStream* input, String name, String shortName, int x, int y, Colour colour) : 
    name (name),
    shortName (shortName),
    trackColour (colour)
{
    processor = new TrackProcessor (input);
    processor->addListener (this);

    initialise (x, y);
}

Track::Track (int64 sampleLength, int64 startSample, bool playing, String name, String shortName, int x, int y, Colour colour) :
    name (name),
    shortName (shortName),
    trackColour (colour),
    isPlaying (playing)
{
    processor = new InputTrackProcessor (sampleLength, startSample);
    processor->addListener (this);

    initialise (x, y);
}

void Track::initialise (int x, int y)
{
    meter.reset (new TrackMeter (this));
    addAndMakeVisible (meter.get());

    setBounds (x, y, width, width);
    setBroughtToFrontOnMouseClick (true);

    setTooltip (name);
    startTimer (25);
}

Track::~Track()
{
    processor->removeListener (this);
}

void Track::timerCallback()
{
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
    autoHelper.rewind();
    
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
    renameWindow->getComp()->setFocused();
}

void Track::trackNameChanged (String newName, String newShortName)
{
    if (newName.isNotEmpty())
    {
        name = newName;

        if (newShortName.isNotEmpty())
            shortName = newShortName;
        else
            shortName = name.substring (0, jmin<int> (4, name.length()));
    }

    repaint();

    if (renameWindow.get() != nullptr)
        renameWindow->getComp()->removeListener (this);
}

void Track::trackMoved()
{
    const int radius = width / 2;
    processor->trackMoved (getX() + radius,  getY() + radius, (int) diameter, false);
}

void Track::paintOverChildren (Graphics& g) { PaintHelper::paint (this, g); }

void Track::resized()
{
    trackMoved();
    meter->setBounds (0, 0, width, width);
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
    auto input = dynamic_cast<InputTrackProcessor*> (processor);
    if (! isPlaying && input != nullptr)
        input->throwAway();

    if (isPlaying)
        autoHelper.setRecordingStatus();
    else if (autoHelper.isRecording())
        autoHelper.throwAway();
    else
        autoHelper.rewind();
}

#if JUCE_DEBUG
class MoveTest : public UnitTest
{
public:
    MoveTest() : UnitTest ("Move Tracks") {}

    int randInt()
    {
        Random r;
        r.setSeedRandomly();

        return (r.nextInt() % 1000) - 200;
    }

    void checkPosition()
    {
        Point<int> center = Point<int> (track->getX() + width / 2, track->getY() + width / 2);

        expect (0 <= center.x && center.x <= track->getParentWidth(),
                "X coordinate out of range: " + String (center.x));

        expect (0 <= center.y && center.y <= track->getParentHeight(),
            "Y coordinate out of range: " + String (center.x));

        expect (minDiameter <= track->getDiameter() && track->getDiameter() <= maxDiameter,
                "Track diameter out of range: " + String (track->getDiameter()));
    }

    void runTest() override
    {
        beginTest ("Move");
        
        for (int i = 0; i < 5000; i++)
        {
            ActionHelper::setPositionConstrained (track, Point<int> (randInt(), randInt()));
            ActionHelper::setSizeConstrained (track, track->getDiameter(), (float) randInt());

            checkPosition();
        }
    }

    void initialise() override { track = new Track (100, 0, false, "Test", "Tst", 100, 100, Colours::white); }
    void shutdown() override 
    {
        delete track->getProcessor(); 
        delete track;
    }

private:
    Track* track;
};

class NameTest : public UnitTest
{
public:
    NameTest() : UnitTest ("Name Tracks") {}

    void checkNames (String expName, String expShort)
    {
        expect (expName == track->name, "Track name incorrect: " + track->name
                                               + ", expected: " + expName);
        expect (expShort == track->shortName, "Track short name incorrect, got: " + track->shortName
                                                                + ", expected: " + expShort);
    }

    void nameTest (String setName, String setShort, String expName, String expShort)
    {
        track->trackNameChanged (setName, setShort);
        checkNames (expName, expShort);
    }

    void runTest() override
    {
        beginTest ("Names");

        nameTest ("", "", "Test", "Tst");
        nameTest ("", "squaminous", "Test", "Tst");
        nameTest ("Guitar", "Gtr", "Guitar", "Gtr");
        nameTest ("Drums", "", "Drums", "Drum");
        nameTest ("Ttesttttttt", "", "Ttesttttttt", "Ttes");
        nameTest ("Gtr", "", "Gtr", "Gtr");
    }

    void initialise() override { track = new Track (100, 0, false, "Test", "Tst", 100, 100, Colours::white); }
    void shutdown() override 
    {
        delete track->getProcessor(); 
        delete track;
    }

private:
    Track* track;
};

static MoveTest moveTest;
static NameTest nameTest;
#endif
