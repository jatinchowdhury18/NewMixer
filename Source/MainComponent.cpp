#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent()
{
    File file = File ("C:/Users/jatin/Desktop/drums.wav");
    tracks.add (new Track (file, 400, 500));

    File file2 = File ("C:/Users/jatin/Desktop/test tone.wav");
    tracks.add (new Track (file2, 500, 500));

    for (auto* track : tracks)
        addAndMakeVisible (track);

    master = new MasterTrack (tracks);

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
    for (auto track : tracks)
        track->setBounds (track->getX(), track->getY(), 50, 50);
}

void MainComponent::itemDropped (const SourceDetails& dragSourceDetails)
{
    for (auto track : tracks)
    {
        if (dragSourceDetails.sourceComponent == track)
        {
            Point<int> newPos = dragSourceDetails.localPosition;
            newPos.x -= track->getWidth() / 2;
            newPos.y -= track->getWidth() / 2;

            track->setTopLeftPosition (newPos);
            break;
        }
    }
}
