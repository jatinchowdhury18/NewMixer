#include "MainComponent.h"
#include "TrackHelpers/SoloHelper.h"
#include "TrackHelpers/ActionHelper.h"

//==============================================================================
enum
{
    xSpace = MainComponent::width / 9, //stems.size();
    xOffset = (xSpace / 2) - (TrackConstants::width / 2),
    yPos = 475,
};

MainComponent::MainComponent (String mode)
{
    addTracks (mode); //"Test" (default), "Chorus", or "Bridge"

    master.reset (new MasterTrack (tracks));

    setSize (width, height);
    setWantsKeyboardFocus (true);

    tooltipWindow.reset (new TooltipWindow (this, tooltipTime));
    initSettings();
}

MainComponent::~MainComponent()
{
    for (auto track : tracks)
        track->removeListener (this);
}

void MainComponent::initSettings()
{
    settingsButton.setButtonText ("Settings");
    settingsButton.setColour (TextButton::buttonColourId, Colours::transparentBlack);
    settingsButton.setColour (TextButton::textColourOffId, Colours::darkred);
    settingsButton.setColour (ComboBox::outlineColourId, Colours::transparentBlack);
    settingsButton.onClick = [this] () { settingsWindow.reset (new SettingsWindow (String ("Settings"), master->getDeviceManager())); };
    addAndMakeVisible (settingsButton);
}

void MainComponent::addTracks (String stemsToUse)
{
    //@TODO: Figure out a more dynamic way to load files from Stems folder
    /*
    const File rootDir = File::getCurrentWorkingDirectory().getParentDirectory().getParentDirectory();
    const File stemsFolder = File (rootDir.getFullPathName() + "\\Stems\\" + stemsToUse);

    Array<File> stems = stemsFolder.findChildFiles (File::TypesOfFileToFind::findFiles, false);
    const int xOffset = width / stems.size();
    for (auto file : stems)
    {
        int n = tracks.size();
        int xPos = (xOffset / 2) - (Track::defaultWidth / 2) + xOffset * n;
        tracks.add (new Track (file, file.getFileNameWithoutExtension(), xPos, 500, trackColours.getColour (n)));
    }
    */

    if (stemsToUse == "Bridge")
        bridgeTracks();
    else if (stemsToUse == "Chorus")
        chorusTracks();
    else if (stemsToUse == "Test")
        testTracks();

    for (auto* track : tracks)
    {
        addAndMakeVisible (track);
        track->addListener (this);
    }
}

void MainComponent::setupTrack (const void* sourceData, size_t sourceSize, String name, String shortName)
{
    int xPos = xOffset + xSpace * tracks.size();
    MemoryInputStream* mis = new MemoryInputStream (sourceData, sourceSize, false);
    tracks.add (new Track (mis, name, shortName, xPos, yPos, trackColours.getColour (tracks.size())));
}

void MainComponent::bridgeTracks()
{
    setupTrack (BinaryData::Bass_wav, BinaryData::Bass_wavSize, String ("Bass"), String ("Bass"));
    setupTrack (BinaryData::Drums_wav, BinaryData::Drums_wavSize, String ("Drums"), String ("Drum"));
    setupTrack (BinaryData::Gtr1_wav, BinaryData::Gtr1_wavSize, String ("Guitar 1"), String ("Gtr1"));
    setupTrack (BinaryData::Gtr2_wav, BinaryData::Gtr2_wavSize, String ("Guitar 2"), String ("Gtr2"));
    setupTrack (BinaryData::Organ_wav, BinaryData::Organ_wavSize, String ("Organ"), String ("Org"));
    setupTrack (BinaryData::Vox1_wav, BinaryData::Vox1_wavSize, String ("Vocals 1"), String ("Vox1"));
    setupTrack (BinaryData::Vox2_wav, BinaryData::Vox2_wavSize, String ("Vocals 2"), String ("Vox2"));
    setupTrack (BinaryData::Vox3_wav, BinaryData::Vox3_wavSize, String ("Vocals 3"), String ("Vox3"));
}

void MainComponent::chorusTracks()
{
    setupTrack (BinaryData::Chorus_Bass_wav, BinaryData::Chorus_Bass_wavSize, String ("Bass"), String ("Bass"));
    setupTrack (BinaryData::Chorus_Drums_wav, BinaryData::Chorus_Drums_wavSize, String ("Drums"), String ("Drum"));
    setupTrack (BinaryData::Chorus_Gtr1_wav, BinaryData::Chorus_Gtr1_wavSize, String ("Guitar 1"), String ("Gtr1"));
    setupTrack (BinaryData::Chorus_Gtr2_wav, BinaryData::Chorus_Gtr2_wavSize, String ("Guitar 2"), String ("Gtr2"));
    setupTrack (BinaryData::Chorus_Vox1_wav, BinaryData::Chorus_Vox1_wavSize, String ("Vocals 1"), String ("Vox1"));
    setupTrack (BinaryData::Chorus_Vox2_wav, BinaryData::Chorus_Vox2_wavSize, String ("Vocals 2"), String ("Vox2"));
    setupTrack (BinaryData::Chorus_Vox3_wav, BinaryData::Chorus_Vox3_wavSize, String ("Vocals 3"), String ("Vox3"));
}

void MainComponent::testTracks()
{
    setupTrack (BinaryData::test_drums_wav, BinaryData::test_drums_wavSize, String ("Drums"), String ("Drum"));
}

void MainComponent::addRecordingTrack (int x, int y)
{
    auto len = tracks[0]->getProcessor()->getLengthSamples();
    auto startSample = tracks[0]->getProcessor()->getStartSample();
    auto playing = tracks[0]->getIsPlaying();

    tracks.add (new Track (len, startSample, playing, String ("Record 1"), String ("Rec1"),
                x - TrackConstants::width / 2, y - TrackConstants::width / 2, trackColours.getColour (tracks.size())));
    addAndMakeVisible (tracks.getLast());
    tracks.getLast()->addListener (this);

    master->addTrack (tracks.getLast());
}

void MainComponent::deleteSelectedTrack()
{
    Track* trackToDelete = nullptr;
    for (int i = 0; i < tracks.size(); i++)
        if (tracks[i]->getIsSelected())
            trackToDelete = tracks.removeAndReturn (i);

    if (trackToDelete != nullptr)
        master->removeTrack (trackToDelete);
    delete trackToDelete;
}

//==============================================================================
void MainComponent::paint (Graphics& g)
{
    g.fillAll (Colours::black);

    g.setFont (Font (40.0f));
    g.setColour (Colours::darkred);
    g.drawText ("New Mixer", getLocalBounds().removeFromTop (30), Justification::centred, true);
}

void MainComponent::resized()
{
    settingsButton.setBounds (width - buttonWidth, 0, buttonWidth, buttonHeight);
}

void MainComponent::mouseDown (const MouseEvent& e)
{
    clearSelectedTrack();

    for (auto track : tracks)
        if (track->hitTest (e.x, e.y))
            return;

    if (e.mods.isPopupMenu())
    {
        PopupMenu m;

        m.addItem (Cmds::newRecordTrack, String ("New Track"));

        m.showMenuAsync (PopupMenu::Options(), ModalCallbackFunction::forComponent (rightClickCallback, this, Point<int> (e.x, e.y)));
    }
}

void MainComponent::clearSelectedTrack()
{
    for (auto track : tracks)
        track->setSelected (false);
    repaint();
}

void MainComponent::rightClickCallback (int result, MainComponent* mc, Point<int> p)
{
    switch (result)
    {
    case 0: //Nothing selected
        return;

    case Cmds::newRecordTrack:
        mc->addRecordingTrack (p.x, p.y);
        return;

    default:
        return;
    }
}

bool MainComponent::keyPressed (const KeyPress& key)
{
    if (key == KeyPress::createFromDescription ("s")) //Solo
    {
        SoloHelper::soloButtonPressed (tracks);
        repaint();
        return true;
    }
    else if (key == KeyPress::spaceKey) //play/pause
    {
        togglePlay();
        return true;
    }
    else if (key == KeyPress::deleteKey) //delete track
    {
        deleteSelectedTrack();
        return true;
    }
    else if (key == KeyPress::returnKey) //select track
    {
        changeSelect (true);
        return true;
    }
    else if (key == KeyPress (KeyPress::returnKey, ModifierKeys::shiftModifier, juce_wchar (NULL)))
    {
        changeSelect (false);
        return true;
    }

    for (auto track : tracks)
    {
        if (track->getIsSelected())
            return ActionHelper::doKeyPressed (track, key);
    }

    return false;
}

void MainComponent::togglePlay()
{
    master->togglePlay();
    for (auto track : tracks)
        track->togglePlay();
}

void MainComponent::changeSelect (bool forward)
{
    int trackToSelect = 0;
    for (int i = 0; i < tracks.size(); i++)
    {
        if (tracks[i]->getIsSelected())
        {
            trackToSelect = negativeAwareModulo ((forward ? i + 1 : i - 1), tracks.size());
            break;
        }
    }

    clearSelectedTrack();
    tracks[trackToSelect]->setSelected (true);
}

//=====================================================
#if JUCE_DEBUG
enum
{
    numTestTracks = 15,
};

class PlayTest : public UnitTest
{
public:
    PlayTest() : UnitTest ("Play Test") {}

    void checkPlaying (Track* track,  bool shouldBePlaying)
    {
        expect (track->getIsPlaying() == shouldBePlaying,
                "Track playstate incorrect: " + String ((int) track->getIsPlaying()));
    }

    void checkPlayheads (Track* track0, Track* track1)
    {
        expect (track0->getProcessor()->getStartSample() == track1->getProcessor()->getStartSample(),
                "Track playheads misaligned: " + String (track0->getProcessor()->getStartSample())
                                        + ", " + String (track1->getProcessor()->getStartSample()));
    }

    void playTests(MainComponent& main, bool& playState)
    {
        for (auto track : main.tracks)
            checkPlaying (track, playState);

        for (int i = 1; i < main.tracks.size(); i++)
            checkPlayheads (main.tracks[i - 1], main.tracks[i]);

        main.togglePlay();
        playState = ! playState;
    }

    void runTest() override
    {
        beginTest ("Play");
        MainComponent main;
        bool playState = false;
        
        for (int i = 0; i < numTestTracks; i++)
        {
            main.addRecordingTrack (0, 0);
            playTests (main, playState);
        } 
    }
};

class AutomationTest : public UnitTest
{
public:
    AutomationTest() : UnitTest ("Automation") {}

    int randInt()
    {
        Random r;
        r.setSeedRandomly();

        return (r.nextInt() % 1000) - 200;
    }

    void setAutoPoints (OwnedArray<Track>& tracks, Array<int>* x, Array<int>* y, Array<float>* diameter)
    {
        beginTest ("Setting test automation points");
        for (int i = 0; i < 5000; i++)
        {
            for (int t = 0; t < numTestTracks; t++)
            {
                x[t].add (randInt());
                y[t].add (randInt());
                diameter[t].add ((float) randInt());
                tracks[t]->getAutoHelper().addAutoPoint (x[t].getLast(), y[t].getLast(), diameter[t].getLast());
            }
        }
    }

    void checkAutoPoints (OwnedArray<Track>& tracks, Array<int>* x, Array<int>* y, Array<float>* diameter)
    {
        beginTest ("Checking test automation points");
        for (int i = 0; i < 5000; i++)
        {
            for (int t = 0; t < numTestTracks; t++)
            {
                int xTest = 0;
                int yTest = 0;
                float dTest = 0;
                tracks[t]->getAutoHelper().getPoint (xTest, yTest, dTest);

                expect (xTest == x[t][i], "Track x position incorrect: " + String (xTest));
                expect (yTest == y[t][i], "Track y position incorrect: " + String (yTest));
                expect (dTest == diameter[t][i], "Track diameter incorrect: " + String (dTest));
            }
        }
    }

    void runTest() override
    {
        MainComponent main;

        Array<int> x[numTestTracks];
        Array<int> y[numTestTracks];
        Array<float> diameter[numTestTracks];

        for (int i = 0; i < numTestTracks; i++)
            main.addRecordingTrack (0, 0);
        
        setAutoPoints (main.tracks, x, y, diameter);
        checkAutoPoints (main.tracks, x, y, diameter);
    }
};

class AddRemoveTracksTest : public UnitTest
{
public:
    AddRemoveTracksTest() : UnitTest ("Track adding and removing") {}

    void runTest() override
    {
        MainComponent main;

        beginTest ("Adding Tracks");

        for (int i = 0; i < numTestTracks; i++)
            main.addRecordingTrack (0, 0);

        main.togglePlay();

        beginTest ("Removing tracks");
        for (int i = numTestTracks - 1; i >= 0; i--)
        {
            main.tracks[i]->setSelected (true);
            main.tracks[i]->deleteSelectedTrack();
            main.togglePlay();
        }

        main.deleteSelectedTrack();
    }
};

static PlayTest playTest;
static AutomationTest autoTest;
static AddRemoveTracksTest arTracksTest;
#endif
