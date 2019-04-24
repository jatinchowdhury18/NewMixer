#ifndef TRACK_H_INCLUDED
#define TRACK_H_INCLUDED

#include "TrackBase.h"
#include "Colours.h"
#include "TrackHelpers/AutomationHelper.h"
#include "TrackRenameWindow.h"
#include "TrackMeter.h"
#include "Timeline/Playhead.h"
#include "AutomationPath.h"
#include "GUI Extras/PluginWindow.h"

namespace TrackConstants
{
    constexpr float darkAlpha = 0.4f;

    constexpr float maxDiameter = 100.0f;
    constexpr float minDiameter = 20.0f;
    constexpr float defaultDiameter = 80.0f;

    enum
    {
        width = 150,
    };
}

class AutomationPath;
class TrackPluginMenu;
class Track : public Component,
              public SettableTooltipClient,
              public Playhead::Listener,
              public Timer,
              private TrackBase::Listener,
              private TrackRenameComponent::Listener
{
public:
    enum TrackCmds
    {
        solo = 0x1001,
        mute,
        rename,
        recordAutomation,
        deleteAutomation,
        deleteTrack,
        duplicateTrack,
    };

    Track (File& file, String name, String shortName, Colour colour);
    Track (MemoryInputStream* input, String name, String shortName, Colour colour);
    Track (int64 sampleLength, int64 startSample, bool playing, String name, String shortName, Colour colour);
    Track (const Track& track);
    void initialise (int x, int y, int ind);
    ~Track();
    void uninitialise();

    TrackBase* getProcessor() const { return processor; }

    void paintOverChildren (Graphics& g) override;
    void resized() override;
    bool hitTest (int x, int y) override;

    bool getIsSelected() { return isSelected; }
    void setSelected (bool selected) { isSelected = selected; }

    bool isSoloed() { return processor->getSoloed() == TrackBase::SoloState::thisTrack; }
    void setSoloed (TrackBase::SoloState state) { processor->setSoloed (state); }

    bool toggleMute();
    void togglePlay();

    void trackMoved();
    void newLoop() override;
    void endReached() override;

    void playheadMoving (int64 posSamples) override { playheadPos = posSamples; }
    int64 getPlayheadPos() { return playheadPos; }

    void trackRename();
    void trackNameChanged (String newName, String newShortName = {}) override;

    TrackColours& getColours() { return colours; }
    Colour getColour() const { return trackColour; }
    void setTrackColour (Colour newColour) { trackColour = newColour; }

    AutoHelper* getAutoHelper() { return autoHelper.get(); }

    void setDiameter (float newD) { diameter = newD; }
    float getDiameter() { return diameter; }

    int getLastDrag() const { return lastDragLocation; }
    void setLastDrag (int drag) { lastDragLocation = drag; }
    void setDragging (bool drag) { isDragging = drag; }

    String getName() const { return name; }
    String getShortName() const { return shortName; }

    bool getIsPlaying() const { return isPlaying; }

    String getFilePath() const;

    class Listener
    {
    public:
        virtual ~Listener() {}
        virtual void deleteSelectedTrack() {}
        virtual void duplicateSelectedTrack() {}
        virtual void soloSelectedTrack() {}
        virtual void trackColourChanged (Track* /*track*/, int /*index*/) {}
    };

    void addListener (Listener* listener) { listeners.add (listener); }
    void removeListener (Listener* listener) { listeners.remove (listener); }
    ListenerList<Listener>& getListeners() { return listeners; }

    void setRelativePosition (float x, float y) { relX = x; relY = y; }
    float getRelX() const { return relX; }
    float getRelY() const { return relY; }

    void setIndex (int ind) { index = ind; }

    void setAutoPath (AutomationPath* ap) { autoPath = ap; }
    void renderAutomationExport();

    void openPluginWindow (int pluginIndex);
    void closePluginWindow();

private:
#if JUCE_DEBUG
    friend class NameTest;
#endif

    void timerCallback() override;
    void mouseDown (const MouseEvent& e) override;
    void mouseDrag (const MouseEvent& e) override;
    void mouseUp (const MouseEvent& e) override;

    void mouseDoubleClick (const MouseEvent& e) override;

#if JUCE_IOS || JUCE_ANDROID   
    Array<MouseEvent> touches;
#endif

    ListenerList<Listener> listeners;

    float relX = 0.0f;
    float relY = 0.0f;

    String name;
    String shortName;
    Colour trackColour;
    TrackColours colours;

    float diameter = TrackConstants::defaultDiameter;
    bool isDragging = false;
    int lastDragLocation = 0;
    int64 playheadPos = -1;

    bool isSelected = false;
    bool isPlaying = false;
    int index = 0;

    TrackBase* processor;

    std::unique_ptr<TrackMeter> meter;
    std::unique_ptr<AutoHelper> autoHelper;

    AutomationPath* autoPath;

    std::unique_ptr<TrackRenameWindow> renameWindow;

    std::unique_ptr<TrackPluginMenu> pluginMenu;
    std::unique_ptr<PluginWindow> pluginWindow;

    JUCE_LEAK_DETECTOR (Track)
};

#endif //TRACK_H_INCLUDED
