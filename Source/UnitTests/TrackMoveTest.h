#ifndef TRACKMOVETEST_H_INCLUDED
#define TRACKMOVETEST_H_INCLUDED

class MoveTest : public UnitTest
{
public:
    MoveTest() : UnitTest ("Move Tracks") {}

    int randInt()
    {
        Random r;
        r.setSeedRandomly();

        return (r.nextInt() % 1000) - 200;
    }

    void checkPosition()
    {
        Point<int> center = Point<int> (track->getX() + TrackConstants::width / 2,
                                        track->getY() + TrackConstants::width / 2);

        expect (0 <= center.x && center.x <= track->getParentWidth(),
                "X coordinate out of range: " + String (center.x));

        expect (0 <= center.y && center.y <= track->getParentHeight(),
                "Y coordinate out of range: " + String (center.x));

        expect (TrackConstants::minDiameter <= track->getDiameter() && track->getDiameter() <= TrackConstants::maxDiameter,
                "Track diameter out of range: " + String (track->getDiameter()));
    }

    void runTest() override
    {
        beginTest ("Move");

        for (int i = 0; i < 5000; i++)
        {
            TrackActionHelper::setPositionConstrained (track, Point<int> (randInt(), randInt()));
            TrackActionHelper::setSizeConstrained (track, track->getDiameter(), (float) randInt());

            checkPosition();
        }
    }

    void initialise() override
    {
        track = new Track (100, 0, false, "Test", "Tst", Colours::white);
        track->initialise (100, 100, 0);
    }

    void shutdown() override 
    {
        delete track;
    }

private:
    Track* track;
};

#endif //TRACKMOVETEST_H_INCLUDED
