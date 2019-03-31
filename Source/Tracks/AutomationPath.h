#ifndef AUTOMATIONPATH_H_INCLUDED
#define AUTOMATIONPATH_H_INCLUDED

#include "Track.h"

class AutomationPath : public Component
{
public:
    AutomationPath (Track* track);

    void paint (Graphics& g) override;
    void resized() override;

private:
    Track* track;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AutomationPath)
};

#endif AUTOMATIONPATH_H_INCLUDED
