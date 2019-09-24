#ifndef ADDTRACK_H_INCLUDED
#define ADDTRACK_H_INCLUDED

#include "ActionHelper.h"

class AddTrack : public UndoableAction
{
public:
    AddTrack (MainComponent* mc, int x, int y);
    ~AddTrack();

    bool perform() override;
    bool undo() override;

    bool checkLength (Track* track, const File& fileToCheck);
    bool checkLength (const File& baseFile, const File& fileToCheck);
    bool checkLength (int64 baseLength, const File& fileToCheck);
    void showLengthErrorBox();

private:
    MainComponent* mc;
    Array<File> trackFiles;
    Array<Track*> addedTracks;
    const int x;
    const int y;

    AudioFormatManager formatManager;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AddTrack)
};

#endif //ADDTRACK_H_INCLUDED
