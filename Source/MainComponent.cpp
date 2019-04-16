#include "MainComponent.h"
#include "TrackHelpers/TrackActionHelper.h"
#include "ActionHelper.h"
#include "SessionManager.h"

//==============================================================================
MainComponent::MainComponent (String mode)
{
    setSize (width, height);
    setWantsKeyboardFocus (true);

    master.reset (new MasterTrackProcessor (tracks));

    waveformView.reset (new WaveformViewer (tracks));
    addAndMakeVisible (waveformView.get());

    if (File (mode.unquoted()).exists())
    {
        const File fileToOpen = File (mode.unquoted());
        if (fileToOpen.hasFileExtension (".chow"))
            SessionManager::openSession (this, &fileToOpen);
    }
    else
        ActionHelper::loadLocalTracks (this, mode); // "Test", or "Bridge"s
    
    resized();

    tooltipWindow.reset (new TooltipWindow (this, tooltipTime));
    initSettings();

#if JUCE_IOS || JUCE_ANDROID
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

#if JUCE_IOS || JUCE_ANDROID
void MainComponent::initPlayButton()
{
    playButton.setButtonText ("Play/Pause");
    playButton.setColour (TextButton::buttonColourId, Colours::transparentBlack);
    playButton.setColour (TextButton::textColourOffId, Colours::darkred);
    playButton.setColour (ComboBox::outlineColourId, Colours::transparentBlack);
    playButton.onClick = [this] () { ActionHelper::togglePlay (this); };
    addAndMakeVisible (playButton);
}
#endif

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

#if JUCE_IOS || JUCE_ANDROID
    playButton.setBounds (getLocalBounds().getX(), getLocalBounds().getY(),
                          (int) (getWidth() * buttonWidthFactor), (int) (getHeight() * buttonHeightFactor));
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

#if JUCE_IOS || JUCE_ANDROID
void MainComponent::mouseDoubleClick (const MouseEvent& e)
{
    ActionHelper::rightClickMenu (this, e);
}
#endif

bool MainComponent::keyPressed (const KeyPress& key)
{
    return ActionHelper::doKeyPressed (this, key);
}

int64 MainComponent::getSessionLength() const
{
    if (tracks.isEmpty())
        return 0;

    return tracks.getFirst()->getProcessor()->getLengthSamples();
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
static SavingTest savingTest;
#endif
