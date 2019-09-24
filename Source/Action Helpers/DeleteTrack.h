#ifndef DELETETRACK_H_INCLUDED
#define DELETETRACK_H_INCLUDED

#include "ActionHelper.h"

class DeleteTrack : public UndoableAction
{
public:
    DeleteTrack (MainComponent* mc, Track* track) :
        mc (mc),
        trackToDelete (track),
        x (track != nullptr ? track->getBounds().getCentreX() : 0),
        y (track != nullptr ? track->getBounds().getCentreY() : 0)
    {}

    ~DeleteTrack()
    {
    }

    bool perform() override
    {
        if (trackToDelete == nullptr)
            return false;

        trackToKeep.reset (new Track (*trackToDelete));
        for (int i = 0; i < mc->getTracks().size(); i++)
        {
            if (mc->getTracks()[i] == trackToDelete)
            {
                mc->getTracks().removeAndReturn (i);
                mc->getAutoPaths().remove (i);

                mc->getWaveform()->deleteTrack (trackToDelete, i);

                for (int j = i; j < mc->getTracks().size(); j++)
                    mc->getTracks()[j]->setIndex (j);
            }
        }

        if (trackToDelete != nullptr)
            mc->getMaster()->removeTrack (trackToDelete);
        delete trackToDelete;

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
    Track* trackToDelete;
    std::unique_ptr<Track> trackToKeep;

    const int x;
    const int y;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DeleteTrack)
};

#endif //DELETETRACK_H_INCLUDED
