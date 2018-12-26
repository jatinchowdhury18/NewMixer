#include "MainComponent.h"
#include "Tracks/SoloHelper.h"

//==============================================================================
MainComponent::MainComponent()
{
    addTracks ("Chorus"); //"Test", "Chorus", or "Bridge"

    master = new MasterTrack (tracks);

    setSize (width, height);
    setWantsKeyboardFocus (true);

    tooltipWindow = new TooltipWindow (this, tooltipTime);
}

MainComponent::~MainComponent()
{
}

void MainComponent::addTracks (String stemsToUse)
{
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
