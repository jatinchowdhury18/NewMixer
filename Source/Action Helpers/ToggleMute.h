#ifndef TOGGLEMUTE_H_INCLUDED
#define TOGGLEMUTE_H_INCLUDED

#include "Track.h"

class ToggleMute : public UndoableAction
{
public:
    ToggleMute (Track* track) :
        track (track)
    {}

    ~ToggleMute() {}

    bool perform() override
    {
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
    Track* track;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ToggleMute)
};

#endif //TOGGLEMUTE_H_INCLUDED
