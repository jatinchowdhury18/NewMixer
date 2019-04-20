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

        expectEquals (pluginList.getNumTypes(), 1, "Incorrect number of plugin types");

        auto plugins = pluginList.createTree (KnownPluginList::SortMethod::defaultOrder)->plugins;

        expectEquals<String> (plugins[0]->name, "CHOWTapeModel", "Incorrect plugin name");
        expectEquals<String> (plugins[0]->pluginFormatName, "VST", "Incorrect plugin format");
    }
};

#endif //PLUGINLOADINGTEST_H_INCLUDED
