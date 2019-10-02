#ifndef TOGGLEMUTE_H_INCLUDED
#define TOGGLEMUTE_H_INCLUDED

#include "Track.h"
#include "ActionHelper.h"

class ToggleMute : public UndoableAction
{
public:
    ToggleMute (MainComponent* mc, String uuid) :
        mc (mc),
        uuid (uuid)
    {}

    ~ToggleMute() {}

    bool perform() override
    {
        auto track = ActionHelper::getTrackWithUuid (mc, uuid);
        if (track == nullptr)
            return false;

        track->toggleMute();
        return true;
    }

    bool undo() override
    {
        return perform();
    }

private:
    MainComponent* mc;
    const String uuid;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ToggleMute)
};

#endif //TOGGLEMUTE_H_INCLUDED
