#ifndef TOGGLELOOP_H_INCLUDED
#define TOGGLELOOP_H_INCLUDED

#include "ActionHelper.h"

class ToggleLoop : public UndoableAction
{
public:
    ToggleLoop (MainComponent* mc) :
        mc (mc)
    {}

    ~ToggleLoop() {}

    bool perform() override
    {
        if (mc->getTracks().isEmpty())
            return false;

        for (auto track : mc->getTracks())
            track->getProcessor()->toggleLoop();
        return true;
    }

    bool undo() override
    {
        return perform();
    }

private:
    MainComponent* mc;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ToggleLoop)
};

#endif //TOGGLELOOP_H_INCLUDED
