#include "ActionHelper.h"
#include "InputTrackProcessor.h"

void ActionHelper::rightClickMenu (Track* track)
{
    PopupMenu m;

    PopupMenu colorMenu;
    for (int ind = 1; ind <= track->getColours().getNumColours(); ind++)  
        colorMenu.addItem (ind, track->getColours().getColourName (ind - 1), true,
                           track->getColour() == track->getColours().getColour (ind - 1));

    m.addItem (TrackCmds::mute, String ("Mute"), true, track->getProcessor()->getIsMute());
    m.addItem (TrackCmds::solo, String ("Solo"), true, track->isSoloed());
    m.addSubMenu (String ("Change Colour"), colorMenu);
    m.addItem (TrackCmds::rename, String ("Rename"));
    m.addItem (TrackCmds::recordAutomation, String ("Automate"),
               ! (track->getAutoHelper().armed() || track->getAutoHelper().isRecording()));
    
    auto* inputProcessor = dynamic_cast<InputTrackProcessor*> (track->getProcessor());
    if (inputProcessor != nullptr)
    {
        m.addItem (TrackCmds::recordInput, String ("Record"),
                   ! (inputProcessor->isArmed() || inputProcessor->isRecording()));
        m.addItem (TrackCmds::deleteTrack, String ("Delete"));
    }


    m.showMenuAsync (PopupMenu::Options(), ModalCallbackFunction::forComponent (ActionHelper::rightClickCallback, track));
}

void ActionHelper::rightClickCallback (int result, Track* track)
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
        track->getAutoHelper().arm();
        track->repaint();
        return;

    case TrackCmds::recordInput:
        dynamic_cast<InputTrackProcessor*> (track->getProcessor())->arm();
        track->repaint();
        return;

    case TrackCmds::rename:
        track->trackRename();
        return;

    case TrackCmds::deleteTrack:
        track->deleteSelectedTrack();
        return;

    default: //Change Colour
        changeColour (track, result - 1);
    }
}

bool ActionHelper::doKeyPressed (Track* track, const KeyPress& key)
{
    if (! track->getIsSelected())
        return false;

    if (key.getModifiers().isAltDown())                    //Change volume
        ActionHelper::changeSize (track);
    else if (key == KeyPress::createFromDescription ("m"))  //Mute Track
        return track->toggleMute();
    else                                                    // Normal move
        ActionHelper::changePosition (track);

    return true;
}

void ActionHelper::changeSize (Track* track, const MouseEvent& e)
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

void ActionHelper::changeSize (Track* track)
{
    const float initValue = track->getDiameter(); 

    if (KeyPress::isKeyCurrentlyDown (KeyPress::upKey))
        setSizeConstrained (track, initValue, 1.0f); //up
    else if (KeyPress::isKeyCurrentlyDown (KeyPress::downKey))
        setSizeConstrained (track, initValue, -1.0f); //down

    track->resized();
}

void ActionHelper::setSizeConstrained (Track* track, float oldSize, float change)
{
    float newSize = jlimit<float> (TrackConstants::minDiameter, TrackConstants::maxDiameter, (oldSize + change));

    if (newSize == oldSize)
        return;

    track->setDiameter (newSize);
    setPositionConstrained (track, track->getPosition());
}

void ActionHelper::changePosition (Track* track, const MouseEvent& e)
{
    Point<int> newPos = e.getEventRelativeTo (track->getParentComponent()).position.toInt();
    newPos.x -= TrackConstants::width / 2;
    newPos.y -= TrackConstants::width / 2;

    setPositionConstrained (track, newPos);
    track->resized();
}

void ActionHelper::changePosition (Track* track)
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

    setPositionConstrained (track, pos);
    track->resized();
}

void ActionHelper::setPositionConstrained (Track* track, Point<int> pos)
{
    const int halfWidth = TrackConstants::width / 2;
    pos.x = jlimit<int> (-halfWidth, track->getParentWidth() - halfWidth, pos.x);
    pos.y = jlimit<int> (-halfWidth, track->getParentHeight() - halfWidth, pos.y);

    track->setTopLeftPosition (pos);
}

void ActionHelper::changeColour (Track* track, int index)
{ 
    track->setTrackColour (track->getColours().getColour (index));
    track->repaint();
}

//@TODO: Unit tests for constraint functions
