#ifndef EXPORTWINDOW_H_INCLUDED
#define EXPORTWINDOW_H_INCLUDED

#include "MasterTrackProcessor.h"

class ExportLNF : public LookAndFeel_V4
{
public:
    ExportLNF()
    {
        setColour (PopupMenu::backgroundColourId, Colours::antiquewhite);
        setColour (PopupMenu::textColourId, Colours::red);
        setColour (PopupMenu::highlightedBackgroundColourId, Colours::dodgerblue);

        setColour (AlertWindow::backgroundColourId, Colours::antiquewhite);
        setColour (AlertWindow::outlineColourId, Colours::dodgerblue);
        setColour (AlertWindow::textColourId, Colours::red);
        setColour (ProgressBar::backgroundColourId, Colours::antiquewhite.darker());
        setColour (ProgressBar::foregroundColourId, Colours::dodgerblue);
    }

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ExportLNF)
};

class ExportComponent;
class ExportProgressWindow : public ThreadWithProgressWindow
{
public:
    ExportProgressWindow (MasterTrackProcessor* master, ExportComponent* comp);

    void run() override;
    void threadComplete (bool /*userPressedCancel*/) override;

private:
    MasterTrackProcessor* masterProc;
    ExportComponent* exportComp;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ExportProgressWindow)
};

class ExportComponent : public Component,
                        private MasterTrackProcessor::Listener
{
public:
    ExportComponent (DocumentWindow* window, MasterTrackProcessor* master,
                     int64 sessionLengthSamples, File sessionFile);

    void paint (Graphics& g) override { g.fillAll (Colours::antiquewhite); }
    void resized() override;
    bool keyPressed (const KeyPress& key) override;

    void startExport();

    ExportInfo getExportInfo() { return exportInfo; }

    void exportCompleted() override;
    ExportLNF& getLNF() { return exportLNF; }

private:
    void initSettingsComponents();

    ExportLNF exportLNF;

    DocumentWindow* window;
    MasterTrackProcessor* master;
    File sessionFile;

    ExportInfo exportInfo;

    Label exportLabel;

    ComboBox formatBox;
    ComboBox bitDepthBox;
    ComboBox sampleRateBox;

    TextButton exportButton;

    std::unique_ptr<ExportProgressWindow> progressWindow;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ExportComponent)
};

class ExportWindow : public DocumentWindow
{
public:
    ExportWindow (MasterTrackProcessor* master, int64 sessionLengthSamples, File sessionFile);
    void closeButtonPressed() override { setVisible (false); }
    ExportComponent* getComp() { return comp.get(); }

private:
    std::unique_ptr<ExportComponent> comp;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ExportWindow)
};

#endif //EXPORTWINDOW_H_INCLUDED
