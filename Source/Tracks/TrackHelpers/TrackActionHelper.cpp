#include "TrackActionHelper.h"
#include "InputTrackProcessor.h"
#include "MainComponent.h"

void TrackActionHelper::rightClickMenu (Track* track)
{
    PopupMenu m;

    PopupMenu colorMenu;
    for (int ind = 1; ind <= track->getColours().getNumColours(); ind++)  
        colorMenu.addItem (ind, track->getColours().getColourName (ind - 1), true,
                           track->getColour() == track->getColours().getColour (ind - 1));

    PopupMenu recordMenu;
    recordMenu.addItem (NumLoops::One, "1 Loop");
    recordMenu.addItem (NumLoops::Two, "2 Loops");
    recordMenu.addItem (NumLoops::Three, "3 Loops");
    recordMenu.addItem (NumLoops::Four, "4 Loops");
    recordMenu.addItem (NumLoops::Free, "Free");

    m.addItem (TrackCmds::mute, String ("Mute [m]"), true, track->getProcessor()->getIsMute());
    m.addItem (TrackCmds::solo, String ("Solo [s]"), true, track->isSoloed());
    m.addSubMenu (String ("Change Colour"), colorMenu);
    m.addItem (TrackCmds::rename, String ("Rename [CMD + R]"));
    m.addItem (TrackCmds::recordAutomation, String ("Automate [a]"));
    m.addItem (TrackCmds::deleteAutomation, String ("Delete Automation [SHIFT + del]"),
               (track->getAutoHelper()->isRecorded() || track->getAutoHelper()->isRecording()));
    
    auto* inputProcessor = dynamic_cast<InputTrackProcessor*> (track->getProcessor());
    if (inputProcessor != nullptr)
        m.addSubMenu (String ("Record [r]"), recordMenu);
        
    m.addItem (TrackCmds::duplicateTrack, String ("Duplicate [CMD + D]"));
    m.addItem (TrackCmds::deleteTrack, String ("Delete [del]"));

    m.showMenuAsync (PopupMenu::Options(), ModalCallbackFunction::forComponent (TrackActionHelper::rightClickCallback, track));
}

void TrackActionHelper::rightClickCallback (int result, Track* track)
{
    switch (result)
    {
    case 0: //Nothing selected
        return;

    case TrackCmds::mute:
        track->toggleMute();
        return;

    case TrackCmds::solo:
        track->soloSelectedTrack();
        return;

    case TrackCmds::recordAutomation:
        track->getAutoHelper()->arm();
        track->repaint();
        return;

    case TrackCmds::deleteAutomation:
        track->getAutoHelper()->throwAway();
        track->repaint();
        track->getParentComponent()->repaint();
        return;

    case NumLoops::One:
    case NumLoops::Two:
    case NumLoops::Three:
    case NumLoops::Four:
    case NumLoops::Free:
        dynamic_cast<InputTrackProcessor*> (track->getProcessor())->arm (NumLoops (result));
        track->repaint();
        return;

    case TrackCmds::rename:
        track->trackRename();
        return;

    case TrackCmds::deleteTrack:
        track->deleteSelectedTrack();
        return;

    case TrackCmds::duplicateTrack:
        track->duplicateSelectedTrack();
        return;

    default: //Change Colour
        changeColour (track, result - 1);
    }
}

bool TrackActionHelper::doKeyPressed (Track* track, const KeyPress& key)
{
    if (! track->getIsSelected())
        return false;

    if (key.getModifiers().isAltDown())                           //Change volume
        TrackActionHelper::changeSize (track);
    else if (key == KeyPress::createFromDescription ("m"))        //Mute Track
        return track->toggleMute();
    else if (key == KeyPress::createFromDescription ("a"))        //Automate
    {
        track->getAutoHelper()->arm();
        track->repaint();
    }
    else if (key == KeyPress::createFromDescription ("SHIFT + DELETE")) //Delete automation
    {
        if (track->getAutoHelper()->isRecorded() || track->getAutoHelper()->isRecording())
        {
            track->getAutoHelper()->throwAway();
            track->repaint();
        }
    }
    else if (key == KeyPress::createFromDescription ("r"))        //Record
    {
        auto* inputProcessor = dynamic_cast<InputTrackProcessor*> (track->getProcessor());
        if (inputProcessor != nullptr)
            inputProcessor->arm (NumLoops::One, true);
        track->repaint();
    }
    else if (key == KeyPress::createFromDescription ("SHIFT + R"))        //Record
    {
        auto* inputProcessor = dynamic_cast<InputTrackProcessor*> (track->getProcessor());
        if (inputProcessor != nullptr)
            inputProcessor->arm (NumLoops::Free, true);
        track->repaint();
    }
    else if (key == KeyPress::createFromDescription ("CMD + R"))
        track->trackRename();
    else                                                          // Normal move
        TrackActionHelper::changePosition (track);

    return true;
}

void TrackActionHelper::changeSize (Track* track, const MouseEvent& e)
{
    e.source.enableUnboundedMouseMovement (true);
    const float initValue = track->getDiameter();
    const int curY = e.getDistanceFromDragStartY();
    auto& lastDragLocation = track->getLastDrag();

    if (curY < lastDragLocation)
        setSizeConstrained (track, initValue, 1.0f); //up
    else if (curY > lastDragLocation)
        setSizeConstrained (track, initValue, -1.0f); //down

    track->setDragging (true);
    lastDragLocation = curY;

    track->resized();
}

void TrackActionHelper::changeSize (Track* track)
{
    const float initValue = track->getDiameter(); 

    if (KeyPress::isKeyCurrentlyDown (KeyPress::upKey))
        setSizeConstrained (track, initValue, 1.0f); //up
    else if (KeyPress::isKeyCurrentlyDown (KeyPress::downKey))
        setSizeConstrained (track, initValue, -1.0f); //down

    track->resized();
}

void TrackActionHelper::setSizeConstrained (Track* track, float oldSize, float change)
{
    float newSize = jlimit<float> (TrackConstants::minDiameter, TrackConstants::maxDiameter, (oldSize + change));

    if (newSize == oldSize)
        return;

    track->setDiameter (newSize);

    setPositionConstrained (track, track->getPosition());
    setRelPosition (track, track->getPosition());
}

void TrackActionHelper::changePosition (Track* track, const MouseEvent& e)
{
    Point<int> newPos = e.getEventRelativeTo (track->getParentComponent()).position.toInt();
    newPos.x -= TrackConstants::width / 2;
    newPos.y -= TrackConstants::width / 2;

    setPositionConstrained (track, newPos);
    setRelPosition (track, newPos);
    track->resized();
}

void TrackActionHelper::changePosition (Track* track)
{
    Point<int> pos = track->getBoundsInParent().getTopLeft();
    const int changeVal = 10;

    if (KeyPress::isKeyCurrentlyDown (KeyPress::upKey))
        pos.y -= changeVal; //up
    if (KeyPress::isKeyCurrentlyDown (KeyPress::downKey))
        pos.y += changeVal; //down
    if (KeyPress::isKeyCurrentlyDown (KeyPress::leftKey))
        pos.x -= changeVal; //left
    if (KeyPress::isKeyCurrentlyDown (KeyPress::rightKey))
        pos.x += changeVal; //right

    setRelPosition (track, pos);
    setPositionConstrained (track, pos);
    track->trackMoved();
}

void TrackActionHelper::setRelPosition (Track* track, Point<int> pos)
{
    const auto* parent = track->getParentComponent();
    if (parent != nullptr)
        track->setRelativePosition ((float) pos.x / parent->getWidth(), (float) pos.y / parent->getHeight());
}

void TrackActionHelper::setPositionConstrained (Track* track, Point<int> pos)
{
    const int halfWidth = TrackConstants::width / 2;
    pos.x = jlimit<int> (-halfWidth, track->getParentWidth() - halfWidth, pos.x);
    pos.y = jlimit<int> (-halfWidth, (int) (track->getParentHeight() * MainConstants::heightFactor) - halfWidth, pos.y);

    track->setTopLeftPosition (pos);
}

void TrackActionHelper::changeColour (Track* track, int index)
{ 
    track->setTrackColour (track->getColours().getColour (index));
    track->repaint();
}

//@TODO: Unit tests for constraint functions
