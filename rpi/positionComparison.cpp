#include "positionComparison.h"


PositionComparison::PositionComparison(Position _targetPosition)
{
    targetPosition = _targetPosition;
}

int 
PositionComparison::calculateHeuristic(const int& row, const int& column) const
{
    int distance = 1;

    int dx = abs(row - targetPosition.x);
    int dy = abs(column - targetPosition.y);

    return distance * (dx+dy);
}    


bool
PositionComparison::operator() (const Position& p1, const Position& p2) const
{

    int relativeCost1 = p1.cost + calculateHeuristic(p1.x, p1.y);
    int relativeCost2 = p2.cost + calculateHeuristic(p2.x, p2.y);

    if(p1.x == targetPosition.x && p1.y == targetPosition.y)
        return false;
    else if(p2.x == targetPosition.x && p2.y == targetPosition.y)
        return true;
    else if(relativeCost1> relativeCost2)
        return true;
    else
        return false;

}
