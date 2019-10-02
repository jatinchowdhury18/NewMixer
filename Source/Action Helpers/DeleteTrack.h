#ifndef DELETETRACK_H_INCLUDED
#define DELETETRACK_H_INCLUDED

#include "ActionHelper.h"

class DeleteTrack : public UndoableAction
{
public:
    DeleteTrack (MainComponent* mc, String uuid) :
        mc (mc),
        uuid (uuid)
    {}

    ~DeleteTrack()
    {
        if (trackToKeep != nullptr)
        {
            auto proc = dynamic_cast<TrackProcessor*> (trackToKeep->getProcessor());
            if (proc != nullptr)
                delete proc->getReader();
        }
    }

    bool perform() override
    {
        trackToDelete = ActionHelper::getTrackWithUuid (mc, uuid);
        if (trackToDelete == nullptr)
            return false;

        x = trackToDelete->getBounds().getCentreX();
        y = trackToDelete->getBounds().getCentreY();

        trackToKeep.reset (new Track (*trackToDelete, trackToDelete->getUuid()));
        ActionHelper::deleteTrack (trackToDelete, mc);

        mc->repaint();
        return true;
    }

    bool undo() override
    {
        if (trackToKeep.get() == nullptr)
            return false;

        trackToDelete = trackToKeep.release();
        ActionHelper::addTrack (trackToDelete, mc, x, y);
        return true;
    }

private:
    MainComponent* mc;
    const String uuid;
    Track* trackToDelete;
    std::unique_ptr<Track> trackToKeep;

    int x;
    int y;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DeleteTrack)
};

#endif //DELETETRACK_H_INCLUDED
