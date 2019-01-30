#include "MainComponent.h"
#include "TrackHelpers/TrackActionHelper.h"
#include "ActionHelper.h"

//==============================================================================
enum
{
    xSpace = MainComponent::width / 9, //stems.size();
    xOffset = (xSpace / 2) - (TrackConstants::width / 2),
    yPos = 350,
};

MainComponent::MainComponent (String mode)
{
    setSize (width, height);
    setWantsKeyboardFocus (true);

    addTracks (mode); //"" (default), "Test", or "Bridge"

    master.reset (new MasterTrack (tracks));

    waveformView.reset (new WaveformViewer (tracks));
    addAndMakeVisible (waveformView.get());
    resized();

    tooltipWindow.reset (new TooltipWindow (this, tooltipTime));
    initSettings();

#if JUCE_ANDROID
    initPlayButton();
#endif
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

void MainComponent::initPlayButton()
{
    playButton.setButtonText ("Play/Pause");
    playButton.setColour (TextButton::buttonColourId, Colours::transparentBlack);
    playButton.setColour (TextButton::textColourOffId, Colours::darkred);
    playButton.setColour (ComboBox::outlineColourId, Colours::transparentBlack);
    playButton.onClick = [this] () { ActionHelper::togglePlay (this); };
    addAndMakeVisible (playButton);
}

void MainComponent::addTracks (String stemsToUse)
{
    if (stemsToUse == "Bridge")
        bridgeTracks();
    else if (stemsToUse == "Test")
        testTracks();

    for (auto* track : tracks)
    {
        addAndMakeVisible (track);
        track->addListener (this);
        track->resized();

        autoPaths.add (new AutomationPath (track));
        addAndMakeVisible (autoPaths.getLast());
        autoPaths.getLast()->toBehind (tracks[0]);
    }
}

void MainComponent::setupTrack (const void* sourceData, size_t sourceSize, String name, String shortName)
{
    int xPos = xOffset + xSpace * tracks.size();
    MemoryInputStream* mis = new MemoryInputStream (sourceData, sourceSize, false);
    tracks.add (new Track (mis, name, shortName, getNextColour()));
    addAndMakeVisible (tracks.getLast());
    tracks.getLast()->addListener (this);
    tracks.getLast()->initialise (xPos, yPos, tracks.size() - 1);
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
    setupTrack (BinaryData::Vox4_wav, BinaryData::Vox4_wavSize, String ("Vocals 4"), String ("Vox4"));
}

void MainComponent::testTracks()
{
    setupTrack (BinaryData::test_drums_wav, BinaryData::test_drums_wavSize, String ("Drums"), String ("Drum"));
}

void MainComponent::deleteSelectedTrack() { ActionHelper::deleteSelectedTrack (this); }
void MainComponent::duplicateSelectedTrack() { ActionHelper::duplicateSelectedTrack (this); }
void MainComponent::soloSelectedTrack() { ActionHelper::soloSelectedTrack (this); }

//==============================================================================
void MainComponent::paint (Graphics& g)
{
    g.fillAll (Colours::black);

    g.setFont (Font (40.0f));
    g.setColour (Colours::darkred);
    g.drawText ("New Mixer", getLocalBounds().removeFromTop (30), Justification::centred, true);

    g.setColour (Colours::white);
    const auto yLine = (float) getHeight() * MainConstants::heightFactor;
    g.drawLine (0.0f, yLine, (float) getWidth(), yLine);
    
    for (auto* autoPath : autoPaths)
        autoPath->repaint();
}

void MainComponent::resized()
{
    using namespace MainConstants;
    settingsButton.setBounds ((int) (getWidth() * (1 - buttonWidthFactor)), 0,
                              (int) (getWidth() * buttonWidthFactor), (int) (getHeight() * buttonHeightFactor));

    if (waveformView != nullptr)
        waveformView->setBounds (0, (int) (getHeight() * MainConstants::heightFactor),
                                 getWidth(), (int) (getHeight() * (1.0f - MainConstants::heightFactor)));

    for (auto track : tracks)
        track->resized();

#if JUCE_ANDROID
    playButton.setBounds (getLocalBounds().getX(), getLocalBounds().getY(), buttonWidth, buttonHeight);
#endif
}

void MainComponent::mouseDown (const MouseEvent& e)
{
    ActionHelper::clearSelectedTrack (this);

    for (int i = 0; i < tracks.size(); i++)
    {
        if (tracks[i]->hitTest (e.x, e.y))
        {
            autoPaths[i]->setVisible (true);
            return;
        }
    }

    if (e.mods.isPopupMenu())
        ActionHelper::rightClickMenu (this, e);
}

bool MainComponent::keyPressed (const KeyPress& key)
{
    return ActionHelper::doKeyPressed (this, key);
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

        ActionHelper::togglePlay (&main);
        playState = ! playState;
    }

    void runTest() override
    {
        beginTest ("Play");
        MainComponent main;
        bool playState = false;
        
        for (int i = 0; i < numTestTracks; i++)
        {
            ActionHelper::addRecordingTrack (&main, 0, 0);
            playTests (main, playState);
        } 
    }
};
//@TODO: Fix automation tests
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

    float randFloat()
    {
        Random r;
        r.setSeedRandomly();

        return (r.nextFloat());
    }

    void setAutoPoints (OwnedArray<Track>& tracks, Array<float>* x, Array<float>* y, Array<float>* diameter)
    {
        beginTest ("Setting test automation points");
        for (int i = 0; i < 5000; i++)
        {
            for (int t = 0; t < numTestTracks; t++)
            {
                x[t].add (randFloat());
                y[t].add (randFloat());
                diameter[t].add ((float) randInt());
                tracks[t]->getAutoHelper()->addAutoPoint (x[t].getLast(), y[t].getLast(), diameter[t].getLast(), i);
            }
        }
    }

    void checkAutoPoints (OwnedArray<Track>& tracks, Array<float>* x, Array<float>* y, Array<float>* diameter)
    {
        beginTest ("Checking test automation points");
        for (int i = 0; i < 5000; i++)
        {
            for (int t = 0; t < numTestTracks; t++)
            {
                float xTest = 0;
                float yTest = 0;
                float dTest = 0;
                tracks[t]->getAutoHelper()->getPoint (xTest, yTest, dTest, i);

                expect (xTest == x[t][i], "Track x position incorrect: " + String (xTest));
                expect (yTest == y[t][i], "Track y position incorrect: " + String (yTest));
                expect (dTest == diameter[t][i], "Track diameter incorrect: " + String (dTest));
            }
        }
    }

    void runTest() override
    {
        MainComponent main;

        Array<float> x[numTestTracks];
        Array<float> y[numTestTracks];
        Array<float> diameter[numTestTracks];

        for (int i = 0; i < numTestTracks; i++)
            ActionHelper::addRecordingTrack (&main, 0, 0);
        
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
            ActionHelper::addRecordingTrack (&main, 0, 0);

        ActionHelper::togglePlay (&main);

        beginTest ("Removing tracks");
        for (int i = numTestTracks - 1; i >= 0; i--)
        {
            main.tracks[i]->setSelected (true);
            main.tracks[i]->deleteSelectedTrack();
            ActionHelper::togglePlay (&main);
        }

        main.deleteSelectedTrack();
    }
};

static PlayTest playTest;
static AutomationTest autoTest;
static AddRemoveTracksTest arTracksTest;
#endif
