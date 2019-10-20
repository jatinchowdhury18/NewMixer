#include "AutomationPath.h"
#include "MainComponent.h"

namespace
{
    constexpr float pathThickness = 2.0f;
    constexpr float dashes[] = { 5.0f, 10.0f };
    constexpr float center = (float) TrackConstants::width / 2.0f;
}

AutomationPath::AutomationPath (Track* track) :
    track (track)
{
    setBoundsRelative (0.0f, 0.0f, 1.0f, 1.0);
    setInterceptsMouseClicks (false, false);
}

void AutomationPath::paint (Graphics& g)
{
    auto* ah = track->getAutoHelper();

    if (ah->getPoints().getNumChildren() == 0)
        return;

    const auto* parent = getParentComponent();
    
    g.setColour (track->getColour());
    
    auto point1 = ah->getPoints().getChild (0);
    auto lastX = (float) (ah->getPointX (point1) * parent->getWidth()) + center;
    auto lastY = (float) (ah->getPointY (point1) * parent->getHeight()) + center;

    for (int i = 1; i < ah->getPoints().getNumChildren(); i++)
    {
        auto point = ah->getPoints().getChild (i);
        const auto x = (float) (ah->getPointX (point) * parent->getWidth())  + center;
        const auto y = (float) (ah->getPointY (point) * parent->getHeight()) + center;

        //const auto thick = 5.0f * (point->diameter / TrackConstants::maxDiameter);

        Line <float> line (lastX, lastY, x, y);
        g.drawDashedLine (line, dashes, 2, pathThickness);

        lastX = x;
        lastY = y;
    }
}

void AutomationPath::resized()
{

}
