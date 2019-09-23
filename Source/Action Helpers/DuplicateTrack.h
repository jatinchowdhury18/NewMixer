#ifndef DUPLICATETRACK_H_INCLUDED
#define DUPLICATETRACK_H_INCLUDED

#include "ActionHelper.h"

class DuplicateTrack : public UndoableAction
{
public:
    DuplicateTrack (MainComponent* mc, Track* track) :
        mc (mc),
        trackToDuplicate (track)
    {}

    bool perform() override
    {
        if (trackToDuplicate == nullptr)
            return false;

        if (trackToDuplicate != nullptr)
            ActionHelper::addTrack (new Track (*trackToDuplicate), mc, 
                trackToDuplicate->getX() + TrackConstants::width * 3 / 4,
                trackToDuplicate->getY() + TrackConstants::width * 3 / 4);

        createdTrack = mc->getTracks().getLast();
        return true; 
    }

    bool undo() override
    {
        if (createdTrack == nullptr)
            return false;

        Track* trackToDelete = nullptr;
        for (int i = 0; i < mc->getTracks().size(); i++)
        {
            if (mc->getTracks()[i] == createdTrack)
            {
                trackToDelete = mc->getTracks().removeAndReturn (i);
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

private:
    MainComponent* mc;
    Track* createdTrack;
    Track* trackToDuplicate;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DuplicateTrack)
};

#endif //DUPLICATETRACK_H_INCLUDED
