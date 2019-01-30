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
        if (tracks[i]->hitTest (e.x, e.y))
            return;

    if (e.mods.isPopupMenu())
        ActionHelper::rightClickMenu (this, e);
}

#if JUCE_ANDROID
void MainComponent::mouseDoubleClick (const MouseEvent& e)
{
    ActionHelper::rightClickMenu (this, e);
}
#endif

bool MainComponent::keyPressed (const KeyPress& key)
{
    return ActionHelper::doKeyPressed (this, key);
}

//=====================================================
#if JUCE_DEBUG //Unit test declarations
#include "UnitTests/PlayTest.h"
#include "UnitTests/AutomationTest.h"
#include "UnitTests/AddRemoveTracksTest.h"
#include "UnitTests/SavingTest.h"

static PlayTest playTest;
static AutomationTest autoTest;
static AddRemoveTracksTest arTracksTest;
//static SavingTest savingTest;
#endif
