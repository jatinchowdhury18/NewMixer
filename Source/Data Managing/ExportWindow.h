#ifndef EXPORTWINDOW_H_INCLUDED
#define EXPORTWINDOW_H_INCLUDED

#include "MasterTrackProcessor.h"

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

private:
    void initSettingsComponents();

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
