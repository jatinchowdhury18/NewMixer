#ifndef AUTOMATIONTEST_H_INCLUDED
#define AUTOMATIONTEST_H_INCLUDED

class AutomationTest : public UnitTest
{
public:
    AutomationTest() : UnitTest ("Automation") {}

    int randInt()
    {
        Random r;
        r.setSeedRandomly();

        return (r.nextInt() % 1000) - 200;
    }

    float randFloat()
    {
        Random r;
        r.setSeedRandomly();

        return (r.nextFloat());
    }

    void setAutoPoints (OwnedArray<Track>& tracks, Array<float>* x, Array<float>* y, Array<float>* diameter)
    {
        beginTest ("Setting test automation points");
        for (int i = 0; i < MainComponent::numTestAutoPoints; i++)
        {
            for (int t = 0; t < MainComponent::numTestTracks; t++)
            {
                x[t].add (randFloat());
                y[t].add (randFloat());
                diameter[t].add ((float) randInt());
                tracks[t]->getAutoHelper()->addAutoPoint (x[t].getLast(), y[t].getLast(), diameter[t].getLast(), i);
            }
        }
    }

    void checkAutoPoints (OwnedArray<Track>& tracks, Array<float>* x, Array<float>* y, Array<float>* diameter)
    {
        beginTest ("Checking test automation points");
        for (int i = 0; i < MainComponent::numTestAutoPoints; i++)
        {
            for (int t = 0; t < MainComponent::numTestTracks; t++)
            {
                float xTest = 0;
                float yTest = 0;
                float dTest = 0;
                tracks[t]->getAutoHelper()->getPoint (xTest, yTest, dTest, i);

                expect (xTest == x[t][i], "Track x position incorrect: " + String (xTest));
                expect (yTest == y[t][i], "Track y position incorrect: " + String (yTest));
                expect (dTest == diameter[t][i], "Track diameter incorrect: " + String (dTest));
            }
        }
    }

    void runTest() override
    {
        MainComponent main;

        Array<float> x[MainComponent::numTestTracks];
        Array<float> y[MainComponent::numTestTracks];
        Array<float> diameter[MainComponent::numTestTracks];

        for (int i = 0; i < MainComponent::numTestTracks; i++)
            ActionHelper::addRecordingTrack (&main, 0, 0);

        setAutoPoints (main.tracks, x, y, diameter);
        checkAutoPoints (main.tracks, x, y, diameter);
    }
};

#endif //AUTOMATIONTEST_H_INCLUDED
