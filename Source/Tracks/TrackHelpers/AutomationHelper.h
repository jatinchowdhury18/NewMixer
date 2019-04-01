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

    struct AutoPoint
    {
        AutoPoint() {}
        AutoPoint (float x, float y, float diameter, int64 sample) :
            x (x),
            y (y),
            diameter (diameter),
            sample (sample)
        {}

        float x = 0;
        float y = 0;
        float diameter = 0;
        int64 sample = 0;
    };

    AutoHelper() {}

    void addAutoPoint (float x, float y, float diameter, int64 sample);
    void getPoint (float& x, float& y, float& diameter, int64 sample);
    OwnedArray<AutoPoint>& getPoints() { return points; }

    void setRecordingStatus();
    void throwAway();
    void arm() { armedForRecording = ! armedForRecording; }
    void rewind() {curPoint = 0; }

    bool isRecording() { return recording; }
    void setRecorded (bool recorded) { automationRecorded = recorded; }
    bool isRecorded() { return automationRecorded; }
    bool armed() { return armedForRecording; }

private:
    OwnedArray<AutoPoint> points;
    int curPoint = 0;
    int numPoints = 0;

    bool recording = false;
    bool armedForRecording = false;
    bool automationRecorded = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AutoHelper)
};

#endif //AUTOMATIONHELPER_H_INCLUDED
