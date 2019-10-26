#ifndef SAVINGTEST_H_INCLUDED
#define SAVINGTEST_H_INCLUDED

#include "Data Managing/SessionManager.h"
#include "ActionHelper.h"

class SavingTest : public UnitTest
{
    enum
    {
        numTestTracks = 5,

        indexTrackAutomate = 0,
        indexTrackUnmute   = 1,
        indexTrackColour   = 2,
    };

public:
    SavingTest() : UnitTest (String ("Saving and Opening")) {}

    void runTest() override;
    void shutdown() override;

private:
    void normalTest();
    void pluginTest();

    void testSave (File saveFolder, const Array<File>& stems);
    void testOpen (MainComponent& main);
    void testPluginOpen (Track* track);

    File save (MainComponent& main, File root);
    void open (MainComponent& main, File saveFolder);

    File getRoot();
    File getTestFolder (const File& root);

    const Array<File> getStems (const File& root);
    void addTracks (MainComponent& main, const Array<File>& stems);
    void addPlugin (Track* track);
};

#endif //SAVINGTEST_H_INCLUDED
