#ifndef SESSIONMANAGER_H_INCLUDED
#define SESSIONMANAGER_H_INCLUDED

#include "MainComponent.h"

class SessionManager
{
public:
    static void newSession (MainComponent* mc);
    static void openSession (MainComponent* mc, const File* sessionFile = nullptr);
    static void saveSession (MainComponent* mc);
    static void saveSessionAs (MainComponent* mc, File* sessionFolder = nullptr);

private:
    static void clearTracks (MainComponent* mc, OwnedArray<Track>& tracks);

    static void parseTrackXml (MainComponent* mc, XmlElement* trackXml);
    static void parsePluginXml (Track* newTrack, XmlElement* pluginXml);
    static bool validateTrackFile (File& file);
    
    static void copyTrackFiles (MainComponent* mc, OwnedArray<Track>& tracks, const File stemsFolder);
    static void saveTracksToXml (const OwnedArray<Track>& tracks, XmlElement* xmlTracks);
    static void saveAutomationToXml (Track* track, XmlElement* xmlTrack);
    static void savePluginsToXml (Track* track, XmlElement* xmlTrack);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SessionManager)
};

#endif // SESSIONMANAGER_H_INCLUDED
