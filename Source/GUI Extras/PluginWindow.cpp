#include "PluginWindow.h"
#include "WindowHelper.h"

enum
{
    pluginX = 400,
    pluginY = 300,
};

PluginWindow::PluginWindow (AudioPluginInstance* plugin, Track* track) :
    DocumentWindow (track->getName(), Colours::lightgrey, DocumentWindow::closeButton),
    editor (plugin->createEditor())
{
    WindowHelper::setupDefaultDocumentWindow (*this, editor);
    setTopLeftPosition (pluginX, pluginY);
    setResizable (false, false);
    setVisible (true);
    setAlwaysOnTop (true);

    setName (plugin->getPluginDescription().name + "-" + track->getName());
}

void PluginWindow::closeButtonPressed()
{
    setVisible (false);
}
