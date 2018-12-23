#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent()
{
    File rootDir = File::getCurrentWorkingDirectory().getParentDirectory().getParentDirectory();
    File file = File (rootDir.getFullPathName() + "\\Stems\\drums.wav");
    tracks.add (new Track (file, String ("Drums"), 400, 500));

    File file2 = File (rootDir.getFullPathName() + "\\Stems\\test tone.wav");
    tracks.add (new Track (file2, String ("Buzz"), 500, 500));

    for (auto* track : tracks)
        addAndMakeVisible (track);

    master = new MasterTrack (tracks);

    setSize (width, height);

    tooltipWindow = new TooltipWindow (this, 500);
}

MainComponent::~MainComponent()
{
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
    for (auto track : tracks)
        track->setBounds (track->getX(), track->getY(), 50, 50);
}

void MainComponent::mouseDown (const MouseEvent& event)
{
    clearSelectedTrack();
}

void MainComponent::clearSelectedTrack()
{
    for (auto track : tracks)
        track->setSelected (false);
    repaint();
}
