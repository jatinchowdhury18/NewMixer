#include "SavingTest.h"
#include "Data Managing/PluginManager.h"

void SavingTest::runTest()
{
    beginTest ("Normal Save and Open");
    normalTest();

    beginTest ("Save and Open with Plugin");
    pluginTest();
}

void SavingTest::shutdown()
{
    auto testFolder = getTestFolder (getRoot());
    testFolder.deleteRecursively();
}

void SavingTest::normalTest()
{
    MainComponent main;

    auto root = getRoot();
    const auto stems = getStems (root);
    addTracks (main, stems);

    main.getTracks()[indexTrackAutomate]->getAutoHelper()->setRecorded (true);
    main.getTracks()[indexTrackUnmute]->toggleMute();

    auto saveFolder = save (main, root);
    testSave (saveFolder, stems);

    open (main, saveFolder);
    testOpen (main);
}

void SavingTest::pluginTest()
{
    MainComponent main;

    auto root = getRoot();
    const auto stems = getStems (root);
    addTracks (main, stems);
    addPlugin (main.getTracks().getFirst());

    auto saveFolder = save (main, root);
    open (main, saveFolder);
    
    testPluginOpen (main.getTracks().getFirst());
}

File SavingTest::save (MainComponent& main, File root)
{
    auto saveFolder = File (getTestFolder (root).getChildFile("Song").getNonexistentSibling());
    SessionManager::saveSessionAs (&main, &saveFolder);

    return saveFolder;
}

void SavingTest::open (MainComponent& main, File saveFolder)
{
    const auto chowFile = saveFolder.getChildFile (saveFolder.getFileNameWithoutExtension() + ".chow");
    SessionManager::openSession (&main, &chowFile);
}

void SavingTest::testSave (File saveFolder, const Array<File>& stems)
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

void SavingTest::testOpen (MainComponent& main)
{
    expectEquals<int> (main.getTracks().size(), numTestTracks, "Incorrect number of tracks created!");

    expect (main.getTracks()[indexTrackAutomate]->getAutoHelper()->isRecorded(), "Automation not saved!");

    expect (main.getTracks()[indexTrackUnmute]->getProcessor()->getIsMute() == false, "Mute information not saved!");
}

void SavingTest::testPluginOpen (Track* track)
{
    auto pluginChain = track->getProcessor()->getPluginChain();
    expectEquals<int> (pluginChain->getNumPlugins(), 1, "Incorrect number of plugins opened");

    auto plugin = pluginChain->getPluginList().getFirst();
    if (plugin == nullptr)
        return;

    expectEquals<String> (plugin->getName(), "CHOW", "Incorrect plugin");
    expectEquals<String> (plugin->getPluginDescription().pluginFormatName, "VST3", "Incorrect plugin");
    expectEquals<float> (plugin->getParameters().getFirst()->getValue(), 0.5f, "Incorrect plugin parameter");
}

File SavingTest::getRoot()
{
    auto root = File::getCurrentWorkingDirectory();
    while (root.getFileName() != "NewMixer")
        root = root.getParentDirectory();
    return root;
}

File SavingTest::getTestFolder (const File& root)
{
    auto testFolder = root.getChildFile ("Testing");
    if (! testFolder.exists())
        testFolder.createDirectory();
    return testFolder;
}

const Array<File> SavingTest::getStems (const File& root)
{
    const auto stemsFolder = root.getChildFile ("Stems").getChildFile ("Bridge");
    const auto stems = stemsFolder.findChildFiles (File::TypesOfFileToFind::findFiles, false, "*.wav");
    return stems;
}

void SavingTest::addTracks (MainComponent& main, const Array<File>& stems)
{
    for (int i = 0; i < numTestTracks; i++)
    {
        auto stem = File (stems[i]);
        ActionHelper::addTrack (new Track (stem, {}, {}, main.getNextColour()), &main, 0, 0);
    }
}

void SavingTest::addPlugin (Track* track)
{
    auto& pluginList = PluginManager::getInstance()->getPluginList();

    std::unique_ptr<KnownPluginList::PluginTree> pluginTree (KnownPluginList::createTree (pluginList.getTypes(), KnownPluginList::SortMethod::defaultOrder));
    auto pluginArray = pluginTree->plugins;

    for (auto plugin : pluginArray)
    {
        if (plugin.name == "CHOW" && plugin.pluginFormatName == "VST3")
            track->getProcessor()->getPluginChain()->addPlugin (&plugin);
    }
    track->getProcessor()->getPluginChain()->getPluginList().getFirst()->getParameters().getFirst()->setValue (0.5f);
}
