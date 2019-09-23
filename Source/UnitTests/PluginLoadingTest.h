#ifndef PLUGINLOADINGTEST_H_INCLUDED
#define PLUGINLOADINGTEST_H_INCLUDED

class PluginLoadingTest : public UnitTest
{
public:
    PluginLoadingTest() : UnitTest("Plugin Loading Test") {}

    void runTest() override
    {
        beginTest ("Loading Test Plugins");

        PluginManager manager;
        auto& pluginList = manager.getPluginList();

        expect (pluginList.getNumTypes() > 0, "No plugins found!");

        std::unique_ptr<KnownPluginList::PluginTree> pluginTree (KnownPluginList::createTree (pluginList.getTypes(), KnownPluginList::SortMethod::defaultOrder));
        auto plugins = pluginTree->plugins;

        int numTestPluginsFound = 0;
        for (const auto plugin : plugins)
        {
            if (plugin.name == "CHOW")
                numTestPluginsFound++;
            else
                continue;

            auto formatName = plugin.pluginFormatName;
            expect (formatName == "VST" || formatName == "VST3" || formatName == "AudioUnit", "Incorrect plugin format");
        }
        expect (numTestPluginsFound > 0, "Test plugin not found");
    }
};

#endif //PLUGINLOADINGTEST_H_INCLUDED
