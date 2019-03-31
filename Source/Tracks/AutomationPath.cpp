#include "AutomationPath.h"
#include "MainComponent.h"

AutomationPath::AutomationPath (Track* track) :
    track (track)
{
    setBoundsRelative (0.0f, 0.0f, 1.0f, 1.0);
    setInterceptsMouseClicks (false, false);
}

void AutomationPath::paint (Graphics& g)
{
    auto* ah = track->getAutoHelper();

    if (ah->getPoints().isEmpty())
        return;

    const auto* parent = getParentComponent();
    
    g.setColour (track->getColour());
    const auto center = (float) TrackConstants::width / 2.0f;
    
    auto point1 = ah->getPoints()[0];
    auto lastX = (float) (point1->x * parent->getWidth()) + center;
    auto lastY = (float) (point1->y * parent->getHeight()) + center;

    float dashes[] = { 5.0f, 10.0f };

    for (int i = 1; i < ah->getPoints().size(); i++)
    {
        auto point = ah->getPoints()[i];
        const auto x = (float) (point->x * parent->getWidth())  + center;
        const auto y = (float) (point->y * parent->getHeight()) + center;

        //const auto thick = 5.0f * (point->diameter / TrackConstants::maxDiameter);

        Line <float> line (lastX, lastY, x, y);
        g.drawDashedLine (line, dashes, 2, 2.5);

        lastX = x;
        lastY = y;
    }
}

void AutomationPath::resized()
{

}
