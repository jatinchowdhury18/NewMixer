#include "PluginProcessingTest.h"
#include "TrackProcessor.h"

void PluginProcessingTest::runTest()
{
    beginTest ("Adding plugin");

    //Test plugin
    auto& pluginList = PluginManager::getInstance()->getPluginList();

    std::unique_ptr<KnownPluginList::PluginTree> pluginTree (KnownPluginList::createTree (pluginList.getTypes(), KnownPluginList::SortMethod::defaultOrder));
    auto pluginArray = pluginTree->plugins;

    for (auto plugin : pluginArray)
    {
        if (plugin.name == "CHOW")
            track->getProcessor()->getPluginChain()->addPlugin (&plugin);
    }
    track->openPluginWindow (0);
   
    beginTest ("Audio Processing Test");
    AudioBuffer<float> buffer (numChannels, blockSize);
    MidiBuffer midiBuffer;
    
    track->getProcessor()->prepareToPlay (sampleRate, blockSize);
    track->getProcessor()->processBlock (buffer, midiBuffer);
    track->getProcessor()->releaseResources();
    
    for (int ch = 0; ch <  numChannels; ch++)
    {
        auto x = buffer.getReadPointer (ch);
        for (int n = 0; n < blockSize; n++)
        {
            expect (x[n] < chowLimit);
        }
    }
   
    beginTest ("Unloading plugin");
    track->closePluginWindow();
    track->getProcessor()->getPluginChain()->removePlugin (0);
}

void PluginProcessingTest::initialise()
{
    File stem = getTestStem (getRoot());
    track = new Track (stem, "Test", "Tst", Colours::white);
    track->initialise (100, 100, 0);
}

void PluginProcessingTest::shutdown()
{
    PluginManager::deleteInstance();
    delete dynamic_cast<TrackProcessor*> (track->getProcessor())->getReader();
    delete track;
}

File PluginProcessingTest::getRoot()
{
    auto root = File::getCurrentWorkingDirectory();
    while (root.getFileName() != "NewMixer")
        root = root.getParentDirectory();
    return root;
}

File PluginProcessingTest::getTestStem (const File& root)
{
    auto stemsFolder = root.getChildFile ("Stems").getChildFile ("Test");
    return stemsFolder.getChildFile ("test_drums.wav");
}
