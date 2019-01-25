#include "MainComponent.h"
#include "TrackHelpers/SoloHelper.h"
#include "TrackHelpers/ActionHelper.h"

//==============================================================================
MainComponent::MainComponent()
{
    addTracks ("Test"); //"Test", "Chorus", or "Bridge"

    master.reset (new MasterTrack (tracks));

    setSize (width, height);
    setWantsKeyboardFocus (true);

    tooltipWindow.reset (new TooltipWindow (this, tooltipTime));
    initSettings();
}

MainComponent::~MainComponent()
{
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
    const int xSpace = width / 9; //stems.size();
    const int xOffset = (xSpace / 2) - (TrackConstants::width / 2);
    const int yPos = 475;
    
    if (stemsToUse == "Bridge")
    {
        int xPos = xOffset + xSpace * tracks.size();
        MemoryInputStream* bass = new MemoryInputStream (BinaryData::Bass_wav, BinaryData::Bass_wavSize, false);
        tracks.add (new Track (bass, String ("Bass"), String ("Bass"), xPos, yPos, trackColours.getColour (tracks.size())));

        xPos = xOffset + xSpace * tracks.size();
        MemoryInputStream* drums = new MemoryInputStream (BinaryData::Drums_wav, BinaryData::Drums_wavSize, false);
        tracks.add (new Track (drums, String ("Drums"), String ("Drum"), xPos, yPos, trackColours.getColour (tracks.size())));

        xPos = xOffset + xSpace * tracks.size();
        MemoryInputStream* gtr1 = new MemoryInputStream (BinaryData::Gtr1_wav, BinaryData::Gtr1_wavSize, false);
        tracks.add (new Track (gtr1, String ("Gtr1"), String ("Gtr1"), xPos, yPos, trackColours.getColour (tracks.size())));

        xPos = xOffset + xSpace * tracks.size();
        MemoryInputStream* gtr2 = new MemoryInputStream (BinaryData::Gtr2_wav, BinaryData::Gtr2_wavSize, false);
        tracks.add (new Track (gtr2, String ("Gtr2"), String ("Gtr2"), xPos, yPos, trackColours.getColour (tracks.size())));

        xPos = xOffset + xSpace * tracks.size();
        MemoryInputStream* organ = new MemoryInputStream (BinaryData::Organ_wav, BinaryData::Organ_wavSize, false);
        tracks.add (new Track (organ, String ("Organ"), String ("Org"), xPos, yPos, trackColours.getColour (tracks.size())));

        xPos = xOffset + xSpace * tracks.size();
        MemoryInputStream* vox1 = new MemoryInputStream (BinaryData::Vox1_wav, BinaryData::Vox1_wavSize, false);
        tracks.add (new Track (vox1, String ("Vox1"), String ("Vox1"), xPos, yPos, trackColours.getColour (tracks.size())));

        xPos = xOffset + xSpace * tracks.size();
        MemoryInputStream* vox2 = new MemoryInputStream (BinaryData::Vox2_wav, BinaryData::Vox2_wavSize, false);
        tracks.add (new Track (vox2, String ("Vox2"), String ("Vox2"), xPos, yPos, trackColours.getColour (tracks.size())));

        xPos = xOffset + xSpace * tracks.size();
        MemoryInputStream* vox3 = new MemoryInputStream (BinaryData::Vox3_wav, BinaryData::Vox3_wavSize, false);
        tracks.add (new Track (vox3, String ("Vox3"), String ("Vox3"), xPos, yPos, trackColours.getColour (tracks.size())));

        xPos = xOffset + xSpace * tracks.size();
        MemoryInputStream* vox4 = new MemoryInputStream (BinaryData::Vox4_wav, BinaryData::Vox4_wavSize, false);
        tracks.add (new Track (vox4, String ("Vox4"), String ("Vox4"), xPos, yPos, trackColours.getColour (tracks.size())));
    }
    else if (stemsToUse == "Test")
    {
        int xPos = xOffset + xSpace * tracks.size();
        MemoryInputStream* drums = new MemoryInputStream (BinaryData::test_drums_wav, BinaryData::test_drums_wavSize, false);
        tracks.add (new Track (drums, String ("Drums"), String ("Drum"), xPos, yPos, trackColours.getColour (tracks.size())));
    }

    for (auto* track : tracks)
        addAndMakeVisible (track);
}

void MainComponent::addRecordingTrack()
{
    auto len = tracks[0]->getProcessor()->getLengthSamples();
    auto startSample = tracks[0]->getProcessor()->getStartSample();
    auto playing = tracks[0]->getIsPlaying();

    tracks.add (new Track (len, startSample, playing, String ("Record 1"), String ("Rec1"), width / 2, 400, trackColours.getColour (tracks.size())));
    addAndMakeVisible (tracks.getLast());

    master->addTrack (tracks.getLast());
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

        m.showMenuAsync (PopupMenu::Options(), ModalCallbackFunction::forComponent (rightClickCallback, this));
    }
}

void MainComponent::clearSelectedTrack()
{
    for (auto track : tracks)
        track->setSelected (false);
    repaint();
}

void MainComponent::rightClickCallback (int result, MainComponent* mc)
{
    switch (result)
    {
    case 0: //Nothing selected
        return;

    case Cmds::newRecordTrack:
        mc->addRecordingTrack();
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

    if (key == KeyPress::spaceKey) //play/pause
        togglePlay();

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

//=====================================================
#if JUCE_DEBUG

class AutomationTest : public UnitTest
{
public:
    AutomationTest() : UnitTest ("Automation") {}

    void dummyTest (int val1, int val2)
    {
        expectEquals<int> (val1, val2, "Dummy Error");
    }

    void runTest() override
    {
        beginTest ("Automation");

        dummyTest (1, 1);
    }
};

static AutomationTest autoTest;
#endif
