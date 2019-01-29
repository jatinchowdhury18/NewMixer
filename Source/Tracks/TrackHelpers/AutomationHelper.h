#ifndef AUTOMATIONHELPER_H_INCLUDED
#define AUTOMATIONHELPER_H_INCLUDED

#include "JuceHeader.h"

class AutoHelper
{
public:
    AutoHelper() {}

    void addAutoPoint (int x, int y, float diameter);
    void getPoint (int& x, int& y, float& diameter);

    void setRecordingStatus();
    void throwAway();
    void arm() { armedForRecording = ! armedForRecording; }
    void rewind() {curPoint = 0; }

    bool isRecording() { return recording; }
    bool isRecorded() { return automationRecorded; }
    bool armed() { return armedForRecording; }

private:
    struct AutoPoint
    {
        AutoPoint() {}
        AutoPoint (int x, int y, float diameter) :
            x (x),
            y (y),
            diameter (diameter)
        {}

        int x = 0;
        int y = 0;
        float diameter = 0;
    };

    OwnedArray<AutoPoint> points;

    int numPoints = 0;
    int curPoint = 0;

    bool recording = false;
    bool armedForRecording = false;
    bool automationRecorded = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AutoHelper)
};

#endif //AUTOMATIONHELPER_H_INCLUDED
