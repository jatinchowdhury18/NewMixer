#include "Track.h"
#include "TrackProcessor.h"
#include "InputTrackProcessor.h"
#include "TrackHelpers/TrackActionHelper.h"
#include "TrackHelpers/PaintHelper.h"
#include "MainComponent.h"

using namespace TrackConstants;

Track::Track (File& file, String name, String shortName, Colour colour) : 
    name (name),
    shortName (shortName),
    trackColour (colour)
{
    processor = new TrackProcessor (file);
    processor->addListener (this);
}

Track::Track (MemoryInputStream* input, String name, String shortName, Colour colour) : 
    name (name),
    shortName (shortName),
    trackColour (colour)
{
    processor = new TrackProcessor (input);
    processor->addListener (this);
}

Track::Track (int64 sampleLength, int64 startSample, bool playing, String name, String shortName, Colour colour) :
    name (name),
    shortName (shortName),
    trackColour (colour),
    isPlaying (playing)
{
    processor = new InputTrackProcessor (sampleLength, startSample);
    processor->addListener (this);
}

Track::Track (const Track& track) :
    name (track.getName()),
    shortName (track.getShortName()),
    trackColour (track.getColour()),
    isPlaying (track.getIsPlaying()),
    diameter (track.diameter)
{
    auto* inputProcessor = dynamic_cast<InputTrackProcessor*> (track.getProcessor());
    if (inputProcessor == nullptr) //File track
        processor = new TrackProcessor (*dynamic_cast<TrackProcessor*> (track.getProcessor()));
    else //Record track
        processor = new InputTrackProcessor (*inputProcessor);

    processor->addListener (this);
}

void Track::initialise (int x, int y, int ind)
{
    index = ind;

    autoHelper.reset (new AutoHelper);

    meter.reset (new TrackMeter (this));
    addAndMakeVisible (meter.get());

    const auto* parent = getParentComponent();
    if (parent != nullptr)
    {
        relX = (float) x / (float) parent->getWidth();
        relY = (float) y / (float) parent->getHeight();
    }

    setBounds (x, y, width, width);
    setBroughtToFrontOnMouseClick (true);

    setTooltip (name);
    startTimer (AutoHelper::timerInterval);
}

Track::~Track()
{
    processor->removeListener (this);
}

void Track::uninitialise()
{
    delete dynamic_cast<TrackProcessor*> (processor)->getReader();
    dynamic_cast<TrackProcessor*> (processor)->getFormatManager().clearFormats();
    delete processor;
}

void Track::timerCallback()
{
    if (autoHelper->isRecording())
        autoHelper->addAutoPoint (relX, relY, diameter, processor->getStartSample());
    else if (autoHelper->isRecorded())
    {
        if (playheadPos >= 0)
            autoHelper->getPoint (relX, relY, diameter, playheadPos);
        else
            autoHelper->getPoint (relX, relY, diameter, processor->getStartSample());
        
        const auto* parent = getParentComponent();
        if (parent != nullptr)
            setTopLeftPosition (Point<int> ((int) (relX * parent->getWidth()), (int) (relY * parent->getHeight())));

        trackMoved();
    }
}

void Track::newLoop()
{
    autoHelper->setRecordingStatus();
    autoHelper->rewind();
    
    auto* inputProcessor = dynamic_cast<InputTrackProcessor*> (processor);
    if (inputProcessor != nullptr)
        inputProcessor->setRecordingStatus();

    MessageManagerLock mml;
    repaint();
}

void Track::endReached()
{
    autoHelper->setRecordingStatus();

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

    setTooltip (name);
    repaint();

    if (renameWindow.get() != nullptr)
        renameWindow->getComp()->removeListener (this);
}

void Track::trackMoved()
{
    const int radius = width / 2;

    const auto* parent = getParentComponent();
    if (parent != nullptr)
        processor->trackMoved (getX() + radius,  getY() + radius, (int) diameter, parent->getWidth(), parent->getHeight());
}

void Track::paintOverChildren (Graphics& g) { PaintHelper::paint (this, g); }

void Track::resized()
{
    const auto* parent = getParentComponent();
    if (parent != nullptr)
        TrackActionHelper::setPositionConstrained (this,
            Point<int> ((int) (relX * parent->getWidth()), (int) (relY * parent->getHeight())));

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

    dynamic_cast<MainComponent*> (getParentComponent())->getWaveform()->setSelected (index);
    
    if (autoPath != nullptr)
        autoPath->setVisible (true);

    if (e.mods.isPopupMenu())
        TrackActionHelper::rightClickMenu (this);
}

void Track::mouseDrag (const MouseEvent& e)
{
    if (e.mods.isAltDown())  //Change volume
        TrackActionHelper::changeSize (this, e);
    else                      // Normal drag
        TrackActionHelper::changePosition (this, e);

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

    auto input = dynamic_cast<InputTrackProcessor*> (processor);
    if (input != nullptr)
    {
        if (isPlaying)
            input->setRecordingStatus();
        else if (input->isRecording())
            input->throwAway();
    }

    if (isPlaying || autoHelper->isRecording())
        autoHelper->setRecordingStatus();
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
            TrackActionHelper::setPositionConstrained (track, Point<int> (randInt(), randInt()));
            TrackActionHelper::setSizeConstrained (track, track->getDiameter(), (float) randInt());

            checkPosition();
        }
    }

    void initialise() override
    {
        track = new Track (100, 0, false, "Test", "Tst", Colours::white);
        track->initialise (100, 100, 0);
    }

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

    void initialise() override
    {
        track = new Track (100, 0, false, "Test", "Tst", Colours::white);
        track->initialise (100, 100, 0);
    }

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
