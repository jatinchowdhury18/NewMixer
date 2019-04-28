#ifndef WINDOWHELPER_H_INCLUDED
#define WINDOWHELPER_H_INCLUDED

#include "JuceHeader.h"

class WindowHelper
{
public:
    static Image getLogo()
    {
        return ImageCache::getFromMemory (BinaryData::logo_256_png, BinaryData::logo_256_pngSize);
    }

    static void setIconForDocumentWindow (DocumentWindow& dw)
    {
        auto icon = getLogo();
        dw.setIcon (icon);
        dw.getPeer()->setIcon (icon); //To affect the native titlebar icon
    }

    static void setupDefaultDocumentWindow (DocumentWindow& dw, Component* compToOwn, bool useNativeTitleBar = true)
    {
        jassert (compToOwn != nullptr);

        dw.setContentOwned (compToOwn, true);
        dw.centreWithSize (compToOwn->getWidth(), compToOwn->getHeight());
        WindowHelper::setIconForDocumentWindow (dw);
        dw.setUsingNativeTitleBar (useNativeTitleBar);
        dw.setDraggable (true);
    }
};

#endif //WINDOWHELPER_H_INCLUDED
