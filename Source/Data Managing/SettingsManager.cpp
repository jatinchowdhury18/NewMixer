#include "SettingsManager.h"
#include "PluginManager.h"

JUCE_IMPLEMENT_SINGLETON (SettingsManager)

namespace
{
    const String xmlName = "GlobalSettings";
    const String pluginXMLTag = "pluginsPath";
    const String deviceSettingsName = "DEVICESETUP";
    const String dne = "DNE";
}

SettingsManager::SettingsManager()
{
    createSettingsFile();
}

SettingsManager::~SettingsManager()
{
    clearSingletonInstance();
}

File& SettingsManager::getSettingsFile()
{ 
    return settingsFile;
}

std::unique_ptr<XmlElement> SettingsManager::getAudioDeviceState()
{
    std::unique_ptr<XmlElement> settingsXML (parseXML (settingsFile));

    auto deviceStateXML = settingsXML->getChildByName (deviceSettingsName);
    if (deviceStateXML == nullptr)
        return nullptr;

    return std::make_unique<XmlElement> (*deviceStateXML);
}

void SettingsManager::updateAudioDeviceState (XmlElement* newDeviceState)
{
    if (newDeviceState == nullptr)
        return;

    std::unique_ptr<XmlElement> settingsXML (parseXML (settingsFile));
    settingsXML->deleteAllChildElementsWithTagName (deviceSettingsName);
    settingsXML->addChildElement (newDeviceState);
    settingsXML->writeTo (settingsFile, {});
}

void SettingsManager::setPluginFolder (String folder)
{
    std::unique_ptr<XmlElement> settingsXml (parseXML (settingsFile));
    settingsXml->setAttribute (pluginXMLTag, folder);
    settingsXml->writeTo (settingsFile, {});
}

FileSearchPath SettingsManager::getPluginFolder()
{
    std::unique_ptr<XmlElement> settingsXML (parseXML (settingsFile));
    return FileSearchPath (settingsXML->getStringAttribute (pluginXMLTag));
}

void SettingsManager::createSettingsFile()
{
    File appDataFolder = File::getSpecialLocation (File::userApplicationDataDirectory).getChildFile ("NewMixer");
    if (! appDataFolder.exists())
        appDataFolder.createDirectory();

    settingsFile = appDataFolder.getChildFile ("Settings.xml");
    if (! settingsFile.exists())
        settingsFile.create();

    std::unique_ptr<XmlElement> settingsXML (parseXML (settingsFile));
    if (settingsXML.get() == nullptr) // XML doesn't exist
    {
        settingsXML.reset (new XmlElement (xmlName));

        // set up plugin path
        settingsXML->setAttribute (pluginXMLTag, PluginManager::getInstance()->getDefaultPluginFolder().toString());
    }
    else
    {
        if (settingsXML->getStringAttribute (pluginXMLTag, dne) == dne) // plugins path not set
            settingsXML->setAttribute (pluginXMLTag, PluginManager::getInstance()->getDefaultPluginFolder().toString());
    }

    settingsXML->writeTo (settingsFile, {});
}
