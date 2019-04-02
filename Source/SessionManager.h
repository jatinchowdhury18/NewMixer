#ifndef SESSIONMANAGER_H_INCLUDED
#define SESSIONMANAGER_H_INCLUDED

#include "MainComponent.h"

class SessionManager
{
public:
    static void newSession (MainComponent* mc);
    static void openSession (MainComponent* mc);
    static void saveSession (MainComponent* mc);
    static void saveSessionAs (MainComponent* mc);

private:
    static void clearTracks (MainComponent* mc, OwnedArray<Track>& tracks);

    static void parseTrackXml (MainComponent* mc, XmlElement* trackXml);
    static void parseAutomationXml (Track* newTrack, XmlElement* pointXml);
    
    static void copyTrackFiles (MainComponent* mc, OwnedArray<Track>& tracks, const File stemsFolder);
    static void saveTracksToXml (const OwnedArray<Track>& tracks, XmlElement* xmlTracks);
    static void saveAutomationToXml (Track* track, XmlElement* xmlTrack);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SessionManager)
};

#endif // SESSIONMANAGER_H_INCLUDED
