/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent()
{
    File file = File ("C:/Users/jatin/Desktop/drums.wav");
    track = new Track (file);
    addAndMakeVisible (*track);

    master = new MasterTrack (track.get());

    setSize (900, 600);
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
    track->setBounds (track->getX(), track->getY(), 50, 50);
}

void MainComponent::itemDropped (const SourceDetails& dragSourceDetails)
{
    Point<int> newPos = dragSourceDetails.localPosition;
    newPos.x -= track->getWidth() / 2;
    newPos.y -= track->getWidth() / 2;

    track->setTopLeftPosition (newPos);
}
