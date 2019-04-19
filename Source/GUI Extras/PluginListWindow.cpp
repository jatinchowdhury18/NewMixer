#include "PluginListWindow.h"
#include "WindowHelper.h"

PluginListWindow::PluginListWindow (AudioPluginFormatManager& formatManager, KnownPluginList& knownPluginList) :
    DocumentWindow (TRANS ("Plugins"), Colours::lightgrey, DocumentWindow::allButtons)
{
    pluginListComponent = new PluginListComponent (formatManager, knownPluginList, File(), nullptr, true);
    pluginListComponent->setSize (600, 600);

    WindowHelper::setupDefaultDocumentWindow (*this, pluginListComponent);
    setResizable (true, false);
}

void PluginListWindow::closeButtonPressed()
{
    setVisible (false);
}
