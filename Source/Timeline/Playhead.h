#ifndef PLAYHEAD_H_INCLUDED
#define PLAYHEAD_H_INCLUDED

#include "Processors/TrackBase.h"

class Playhead : public Component,
                 private Timer
{
public:
    Playhead (Array<TrackBase*>& trackProcs);

    void timerCallback() override;

    void paint (Graphics& g) override;

    void mouseMove (const MouseEvent& event) override;
    void mouseDrag (const MouseEvent& event) override;
    void mouseUp (const MouseEvent& event) override;

    void addProc (TrackBase* newProc) { procs.add(newProc); }
    void deleteProc (int index) { procs.remove (index); }

    class Listener
    {
    public:
        virtual ~Listener() {}
        virtual void playheadMoving (int64 /*posSamples*/) {}
    };

    void addListener (Listener* listener) { listeners.add (listener); }
    void removeListener (Listener* listener) { listeners.remove (listener); }

private:
    ListenerList<Listener> listeners;

    Array<TrackBase*> procs;

    int64 pos;

    bool mouseIsDragging = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Playhead)
};

#endif //PLAYHEAD_H_INCLUDED
