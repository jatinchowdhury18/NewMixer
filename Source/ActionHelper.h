#ifndef ACTIONHELPER_H_INCLUDED
#define ACTIONHELPER_H_INCLUDED

#include "MainComponent.h"

class ActionHelper
{
public:
    using Cmds = MainComponent::Cmds;

    static void rightClickMenu (MainComponent* mc, const MouseEvent& e);
    static void rightClickCallback (int result, MainComponent* mc, Point<int> p);
    static bool doKeyPressed (MainComponent* mc, const KeyPress& key);

    static void togglePlay (MainComponent* mc);
    static void toggleLoop (MainComponent* mc);
    static void rewind (MainComponent* mc);

    static void duplicateSelectedTrack (MainComponent* mc);
    static void deleteSelectedTrack (MainComponent* mc);
    static void soloSelectedTrack (MainComponent* mc);

    static void clearSelectedTrack (MainComponent* mc);
    static void changeSelect (MainComponent* mc, bool forward);

    static void addFileTrack (MainComponent* mc, int x, int y);
    static void addRecordingTrack (MainComponent* mc, int x, int y);
    static void addTrack (Track* track, MainComponent* mc, int x, int y);

private:
    static bool validTrackFile (Track* firstTrack, Track* newTrack, MainComponent* mc);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ActionHelper)
};

#endif //ACTIONHELPER_H_INCLUDED
