#ifndef SETTINGS_H_INCLUDED
#define SETTINGS_H_INCLUDED

#include "JuceHeader.h"

class AudioSettings : public Component,
                      public ChangeListener
{
public:
    AudioSettings (AudioDeviceManager& manager);
    ~AudioSettings();

    void paint (Graphics& g) override;
    void resized() override;

    void dumpDeviceInfo();
    void logMessage (const String& m);

private:
    AudioDeviceManager& audioDeviceManager;

    std::unique_ptr<AudioDeviceSelectorComponent> audioSetupComp;
    TextEditor diagnosticsBox;

    void changeListenerCallback (ChangeBroadcaster*) override;
    void lookAndFeelChanged() override;
    static String getListOfActiveBits (const BigInteger& b);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioSettings)
};

class SettingsWindow : public DocumentWindow
{
public:
    SettingsWindow (const String& name, AudioDeviceManager& manager);

    void closeButtonPressed() override { setVisible (false); }

private:
    std::unique_ptr<AudioSettings> settings;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SettingsWindow)
};

#endif //SETTINGS_H_INCLUDED
