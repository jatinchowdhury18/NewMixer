#include "MainComponent.h"
#include "Tracks/SoloHelper.h"

//==============================================================================
MainComponent::MainComponent()
{
    addTracks ("Bridge"); //"Test", "Chorus", or "Bridge"

    master.reset (new MasterTrack (tracks));

    setSize (width, height);
    setWantsKeyboardFocus (true);

    tooltipWindow.reset (new TooltipWindow (this, tooltipTime));
}

MainComponent::~MainComponent()
{
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
    const int xOffset = width / 9; //stems.size();

    int xPos = (xOffset / 2) - (Track::defaultWidth / 2) + xOffset * tracks.size();
    MemoryInputStream* bass = new MemoryInputStream (BinaryData::Bass_wav, BinaryData::Bass_wavSize, false);
    tracks.add (new Track (bass, String ("Bass"), xPos, 500, trackColours.getColour (tracks.size())));

    xPos = (xOffset / 2) - (Track::defaultWidth / 2) + xOffset * tracks.size();
    MemoryInputStream* drums = new MemoryInputStream (BinaryData::Drums_wav, BinaryData::Drums_wavSize, false);
    tracks.add (new Track (drums, String ("Drums"), xPos, 500, trackColours.getColour (tracks.size())));

    xPos = (xOffset / 2) - (Track::defaultWidth / 2) + xOffset * tracks.size();
    MemoryInputStream* gtr1 = new MemoryInputStream (BinaryData::Gtr1_wav, BinaryData::Gtr1_wavSize, false);
    tracks.add (new Track (gtr1, String ("Gtr1"), xPos, 500, trackColours.getColour (tracks.size())));

    xPos = (xOffset / 2) - (Track::defaultWidth / 2) + xOffset * tracks.size();
    MemoryInputStream* gtr2 = new MemoryInputStream (BinaryData::Gtr2_wav, BinaryData::Gtr2_wavSize, false);
    tracks.add (new Track (gtr2, String ("Gtr2"), xPos, 500, trackColours.getColour (tracks.size())));

    xPos = (xOffset / 2) - (Track::defaultWidth / 2) + xOffset * tracks.size();
    MemoryInputStream* organ = new MemoryInputStream (BinaryData::Organ_wav, BinaryData::Organ_wavSize, false);
    tracks.add (new Track (organ, String ("Organ"), xPos, 500, trackColours.getColour (tracks.size())));

    xPos = (xOffset / 2) - (Track::defaultWidth / 2) + xOffset * tracks.size();
    MemoryInputStream* vox1 = new MemoryInputStream (BinaryData::Vox1_wav, BinaryData::Vox1_wavSize, false);
    tracks.add (new Track (vox1, String ("Vox1"), xPos, 500, trackColours.getColour (tracks.size())));

    xPos = (xOffset / 2) - (Track::defaultWidth / 2) + xOffset * tracks.size();
    MemoryInputStream* vox2 = new MemoryInputStream (BinaryData::Vox2_wav, BinaryData::Vox2_wavSize, false);
    tracks.add (new Track (vox2, String ("Vox2"), xPos, 500, trackColours.getColour (tracks.size())));

    xPos = (xOffset / 2) - (Track::defaultWidth / 2) + xOffset * tracks.size();
    MemoryInputStream* vox3 = new MemoryInputStream (BinaryData::Vox3_wav, BinaryData::Vox3_wavSize, false);
    tracks.add (new Track (vox3, String ("Vox3"), xPos, 500, trackColours.getColour (tracks.size())));

    xPos = (xOffset / 2) - (Track::defaultWidth / 2) + xOffset * tracks.size();
    MemoryInputStream* vox4 = new MemoryInputStream (BinaryData::Vox4_wav, BinaryData::Vox4_wavSize, false);
    tracks.add (new Track (vox4, String ("Vox4"), xPos, 500, trackColours.getColour (tracks.size())));

    for (auto* track : tracks)
        addAndMakeVisible (track);
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
}

void MainComponent::mouseDown (const MouseEvent& /*event*/)
{
    clearSelectedTrack();
}

void MainComponent::clearSelectedTrack()
{
    for (auto track : tracks)
        track->setSelected (false);
    repaint();
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
            return track->doKeyPressed (key);
    }

    return false;
}

void MainComponent::togglePlay()
{
    master->togglePlay();
    for (auto track : tracks)
        track->rewind();
}
