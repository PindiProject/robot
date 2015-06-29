#ifndef POSITION_COMPARISON_H
#define POSITION_COMPARISON_H

#include "position.h"
#include <cstdlib>

class PositionComparison
{
	
	public:
                
        PositionComparison(Position);
        Position targetPosition;
        int calculateHeuristic(const int&, const int&) const;
        bool operator() (const Position&, const Position&) const;
    

};

#endif
