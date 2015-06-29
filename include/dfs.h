#ifndef DEPTH_FIRST_SEARCH
#define DEPTH_FIRST_SEARCH

#include <iostream>
#include "position.h"
#include "pathNode.h"
#include "positionComparison.h"
#include <stack>
#include <vector>
#include <queue>
#include <map>
#include <utility>


class DepthFirstSearch
{

    public:

        static const int INCREASING_SIZE;

        DepthFirstSearch();
        int init(int, int);
        void display_visited_positions();
        int explore(bool, int);
        int move(bool, int);
        int getDistance();

        int numSearchSpace();

        int nextPosition();
        Position nextPositionData();

    private:

        void defineStateOrder(int);
        void createPathToLocation();
        bool isNeighbour(int, int);
        int pathToLocation();
        void actualizeCost(Position, int, bool);
        int reverseDirection(int);
        int findHighestColumn(bool);

        std::stack<Position> searchSpace;
        std::stack<Position> path;
        std::map<int, std::map<int, int> > visitedPositions;
        std::map<std::pair<int, int>, std::vector<Position> > positionEdges;
        Position neighbours[4];
        Position valid_neighbours[4];
        Position actualPosition;
        Position lastPosition;
        bool pathAvailable;

};


#endif
