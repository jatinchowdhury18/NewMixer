#ifndef PLAYTEST_H_INCLUDED
#define PLAYTEST_H_INCLUDED

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

        //@TODO: Fix this unit test!!!
        // for (int i = 1; i < main.tracks.size(); i++)
        //     checkPlayheads (main.tracks[i - 1], main.tracks[i]);

        ActionHelper::togglePlay (&main);
        playState = ! playState;
    }

    void runTest() override
    {
        beginTest ("Play");
        MainComponent main;
        bool playState = false;

        for (int i = 0; i < MainComponent::numTestTracks; i++)
        {
            ActionHelper::addRecordingTrack (&main, 0, 0);
            playTests (main, playState);
        } 
    }
};
#endif //PLAYTEST_H_INCLUDED
