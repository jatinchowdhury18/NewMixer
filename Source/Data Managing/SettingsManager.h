#ifndef SETTINGSMANAGER_H_INCLUDED
#define SETTINGSMANAGER_H_INCLUDED

#include "MainComponent.h"

class SettingsManager
{
public:
    SettingsManager();
    ~SettingsManager();

    File& getSettingsFile();

    void setPluginFolder (String folder);
    FileSearchPath getPluginFolder();

    void updateAudioDeviceState (XmlElement* newDeviceState);
    std::unique_ptr<XmlElement> getAudioDeviceState();

    JUCE_DECLARE_SINGLETON (SettingsManager, false)

private:
    void createSettingsFile();

    File settingsFile;
};

#endif //SETTINGSMANAGER_H_INCLUDED
