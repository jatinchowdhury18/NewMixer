#ifndef SAVINGTEST_H_INCLUDED
#define SAVINGTEST_H_INCLUDED

#include "SessionManager.h"

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
        MainComponent main;

        const auto root = File::getCurrentWorkingDirectory().getParentDirectory().getParentDirectory();
        const auto stemsFolder = root.getChildFile ("Stems\\Bridge");
        const auto stems = stemsFolder.findChildFiles (File::TypesOfFileToFind::findFiles, false, "*.wav");

        for (int i = 0; i < numTestTracks; i++)
        {
            auto* newTrack = new Track (stems.getReference (i), {}, {}, main.getNextColour());
            ActionHelper::addTrack (newTrack, &main, 0, 0);
        }

        main.getTracks()[indexTrackAutomate]->getAutoHelper()->setRecorded (true);
        main.getTracks()[indexTrackUnmute]->toggleMute();

        auto saveFolder = File (root.getFullPathName() + "\\Testing\\Song").getNonexistentSibling();
        SessionManager::saveSessionAs (&main, &saveFolder);

        testSave (saveFolder, stems);

        const auto chowFile = saveFolder.getChildFile (saveFolder.getFileNameWithoutExtension() + ".chow");
        SessionManager::newSession (&main);
        SessionManager::openSession (&main, &chowFile);

        testOpen (main);
    }

    void testSave (File saveFolder, const Array<File>& stems)
    {
        beginTest ("Saving...");

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
        beginTest ("Opening...");

        expectEquals<int> (main.getTracks().size(), numTestTracks, "Incorrect number of tracks created!");

        expect (main.getTracks()[indexTrackAutomate]->getAutoHelper()->isRecorded(), "Automation notsaved!");

        expect (main.getTracks()[indexTrackUnmute]->getProcessor()->getIsMute() == false, "Mute information not saved!");
    }
};

#endif //SAVINGTEST_H_INCLUDED
