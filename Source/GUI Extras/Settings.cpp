#include "Settings.h"
#include "WindowHelper.h"

SettingsWindow::SettingsWindow (const String& name, AudioDeviceManager& manager)
    : DocumentWindow (name, Desktop::getInstance().getDefaultLookAndFeel()
        .findColour (ResizableWindow::backgroundColourId),
        DocumentWindow::allButtons)
{
    settings.reset (new AudioSettings (manager));

    setResizable (true, false);
    setResizeLimits (300, 250, 10000, 10000);

    WindowHelper::setupDefaultDocumentWindow (*this, settings.get());
    setVisible (true);
}

AudioSettings::AudioSettings (AudioDeviceManager& manager) : audioDeviceManager (manager)
{
    setOpaque (true);
    /*
    RuntimePermissions::request (RuntimePermissions::recordAudio,
    [this] (bool granted)
    {
    int numInputChannels = granted ? 2 : 0;
    audioDeviceManager.initialise (numInputChannels, 2, nullptr, true, {}, nullptr);
    });
    */
    audioSetupComp.reset (new AudioDeviceSelectorComponent (audioDeviceManager,
        0, 256, 0, 256, true, true, true, false));
    addAndMakeVisible (audioSetupComp.get());

    addAndMakeVisible (diagnosticsBox);
    diagnosticsBox.setMultiLine (true);
    diagnosticsBox.setReturnKeyStartsNewLine (true);
    diagnosticsBox.setReadOnly (true);
    diagnosticsBox.setScrollbarsShown (true);
    diagnosticsBox.setCaretVisible (false);
    diagnosticsBox.setPopupMenuEnabled (true);

    audioDeviceManager.addChangeListener (this);

    logMessage ("Audio device diagnostics:\n");
    dumpDeviceInfo();

    setSize (500, 600);
}

AudioSettings::~AudioSettings()
{
    audioDeviceManager.removeChangeListener (this);
}

void AudioSettings::paint (Graphics& g)
{
    g.fillAll (Colours::black);
}

void AudioSettings::resized()
{
    auto r =  getLocalBounds().reduced (4);
    audioSetupComp->setBounds (r.removeFromTop (proportionOfHeight (0.65f)));
    diagnosticsBox.setBounds (r);
}

void AudioSettings::dumpDeviceInfo()
{
    logMessage ("--------------------------------------");
    logMessage ("Current audio device type: " + (audioDeviceManager.getCurrentDeviceTypeObject() != nullptr
        ? audioDeviceManager.getCurrentDeviceTypeObject()->getTypeName()
        : "<none>"));

    if (AudioIODevice* device = audioDeviceManager.getCurrentAudioDevice())
    {
        logMessage ("Current audio device: "   + device->getName().quoted());
        logMessage ("Sample rate: "    + String (device->getCurrentSampleRate()) + " Hz");
        logMessage ("Block size: "     + String (device->getCurrentBufferSizeSamples()) + " samples");
        logMessage ("Output Latency: " + String (device->getOutputLatencyInSamples())   + " samples");
        logMessage ("Input Latency: "  + String (device->getInputLatencyInSamples())    + " samples");
        logMessage ("Bit depth: "      + String (device->getCurrentBitDepth()));
        logMessage ("Input channel names: "    + device->getInputChannelNames().joinIntoString (", "));
        logMessage ("Active input channels: "  + getListOfActiveBits (device->getActiveInputChannels()));
        logMessage ("Output channel names: "   + device->getOutputChannelNames().joinIntoString (", "));
        logMessage ("Active output channels: " + getListOfActiveBits (device->getActiveOutputChannels()));
    }
    else
    {
        logMessage ("No audio device open");
    }
}

void AudioSettings::logMessage (const String& m)
{
    diagnosticsBox.moveCaretToEnd();
    diagnosticsBox.insertTextAtCaret (m + newLine);
}

void AudioSettings::changeListenerCallback (ChangeBroadcaster*)
{
    dumpDeviceInfo();
}

void AudioSettings::lookAndFeelChanged()
{
    diagnosticsBox.applyFontToAllText (diagnosticsBox.getFont());
}

String AudioSettings::getListOfActiveBits (const BigInteger& b)
{
    StringArray bits;

    for (int i = 0; i <= b.getHighestBit(); ++i)
        if (b[i])
            bits.add (String (i));

    return bits.joinIntoString (", ");
}
