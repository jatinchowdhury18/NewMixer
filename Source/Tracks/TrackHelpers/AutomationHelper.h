#ifndef AUTOMATIONHELPER_H_INCLUDED
#define AUTOMATIONHELPER_H_INCLUDED

#include "JuceHeader.h"

class AutoHelper
{
public:
    enum
    {
        timerInterval = 10,
    };

    AutoHelper (Component* parent);
    ~AutoHelper();

    void recordAutoPoint (float x, float y, float diameter, int64 sample);
    void addAutoPoint (float x, float y, float diameter, int64 sample);
    void getPoint (float& x, float& y, float& diameter, int64 sample);
    ValueTree& getPoints() { return autoPointsValueTree; }

    void setRecordingStatus();
    void throwAway();
    void arm() { armedForRecording = ! armedForRecording; }
    void rewind() {curPoint = 0; }

    bool isRecording() { return recording; }
    void setRecorded (bool recorded) { automationRecorded = recorded; }
    bool isRecorded() { return automationRecorded; }
    bool armed() { return armedForRecording; }
    
    float getPointX (const ValueTree& point) const noexcept;
    float getPointY (const ValueTree& point) const noexcept;
    float getPointDiameter (const ValueTree& point) const noexcept;
    int64 getPointSample (const ValueTree& point) const noexcept;
    var getPointValue (const ValueTree& point, Identifier id) const noexcept;

private:
    bool checkIsSamePoint (const ValueTree& originalPoint, const ValueTree&newPoint);
    UndoManager* getUndoManager();

    ValueTree autoPointsValueTree;
    int curPoint = 0;

    Component* parent;

    bool recording = false;
    bool armedForRecording = false;
    bool automationRecorded = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AutoHelper)
};

#endif //AUTOMATIONHELPER_H_INCLUDED
