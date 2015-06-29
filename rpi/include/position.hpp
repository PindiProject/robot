#ifndef _POSITION_HPP_
#define _POSITION_HPP_

#include "def.hpp"

class Position
{
    public:

        static enum Directions
        {
            FRONT = CMD_MOVE_FORWARD,
            BACK,
            RIGHT,
            LEFT
        }possible_directions;

        Position(int, int, int, int);
        Position(){};

        int x;
        int y;
        int direction;
        int cost;

};


#endif
