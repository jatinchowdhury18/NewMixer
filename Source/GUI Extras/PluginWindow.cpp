#include "PluginWindow.h"
#include "WindowHelper.h"

enum
{
    pluginX = 400,
    pluginY = 300,
};

PluginWindow::PluginWindow (AudioProcessorEditor* editor) :
    DocumentWindow (editor->getName(), Colours::lightgrey, DocumentWindow::closeButton),
    editor (editor)
{
    WindowHelper::setupDefaultDocumentWindow (*this, editor);
    setTopLeftPosition (pluginX, pluginY);
    setResizable (false, false);
    setVisible (true);
    setAlwaysOnTop (true);
}

void PluginWindow::closeButtonPressed()
{
    setVisible (false);
}
