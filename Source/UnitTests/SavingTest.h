#ifndef SAVINGTEST_H_INCLUDED
#define SAVINGTEST_H_INCLUDED

#include "Data Managing/SessionManager.h"

class SavingTest : public UnitTest
{
    enum
    {
        numTestTracks = 5,

        indexTrackAutomate = 0,
        indexTrackUnmute = 1,
    };

public:
    SavingTest() : UnitTest (String ("Saving and Opening")) {}

    void runTest() override
    {
        normalTest();
    }

    void normalTest()
    {
        beginTest ("Normal Save and Open");
        MainComponent main;

        auto root = getRoot();
        const auto stems = getStems (root);
        addTracks (main, stems);
        
        main.getTracks()[indexTrackAutomate]->getAutoHelper()->setRecorded (true);
        main.getTracks()[indexTrackUnmute]->toggleMute();

        auto saveFolder = File (getTestFolder (root).getChildFile("Song").getNonexistentSibling());
        SessionManager::saveSessionAs (&main, &saveFolder);
        testSave (saveFolder, stems);

        const auto chowFile = saveFolder.getChildFile (saveFolder.getFileNameWithoutExtension() + ".chow");
        SessionManager::openSession (&main, &chowFile);
        testOpen (main);
    }

    void testSave (File saveFolder, const Array<File>& stems)
    {
        const auto songStemsFolder = saveFolder.getChildFile ("Stems");
        expectEquals<int> (songStemsFolder.getNumberOfChildFiles (File::TypesOfFileToFind::findFiles, "*.wav"),
                           numTestTracks, "Incorrect number of stems saved!");

        const auto stemsTest = songStemsFolder.findChildFiles (File::TypesOfFileToFind::findFiles, false, "*.wav");
        for (int i = 0; i < numTestTracks; i++)
        {
            expectEquals<String> (stems[i].getFileName(), stemsTest[i].getFileName(), "Incorrect stem file saved!");
        }

        const auto chowFile = saveFolder.getChildFile (saveFolder.getFileNameWithoutExtension() + ".chow");
        expect (chowFile.existsAsFile(), ".chow file not saved!");
    }

    void testOpen (MainComponent& main)
    {
        expectEquals<int> (main.getTracks().size(), numTestTracks, "Incorrect number of tracks created!");

        expect (main.getTracks()[indexTrackAutomate]->getAutoHelper()->isRecorded(), "Automation notsaved!");

        expect (main.getTracks()[indexTrackUnmute]->getProcessor()->getIsMute() == false, "Mute information not saved!");
    }

    File getRoot()
    {
        auto root = File::getCurrentWorkingDirectory();
        while (root.getFileName() != "NewMixer")
            root = root.getParentDirectory();
        return root;
    }

    File getTestFolder (const File& root)
    {
        auto testFolder = root.getChildFile ("Testing");
        if (! testFolder.exists())
            testFolder.createDirectory();
        return testFolder;
    }

    const Array<File> getStems (const File& root)
    {
        const auto stemsFolder = root.getChildFile ("Stems").getChildFile ("Bridge");
        const auto stems = stemsFolder.findChildFiles (File::TypesOfFileToFind::findFiles, false, "*.wav");
        return stems;
    }

    void addTracks (MainComponent& main, const Array<File>& stems)
    {
        for (int i = 0; i < numTestTracks; i++)
        {
            auto stem = File (stems[i]);
            ActionHelper::addTrack (new Track (stem, {}, {}, main.getNextColour()), &main, 0, 0);
        }
    }
};

#endif //SAVINGTEST_H_INCLUDED
