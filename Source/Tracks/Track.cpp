#include "Track.h"
#include "TrackProcessor.h"
#include "InputTrackProcessor.h"
#include "TrackActionHelper.h"
#include "TrackHelpers/PaintHelper.h"
#include "MainComponent.h"
#include "TrackPluginMenu.h"
#include "GUI Extras/PluginWindow.h"

using namespace TrackConstants;

namespace
{
    const Identifier trackType = Identifier ("Track");
    const Identifier trackName = Identifier ("Name");
    const Identifier trackShortName = Identifier ("ShortName");
    const Identifier trackColour = Identifier ("Colour");
    const Identifier trackX = Identifier ("X");
    const Identifier trackY = Identifier ("Y");
    const Identifier trackDiameter = Identifier ("Diameter");

    enum
    {
        uuidSize = 6,
    };
}

Track::Track (File& file, String name, String shortName, Colour colour, String uuid) : 
    trackValueTree (trackType),
    uuid (uuid == "" ? Uuid().toString().substring (0, uuidSize) : uuid)
{
    setupValueTree (name, shortName, colour);

    autoHelper.reset (new AutoHelper (this));

    initProcessor.reset (new TrackProcessor (file));
    processor = initProcessor.get();
    processor->addListener (this);
}

Track::Track (MemoryInputStream* input, String name, String shortName, Colour colour, String uuid) : 
    trackValueTree (trackType),
    uuid (uuid == "" ? Uuid().toString().substring (0, uuidSize) : uuid)
{
    setupValueTree (name, shortName, colour);

    autoHelper.reset (new AutoHelper (this));

    initProcessor.reset (new TrackProcessor (input));
    processor = initProcessor.get();
    processor->addListener (this);
}

Track::Track (int64 sampleLength, int64 startSample, bool playing, String name, String shortName, Colour colour, String uuid) :
    trackValueTree (trackType),
    uuid (uuid == "" ? Uuid().toString().substring (0, uuidSize) : uuid),
    isPlaying (playing)
{
    setupValueTree (name, shortName, colour);

    autoHelper.reset (new AutoHelper (this));

    initProcessor.reset (new InputTrackProcessor (sampleLength, startSample));
    processor = initProcessor.get();
    processor->addListener (this);
}

Track::Track (const Track& track, String uuid) :
    uuid (uuid == "" ? Uuid().toString().substring (0, uuidSize) : uuid),
    diameter (track.diameter),
    isPlaying (track.getIsPlaying()),
    trackValueTree (track.trackValueTree.createCopy())
{
    autoHelper.reset (new AutoHelper (this));
    autoHelper->setRecorded (track.autoHelper->isRecorded());
    autoHelper->getPoints() = track.autoHelper->getPoints().createCopy();

    auto* inputProcessor = dynamic_cast<InputTrackProcessor*> (track.getProcessor());
    if (inputProcessor == nullptr) //File track
        initProcessor.reset (new TrackProcessor (*dynamic_cast<TrackProcessor*> (track.getProcessor())));
    else //Record track
        initProcessor.reset (new InputTrackProcessor (*inputProcessor));

    processor = initProcessor.get();
    processor->addListener (this);

    if (! track.getProcessor()->getIsMute())
        toggleMute();
}

void Track::setupValueTree (String name, String shortName, Colour colour)
{
    setName (name);
    setShortName (shortName);
    setTrackColour (colour);
}

void Track::initialise (int x, int y, int ind)
{
    index = ind;

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

    setTooltip (getName());
    startTimer (AutoHelper::timerInterval);
}

Track::~Track()
{
    TrackColours::deleteInstance();
    processor->removeListener (this);
    
#if JUCE_IOS || JUCE_ANDROID
    touches.clear();
#endif
}

void Track::timerCallback()
{
    if (autoHelper->isRecording())
        autoHelper->recordAutoPoint (relX, relY, diameter, processor->getStartSample());
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

UndoManager* Track::getUndoManager() const noexcept
{
    auto mc = dynamic_cast<MainComponent*> (getParentComponent());
    if (mc == nullptr)
    {
        // The main component should always exist, except for some unit tests
        return nullptr; 
    }

    return &mc->getUndoManager();
}

void Track::renderAutomationExport()
{
    if (! autoHelper->isRecorded())
        return;

    if (playheadPos >= 0)
        autoHelper->getPoint (relX, relY, diameter, playheadPos);
    else
        autoHelper->getPoint (relX, relY, diameter, processor->getStartSample());

    const auto* parent = getParentComponent();
    if (parent != nullptr)
        processor->trackMoved ((int) (relX * parent->getWidth()) + width/2,
                               (int) (relY * parent->getHeight()) + width/2,
                               (int) diameter, parent->getWidth(), parent->getHeight());
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

Colour Track::getColour() const noexcept
{
    return Colour ((uint32) (int) trackValueTree.getProperty (trackColour));
}

void Track::setTrackColour (Colour newColour)
{
    trackValueTree.setProperty (trackColour, (int) newColour.getARGB(), getUndoManager());
}

String Track::getName() const noexcept
{
    return trackValueTree.getProperty (trackName).toString();
}

void Track::setName (String name)
{
    trackValueTree.setProperty (trackName, name, getUndoManager());
}

String Track::getShortName() const noexcept
{
    return trackValueTree.getProperty (trackShortName).toString();
}

void Track::setShortName (String shortName)
{
    trackValueTree.setProperty (trackShortName, shortName, getUndoManager());
}

void Track::trackRename()
{
    getUndoManager()->beginNewTransaction();
    renameWindow.reset (new TrackRenameWindow (getName()));
    renameWindow->getComp()->addListener (this);
    renameWindow->getComp()->setFocused();
}

void Track::trackNameChanged (String newName, String newShortName)
{
    if (newName.isNotEmpty())
    {
        setName (newName);

        if (newShortName.isNotEmpty())
            setShortName (newShortName);
        else
            setShortName (getName().substring (0, jmin<int> (4, getName().length())));
    }

    setTooltip (getName());
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

void Track::paintOverChildren (Graphics& g)
{ 
    PaintHelper::paint (this, g);

    if (pluginWindow.get() != nullptr && pluginWindow->isVisible())
        pluginWindow->repaint();
}

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

void Track::openPluginWindow (int pluginIndex)
{
    pluginWindow.reset (new PluginWindow (processor->getPluginChain()->getPluginList()[pluginIndex], this));
}

void Track::closePluginWindow()
{
    pluginWindow.reset (nullptr);
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
    
#if JUCE_IOS || JUCE_ANDROID
    touches.add (e);
#endif
}

#if JUCE_IOS || JUCE_ANDROID
void Track::mouseDoubleClick (const MouseEvent& e)
{
    TrackActionHelper::rightClickMenu (this);
}
#else
void Track::mouseDoubleClick (const MouseEvent& /*e*/)
{
    pluginMenu.reset (new TrackPluginMenu (this));

    const auto b = getScreenBounds();
    pluginMenu->showAt (Rectangle<int> (b.getCentreX(), b.getCentreY(), 0, 0));
}
#endif

void Track::mouseDrag (const MouseEvent& e)
{
#if JUCE_IOS || JUCE_ANDROID
    if (touches.size() > 1)
    {
        TrackActionHelper::changeSize (this, e);
    }
    else                      // Normal drag
        TrackActionHelper::changePosition (this, e);
#else
    if (e.mods.isAltDown())   //Change volume
        TrackActionHelper::changeSize (this, e);
    else                      // Normal drag
        TrackActionHelper::changePosition (this, e);
#endif
    
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
    
#if JUCE_IOS || JUCE_ANDROID
    touches.clear();
#endif
}

void Track::toggleMute()
{
    processor->setMute (! processor->getIsMute());
    repaint();
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

String Track::getFilePath() const
{
    const auto proc = dynamic_cast<TrackProcessor*> (processor);
    if (proc == nullptr)
        return {};

    return proc->getFile().getFullPathName();
}

#if JUCE_DEBUG
#include "UnitTests/TrackNameTest.h"
#include "UnitTests/TrackMoveTest.h"
#include "UnitTests/PluginProcessingTest.h"

static MoveTest moveTest;
static NameTest nameTest;
static PluginProcessingTest pluginTest;
#endif
