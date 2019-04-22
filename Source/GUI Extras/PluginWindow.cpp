#include "PluginWindow.h"
#include "WindowHelper.h"

PluginWindow::PluginWindow (AudioProcessorEditor* editor) :
    DocumentWindow (editor->getName(), Colours::lightgrey, DocumentWindow::allButtons),
    editor (editor)
{
    WindowHelper::setupDefaultDocumentWindow (*this, editor);
    setResizable (true, false);
    setVisible (true);
}

void PluginWindow::closeButtonPressed()
{
    setVisible (false);
}
