#include "ActionHelper.h"
#include "Tracks/TrackHelpers/SoloHelper.h"
#include "TrackHelpers/TrackActionHelper.h"
#include "SessionManager.h"

enum
{
    xSpace = MainComponent::width / 9, //stems.size();
    xOffset = (xSpace / 2) - (TrackConstants::width / 2),
    yPos = 350,
};

void ActionHelper::rightClickMenu (MainComponent* mc, const MouseEvent& e)
{
    PopupMenu m;

    m.addItem (Cmds::newFileTrack, String ("New Track [CMD + N]"));
    m.addItem (Cmds::newSession, String ("New Session [CMD + SHIFT + N]"));
    m.addItem (Cmds::openSession, String ("Open Session [CMD + O]"));
    m.addItem (Cmds::saveSession, String ("Save Session [CMD +S]"));
    m.addItem (Cmds::saveSessionAs, String ("Save Session As [CMD + SHIFT + S]"));
    m.addItem (Cmds::exportSession, String ("Export Session [CMD + E]"));
    //m.addItem (Cmds::newRecordTrack, String ("New Recording Track"));

    m.showMenuAsync (PopupMenu::Options(), ModalCallbackFunction::forComponent (rightClickCallback, mc, Point<int> (e.x, e.y)));
}

void ActionHelper::rightClickCallback (int result, MainComponent* mc, Point<int> p)
{
    switch (result)
    {
    case 0: //Nothing selected
        return;

    case Cmds::newSession:
        SessionManager::newSession (mc);
        return;

    case Cmds::openSession:
        SessionManager::openSession (mc);
        return;

    case Cmds::saveSession:
        SessionManager::saveSession (mc);
        return;

    case Cmds::saveSessionAs:
        SessionManager::saveSessionAs (mc);
        return;

    case Cmds::newFileTrack:
        ActionHelper::addFileTrack (mc, p.x, p.y);
        return;

    case Cmds::newRecordTrack:
        ActionHelper::addRecordingTrack (mc, p.x, p.y);
        return;

    case Cmds::exportSession:
        exportSession (mc);
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
    else if (key == KeyPress::createFromDescription ("CMD + SHIFT + N")) //New Session
    {
        SessionManager::newSession (mc);
        return true;
    }
    else if (key == KeyPress::createFromDescription ("CMD + O")) //Open Session
    {
        SessionManager::openSession (mc);
        return true;
    }
    else if (key == KeyPress::createFromDescription ("CMD + S")) //Save Session
    {
        SessionManager::saveSession (mc);
        return true;
    }
    else if (key == KeyPress::createFromDescription ("CMD + SHIFT + S")) //Save session as
    {
        SessionManager::saveSessionAs (mc);
        return true;
    }
    else if (key == KeyPress::createFromDescription ("CMD + L")) //loop mode
    {
        ActionHelper::toggleLoop (mc);
        return true;
    }
    else if (key == KeyPress::createFromDescription ("CMD + E")) //export session
    { 
        exportSession (mc);
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
            mc->getAutoPaths().remove (i);

            mc->getWaveform()->deleteTrack (trackToDelete, i);

            for (int j = i; j < mc->getTracks().size(); j++)
                mc->getTracks()[j]->setIndex (j);
        }
    }

    if (trackToDelete != nullptr)
        mc->getMaster()->removeTrack (trackToDelete);
    delete trackToDelete;

    mc->repaint();
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
        addTrack (new Track (*trackToDuplicate), mc, 
            trackToDuplicate->getX() + TrackConstants::width * 3 / 4,
            trackToDuplicate->getY() + TrackConstants::width * 3 / 4);
}

void ActionHelper::clearSelectedTrack (MainComponent* mc)
{
    for (auto track : mc->getTracks())
        track->setSelected (false);

    for (auto path : mc->getAutoPaths())
        path->setVisible (false);

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
    mc->getAutoPaths()[trackToSelect]->setVisible (true);
}

void ActionHelper::addFileTrack (MainComponent* mc, int x, int y)
{
#if JUCE_ANDROID
    return;
#endif

    FileChooser nativeFileChooser (TRANS ("Import Audio Files"), {}, String ("*.wav"), true);

    if (nativeFileChooser.browseForMultipleFilesToOpen())
    {
        auto files = nativeFileChooser.getResults();
        const auto numFiles = files.size();

        for (int i = 0; i < numFiles; i++)
        {
            auto* newTrack = new Track (files.getReference (i), {}, {}, mc->getNextColour());

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

    mc->getAutoPaths().add (new AutomationPath (track));
    auto autoPath = mc->getAutoPaths().getLast();
    mc->addAndMakeVisible (autoPath);
    autoPath->setVisible (false);
    track->setAutoPath (autoPath);
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

void ActionHelper::loadLocalTracks (MainComponent* mc, String tracksToLoad)
{
    auto setupTrack = [mc] (const void* sourceData, size_t sourceSize, String name, String shortName)
    {
        int xPos = xOffset + xSpace * mc->getTracks().size();
        MemoryInputStream* mis = new MemoryInputStream (sourceData, sourceSize, false);
        addTrack (new Track (mis, name, shortName, mc->getNextColour()), mc, xPos, yPos);
    };

    if (tracksToLoad == "Bridge")
    {
        setupTrack (BinaryData::Bass_wav,  BinaryData::Bass_wavSize,  String ("Bass"),     String ("Bass"));
        setupTrack (BinaryData::Drums_wav, BinaryData::Drums_wavSize, String ("Drums"),    String ("Drum"));
        setupTrack (BinaryData::Gtr1_wav,  BinaryData::Gtr1_wavSize,  String ("Guitar 1"), String ("Gtr1"));
        setupTrack (BinaryData::Gtr2_wav,  BinaryData::Gtr2_wavSize,  String ("Guitar 2"), String ("Gtr2"));
        setupTrack (BinaryData::Organ_wav, BinaryData::Organ_wavSize, String ("Organ"),    String ("Org"));
        setupTrack (BinaryData::Vox1_wav,  BinaryData::Vox1_wavSize,  String ("Vocals 1"), String ("Vox1"));
        setupTrack (BinaryData::Vox2_wav,  BinaryData::Vox2_wavSize,  String ("Vocals 2"), String ("Vox2"));
        setupTrack (BinaryData::Vox3_wav,  BinaryData::Vox3_wavSize,  String ("Vocals 3"), String ("Vox3"));
        setupTrack (BinaryData::Vox4_wav,  BinaryData::Vox4_wavSize,  String ("Vocals 4"), String ("Vox4"));
    }
    else if (tracksToLoad == "Test")
    {
        setupTrack (BinaryData::test_drums_wav, BinaryData::test_drums_wavSize, String ("Drums"), String ("Drum"));
    }
}

void ActionHelper::exportSession (MainComponent* mc)
{
    if (mc->getMaster()->getIsPlaying())
        togglePlay (mc);
    
    rewind (mc);
    
    mc->getExportWindow().reset (new ExportWindow (mc->getMaster(), mc->getSessionLength(), mc->getSessionFile()));
}
