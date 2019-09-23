#ifndef TRACKNAMETEST_H_INCLUDED
#define TRACKNAMETEST_H_INCLUDED

class NameTest : public UnitTest
{
public:
    NameTest() : UnitTest ("Name Tracks") {}

    void checkNames (String expName, String expShort)
    {
        expect (expName == track->name, "Track name incorrect: " + track->name
                + ", expected: " + expName);
        expect (expShort == track->shortName, "Track short name incorrect, got: " + track->shortName
                + ", expected: " + expShort);
    }

    void nameTest (String setName, String setShort, String expName, String expShort)
    {
        track->trackNameChanged (setName, setShort);
        checkNames (expName, expShort);
    }

    void runTest() override
    {
        beginTest ("Names");

        nameTest ("", "", "Test", "Tst");
        nameTest ("", "squaminous", "Test", "Tst");
        nameTest ("Guitar", "Gtr", "Guitar", "Gtr");
        nameTest ("Drums", "", "Drums", "Drum");
        nameTest ("Ttesttttttt", "", "Ttesttttttt", "Ttes");
        nameTest ("Gtr", "", "Gtr", "Gtr");
    }

    void initialise() override
    {
        track = new Track (100, 0, false, "Test", "Tst", Colours::white);
        track->initialise (100, 100, 0);
    }

    void shutdown() override 
    {
        delete track;
    }

private:
    Track* track;
};

#endif //TRACKNAMETEST_H_INCLUDED
