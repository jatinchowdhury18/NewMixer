#include "ActionHelper.h"
#include "Tracks/TrackHelpers/SoloHelper.h"
#include "TrackHelpers/TrackActionHelper.h"

void ActionHelper::rightClickMenu (MainComponent* mc, const MouseEvent& e)
{
    PopupMenu m;

    m.addItem (Cmds::newFileTrack, String ("New Track"));
    //m.addItem (Cmds::newRecordTrack, String ("New Recording Track"));

    m.showMenuAsync (PopupMenu::Options(), ModalCallbackFunction::forComponent (rightClickCallback, mc, Point<int> (e.x, e.y)));
}

void ActionHelper::rightClickCallback (int result, MainComponent* mc, Point<int> p)
{
    switch (result)
    {
    case 0: //Nothing selected
        return;

    case Cmds::newFileTrack:
        ActionHelper::addFileTrack (mc, p.x, p.y);
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
        soloSelectedTrack (mc);
        return true;
    }
    else if (key == KeyPress::spaceKey) //play/pause
    {
        ActionHelper::togglePlay (mc);
        return true;
    }
    else if (key == KeyPress::homeKey) //rewind
    {
        ActionHelper::rewind (mc);
        return true;
    }
    else if (key == KeyPress::deleteKey) //delete track
    {
        ActionHelper::deleteSelectedTrack (mc);
        return true;
    }
    else if (key == KeyPress::createFromDescription ("CMD + D"))
    {
        ActionHelper::duplicateSelectedTrack (mc);
        return true;
    }
    else if (key == KeyPress::returnKey) //select track
    {
        ActionHelper::changeSelect (mc, true);
        return true;
    }
    else if (key == KeyPress (KeyPress::returnKey, ModifierKeys::shiftModifier, juce_wchar (NULL))) //reverse select
    {
        ActionHelper::changeSelect (mc, false);
        return true;
    }
    else if (key == KeyPress::createFromDescription ("CMD + N")) //New track
    {
        ActionHelper::addFileTrack (mc, mc->getWidth() / 2, mc->getHeight() / 2);
        //ActionHelper::addRecordingTrack (mc, mc->width / 2, mc->height / 2);
        return true;
    }
    else if (key == KeyPress::createFromDescription ("CMD + L")) //loop mode
    {
        ActionHelper::toggleLoop (mc);
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

void ActionHelper::toggleLoop (MainComponent* mc)
{
    for (auto track : mc->getTracks())
        track->getProcessor()->toggleLoop();
}

void ActionHelper::rewind (MainComponent* mc)
{
    for (auto track : mc->getTracks())
        track->getProcessor()->rewind();
}

void ActionHelper::soloSelectedTrack (MainComponent* mc)
{
    SoloHelper::soloButtonPressed (mc->getTracks());
    mc->repaint();
}

void ActionHelper::deleteSelectedTrack (MainComponent* mc)
{
    Track* trackToDelete = nullptr;
    for (int i = 0; i < mc->getTracks().size(); i++)
    {
        if (mc->getTracks()[i]->getIsSelected())
        {
            trackToDelete = mc->getTracks().removeAndReturn (i);

            mc->getWaveform()->deleteTrack (i);

            for (int j = i; j < mc->getTracks().size(); j++)
                mc->getTracks()[j]->setIndex (j);
        }
    }

    if (trackToDelete != nullptr)
        mc->getMaster()->removeTrack (trackToDelete);
    delete trackToDelete;
}

void ActionHelper::duplicateSelectedTrack (MainComponent* mc)
{
    Track* trackToDuplicate = nullptr;
    for (int i = 0; i < mc->getTracks().size(); i++)
    {
        if (mc->getTracks()[i]->getIsSelected())
        {
            trackToDuplicate = mc->getTracks()[i];
            break;
        }
    }

    if (trackToDuplicate != nullptr)
    {
        mc->getTracks().add (new Track (*trackToDuplicate));
        
        auto* newTrack = mc->getTracks().getLast();

        mc->addAndMakeVisible (newTrack);
        newTrack->addListener (mc);
        newTrack->initialise (trackToDuplicate->getX() + 10, trackToDuplicate->getY() + 10, mc->getTracks().size() - 1);

        mc->getMaster()->addTrack (newTrack);
    }
}

void ActionHelper::clearSelectedTrack (MainComponent* mc)
{
    for (auto track : mc->getTracks())
        track->setSelected (false);
    mc->repaint();
    mc->getWaveform()->setSelected();
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
    mc->getWaveform()->setSelected (trackToSelect);
}

void ActionHelper::addFileTrack (MainComponent* mc, int x, int y)
{
    FileChooser nativeFileChooser (TRANS ("Import Audio Files"), {}, String ("*.wav"), true);

    if (nativeFileChooser.browseForMultipleFilesToOpen())
    {
        auto files = nativeFileChooser.getResults();
        const auto numFiles = files.size();

        for (int i = 0; i < numFiles; i++)
        {
            auto* newTrack = new Track (files.getReference(i), {}, {}, mc->getNextColour());

            //check file is same length as others
            if (mc->getTracks().size() >= 1)
            {
                const auto firstTrack = mc->getTracks().getFirst();
                if (newTrack->getProcessor()->getLengthSamples() != firstTrack->getProcessor()->getLengthSamples())
                {
                    NativeMessageBox::showMessageBoxAsync (AlertWindow::WarningIcon,
                                                           String ("Invalid File"),
                                                           String ("All files must be the same length"),
                                                           mc);
                    
                    delete dynamic_cast<TrackProcessor*> (newTrack->getProcessor())->getReader();
                    dynamic_cast<TrackProcessor*> (newTrack->getProcessor())->getFormatManager().clearFormats();
                    delete newTrack->getProcessor();
                    delete newTrack;
                    return;
                }
            }

            newTrack->trackNameChanged (files[i].getFileNameWithoutExtension());

            const auto thisX = (int) (x - (numFiles * TrackConstants::defaultDiameter / 2)
                + (i * TrackConstants::defaultDiameter) + TrackConstants::defaultDiameter / 2);

            addTrack (newTrack, mc, thisX, y);
            
        }
    }
}

void ActionHelper::addRecordingTrack (MainComponent* mc, int x, int y)
{
    auto len =         mc->getTracks().isEmpty() ? (int64) 1000000 : mc->getTracks()[0]->getProcessor()->getLengthSamples();
    auto startSample = mc->getTracks().isEmpty() ? (int64) 0       : mc->getTracks()[0]->getProcessor()->getStartSample();
    auto playing =     mc->getTracks().isEmpty() ? false           : mc->getTracks()[0]->getIsPlaying();

    auto* newTrack = new Track (len, startSample, playing, String ("Record 1"), String ("Rec1"), mc->getNextColour());

    addTrack (newTrack, mc, x, y);
}

void ActionHelper::addTrack (Track* track, MainComponent* mc, int x, int y)
{
    mc->getTracks().add (track);

    mc->addAndMakeVisible (track);
    track->addListener (mc);

    track->initialise (x - TrackConstants::width / 2, y - TrackConstants::width / 2, mc->getTracks().size() - 1);

    mc->getMaster()->addTrack (track);
    mc->getWaveform()->addTrack (track);
}

bool ActionHelper::validTrackFile (Track* firstTrack, Track* newTrack, MainComponent* mc)
{
    if (newTrack->getProcessor()->getLengthSamples() == firstTrack->getProcessor()->getLengthSamples())
        return true;

    NativeMessageBox::showMessageBoxAsync (AlertWindow::WarningIcon,
                                           String ("Invalid File"),
                                           String ("All files must be the same length"),
                                           mc);

    newTrack->uninitialise();
    delete newTrack;
    return false;
}
