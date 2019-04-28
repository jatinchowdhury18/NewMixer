#include "ExportWindow.h"

namespace
{
    enum
    {
        yBuffer = 10,
        xBuffer = 10,
        buttonHeight = 30,
        buttonWidth = 80,
        boxWidth = 120,
    };

    const StringArray WavBitDepths ({ "32", "24", "16" });
    const StringArray AiffBitDepths ({ "24", "16" });
};

ExportWindow::ExportWindow (MasterTrackProcessor* master, int64 sessionLengthSamples, File sessionFile) :
  DocumentWindow ("Export Session", Desktop::getInstance().getDefaultLookAndFeel()
  .findColour (ResizableWindow::backgroundColourId),
  DocumentWindow::closeButton)
{
    comp.reset (new ExportComponent (this, master, sessionLengthSamples, sessionFile));

    setUsingNativeTitleBar (true);
    setContentOwned (comp.get(), true);

    setResizable (false, false);
    centreWithSize (getWidth(), getHeight());

    setVisible (true);
}

ExportComponent::ExportComponent (DocumentWindow* window, MasterTrackProcessor* master,
                                  int64 sessionLengthSamples, File sessionFile) :
    window (window),
    master (master),
    sessionFile (sessionFile)
{
    setSize (2 * xBuffer + boxWidth, 2 * yBuffer + 6 * buttonHeight);
    initSettingsComponents();
    setWantsKeyboardFocus (true);

    master->addListener (this);
    progressWindow.reset (new ExportProgressWindow (master, this));

    exportInfo.lengthSamples = sessionLengthSamples;
}

void ExportComponent::initSettingsComponents()
{
    exportLabel.setText ("Export: ", dontSendNotification);
    exportLabel.setColour (Label::textColourId, Colours::dodgerblue);
    exportLabel.setFont (Font (20.0).boldened());
    addAndMakeVisible (exportLabel);

    exportButton.setButtonText (String ("Export"));
    exportButton.setColour (TextButton::textColourOffId, Colours::antiquewhite);
    exportButton.setColour (TextButton::buttonColourId, Colours::dodgerblue);
    exportButton.setColour (ComboBox::outlineColourId, Colours::red);
    exportButton.onClick = [this] () { startExport(); };
    addAndMakeVisible (exportButton);

    auto setupBox = [this] (ComboBox& box, StringArray choice,
        std::function<void()> onChange = {}, int firstIndex = 1)
    {
        box.addItemList (choice, firstIndex);
        box.setSelectedId (firstIndex, dontSendNotification);
        box.onChange = onChange;

        box.setColour (ComboBox::backgroundColourId, Colours::transparentWhite);
        box.setColour (ComboBox::outlineColourId, Colours::dodgerblue);
        box.setColour (ComboBox::textColourId, Colours::red);
        box.setColour (ComboBox::arrowColourId, Colours::red);
        getLookAndFeel().setColour (PopupMenu::backgroundColourId, Colours::antiquewhite);
        getLookAndFeel().setColour (PopupMenu::textColourId, Colours::red);
        getLookAndFeel().setColour (PopupMenu::highlightedBackgroundColourId, Colours::dodgerblue);

        addAndMakeVisible (box);
    };

    std::function<void()> bitDepthLamda = [this]
    {
        if (exportInfo.format == ExportInfo::ExportFormat::WAV)
            exportInfo.bitDepth = WavBitDepths[bitDepthBox.getSelectedItemIndex()].getIntValue();
        if (exportInfo.format == ExportInfo::ExportFormat::AIFF)
            exportInfo.bitDepth = AiffBitDepths[bitDepthBox.getSelectedItemIndex()].getIntValue();
    };

    setupBox (bitDepthBox, WavBitDepths, bitDepthLamda);

    setupBox (sampleRateBox, StringArray ({ "44100" }));
    sampleRateBox.setEnabled (false);

    std::function<void()> formatLamda = [this]
    {
        exportInfo.format = ExportInfo::ExportFormat (formatBox.getSelectedId());

        //AIFF cannot export 32 bit
        bitDepthBox.clear (dontSendNotification);
        if (exportInfo.format == ExportInfo::ExportFormat::WAV)
            bitDepthBox.addItemList (WavBitDepths, 1);
        else if (exportInfo.format == ExportInfo::ExportFormat::AIFF)
            bitDepthBox.addItemList (AiffBitDepths, 1);

        bitDepthBox.setSelectedId (1, sendNotificationSync);
    };
    setupBox (formatBox, StringArray ({ "WAV", "AIFF" }), formatLamda, ExportInfo::ExportFormat::WAV);
}

void ExportComponent::resized()
{
    exportLabel.setBounds (xBuffer, 0, buttonWidth, buttonHeight);

    formatBox.setBounds     (xBuffer, exportLabel.getBottom() + yBuffer, boxWidth, buttonHeight);
    bitDepthBox.setBounds   (xBuffer, formatBox.getBottom()   + yBuffer, boxWidth, buttonHeight);
    sampleRateBox.setBounds (xBuffer, bitDepthBox.getBottom() + yBuffer, boxWidth, buttonHeight);

    exportButton.setBounds ((getWidth() - buttonWidth) / 2, getBottom() - buttonHeight - yBuffer, buttonWidth, buttonHeight);
}

bool ExportComponent::keyPressed (const KeyPress& key)
{
    if (key == KeyPress::returnKey)
    {
        startExport();
        return true;
    }

    return false;
}

void ExportComponent::startExport()
{
    File defaultExportFile = sessionFile.exists() ? sessionFile.getChildFile (sessionFile.getFileName()) : File();
    auto formatString = ExportInfo::getStringForFormat (exportInfo.format);
    
    FileChooser nativeFileChooser (String ("Export File"), defaultExportFile, "*." + formatString);

    if (nativeFileChooser.browseForFileToSave (true))
    {
        exportInfo.exportFile = nativeFileChooser.getResult().withFileExtension (formatString);

        progressWindow->launchThread();
    }
}

void ExportComponent::exportCompleted()
{
    MessageManager::callAsync ([this] () { window->closeButtonPressed(); });
}

ExportProgressWindow::ExportProgressWindow (MasterTrackProcessor* master, ExportComponent* comp) :
    ThreadWithProgressWindow (String ("Exporting..."), true, false, 1000000),
    masterProc (master),
    exportComp (comp)

{
    getAlertWindow()->getLookAndFeel().setColour (AlertWindow::backgroundColourId, Colours::antiquewhite);
    getAlertWindow()->getLookAndFeel().setColour (AlertWindow::outlineColourId, Colours::dodgerblue);
    getAlertWindow()->getLookAndFeel().setColour (AlertWindow::textColourId, Colours::red);
    getAlertWindow()->getLookAndFeel().setColour (ProgressBar::backgroundColourId, Colours::antiquewhite.darker());
    getAlertWindow()->getLookAndFeel().setColour (ProgressBar::foregroundColourId, Colours::dodgerblue);
    getAlertWindow()->sendLookAndFeelChange();
}

void ExportProgressWindow::run()
{
    //@TODO: options for export file type, sample rate, bit depth
    masterProc->prepareToExport (exportComp->getExportInfo());
    masterProc->exportToFile (exportComp->getExportInfo(), this);
}

void ExportProgressWindow::threadComplete (bool /*userPressedCancel*/)
{
    masterProc->restoreAfterExporting();
}
