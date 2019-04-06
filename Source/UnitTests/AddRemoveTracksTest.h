#ifndef ADDREMOVETRACKSTEST_H_INCLUDED
#define ADDREMOVETRACKSTEST_H_INCLUDED

class AddRemoveTracksTest : public UnitTest
{
public:
    AddRemoveTracksTest() : UnitTest ("Track adding and removing") {}

    void runTest() override
    {
        MainComponent main;

        beginTest ("Adding Tracks");

        for (int i = 0; i < MainComponent::numTestTracks; i++)
            ActionHelper::addRecordingTrack (&main, 0, 0);

        ActionHelper::togglePlay (&main);

        beginTest ("Removing tracks");
        for (int i = MainComponent::numTestTracks - 1; i >= 0; i--)
        {
            main.tracks[i]->setSelected (true);
            main.tracks[i]->deleteSelectedTrack();
            ActionHelper::togglePlay (&main);
        }

        main.deleteSelectedTrack();
    }
};

#endif //ADDREMOVETRACKSTEST_H_INCLUDED
