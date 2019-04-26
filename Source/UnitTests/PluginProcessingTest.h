#ifndef PLUGINPROCESSINGTEST_H_INCLUDED
#define PLUGINPROCESSINGTEST_H_INCLUDED

#include "Track.h"
#include "Data Managing/PluginManager.h"

class Track;
class PluginProcessingTest : public UnitTest
{
public:
    PluginProcessingTest() : UnitTest ("PluginProcessingTest") {}

    void runTest() override;
    void initialise() override;
    void shutdown() override;

private:
    File getRoot();
    File getTestStem (const File& root);

    enum
    {
        numChannels = 2,
        blockSize = 512,
    };

    const double sampleRate = 44100.0;
    const float chowLimit = 0.1f;

    Track* track;
};

#endif //PLUGINPROCESSINGTEST_H_INCLUDED
