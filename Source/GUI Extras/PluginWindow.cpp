#include "PluginWindow.h"
#include "WindowHelper.h"

PluginWindow::PluginWindow (AudioProcessorEditor* editor) :
    DocumentWindow (editor->getName(), Colours::lightgrey, DocumentWindow::closeButton),
    editor (editor)
{
    WindowHelper::setupDefaultDocumentWindow (*this, editor);
    setResizable (false, false);
    setVisible (true);
    setAlwaysOnTop (true);
}

void PluginWindow::closeButtonPressed()
{
    setVisible (false);
}
