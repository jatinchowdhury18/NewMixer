#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent()
{
    File rootDir = File::getCurrentWorkingDirectory().getParentDirectory().getParentDirectory();
    File file = File (rootDir.getFullPathName() + "\\Stems\\drums.wav");
    tracks.add (new Track (file, String ("Drums"), 400, 500, trackColours.getColour (tracks.size())));

    File file2 = File (rootDir.getFullPathName() + "\\Stems\\test tone.wav");
    tracks.add (new Track (file2, String ("Buzz"), 500, 500, trackColours.getColour (tracks.size())));

    for (auto* track : tracks)
        addAndMakeVisible (track);

    master = new MasterTrack (tracks);

    setSize (width, height);

    tooltipWindow = new TooltipWindow (this, tooltipTime);

    setWantsKeyboardFocus (true);
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
        track->setBounds (track->getX(), track->getY(), 80, 80);
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
        soloButtonPressed();
        repaint();
        return true;
    }

    for (auto track : tracks)
    {
        if (track->getIsSelected())
            return track->doKeyPressed (key);
    }

    return false;
}

void MainComponent::soloButtonPressed()
{
    bool aTrackIsSoloed = setSelectedTrackSolo();
    setOtherTracksSolo (aTrackIsSoloed);
}

bool MainComponent::setSelectedTrackSolo()
{
    for (auto track : tracks)
    {
        if (track->getIsSelected())
        {
            if (track->isSoloed())
            {
                track->setSoloed (TrackProcessor::SoloState::noTracks);
                return false;
            }
            else
            {
                track->setSoloed (TrackProcessor::SoloState::thisTrack);
                return true;
            }
        }
    }

    return false;
}

void MainComponent::setOtherTracksSolo (bool aTrackIsSoloed)
{
    for (auto track : tracks)
    {
        if (! track->getIsSelected())
            track->setSoloed (aTrackIsSoloed ? TrackProcessor::SoloState::otherTrack
                                             : TrackProcessor::SoloState::noTracks);
    }
}
