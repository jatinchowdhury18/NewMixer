#include "ActionHelper.h"
#include "Tracks/TrackHelpers/SoloHelper.h"
#include "TrackHelpers/TrackActionHelper.h"

void ActionHelper::rightClickMenu (MainComponent* mc, const MouseEvent& e)
{
    PopupMenu m;

    m.addItem (Cmds::newRecordTrack, String ("New Track"));

    m.showMenuAsync (PopupMenu::Options(), ModalCallbackFunction::forComponent (rightClickCallback, mc, Point<int> (e.x, e.y)));
}

void ActionHelper::rightClickCallback (int result, MainComponent* mc, Point<int> p)
{
    switch (result)
    {
    case 0: //Nothing selected
        return;

    case Cmds::newRecordTrack:
        ActionHelper::addRecordingTrack (mc, p.x, p.y);
        return;

    default:
        return;
    }
}

bool ActionHelper::doKeyPressed (MainComponent* mc, const KeyPress& key)
{
    if (key == KeyPress::createFromDescription ("s")) //Solo
    {
        SoloHelper::soloButtonPressed (mc->getTracks());
        mc->repaint();
        return true;
    }
    else if (key == KeyPress::spaceKey) //play/pause
    {
        ActionHelper::togglePlay (mc);
        return true;
    }
    else if (key == KeyPress::deleteKey) //delete track
    {
        ActionHelper::deleteSelectedTrack (mc);
        return true;
    }
    else if (key == KeyPress::returnKey) //select track
    {
        ActionHelper::changeSelect (mc, true);
        return true;
    }
    else if (key == KeyPress (KeyPress::returnKey, ModifierKeys::shiftModifier, juce_wchar (NULL)))
    {
        ActionHelper::changeSelect (mc, false);
        return true;
    }
    else if (key == KeyPress::createFromDescription ("CMD + N"))
    {
        ActionHelper::addRecordingTrack (mc, mc->width / 2, mc->height / 2);
        return true;
    }

    for (auto track : mc->getTracks())
    {
        if (track->getIsSelected())
            return TrackActionHelper::doKeyPressed (track, key);
    }

    return false;
}

void ActionHelper::togglePlay (MainComponent* mc)
{
    mc->getMaster()->togglePlay();
    for (auto track : mc->getTracks())
        track->togglePlay();
}

void ActionHelper::deleteSelectedTrack (MainComponent* mc)
{
    Track* trackToDelete = nullptr;
    for (int i = 0; i < mc->getTracks().size(); i++)
        if (mc->getTracks()[i]->getIsSelected())
            trackToDelete = mc->getTracks().removeAndReturn (i);

    if (trackToDelete != nullptr)
        mc->getMaster()->removeTrack (trackToDelete);
    delete trackToDelete;
}

void ActionHelper::clearSelectedTrack (MainComponent* mc)
{
    for (auto track : mc->getTracks())
        track->setSelected (false);
    mc->repaint();
}

void ActionHelper::changeSelect (MainComponent* mc, bool forward)
{
    int trackToSelect = 0;
    for (int i = 0; i < mc->getTracks().size(); i++)
    {
        if (mc->getTracks()[i]->getIsSelected())
        {
            trackToSelect = negativeAwareModulo ((forward ? i + 1 : i - 1), mc->getTracks().size());
            break;
        }
    }

    ActionHelper::clearSelectedTrack (mc);
    mc->getTracks()[trackToSelect]->setSelected (true);
}

void ActionHelper::addRecordingTrack (MainComponent* mc, int x, int y)
{
    auto len =         mc->getTracks().isEmpty() ? (int64) 1000000 : mc->getTracks()[0]->getProcessor()->getLengthSamples();
    auto startSample = mc->getTracks().isEmpty() ? (int64) 0       : mc->getTracks()[0]->getProcessor()->getStartSample();
    auto playing =     mc->getTracks().isEmpty() ? false           : mc->getTracks()[0]->getIsPlaying();

    mc->getTracks().add (new Track (len, startSample, playing, String ("Record 1"), String ("Rec1"),
        x - TrackConstants::width / 2, y - TrackConstants::width / 2, mc->getNextColour()));
    mc->addAndMakeVisible (mc->getTracks().getLast());
    mc->getTracks().getLast()->addListener (mc);

    mc->getMaster()->addTrack (mc->getTracks().getLast());
}
