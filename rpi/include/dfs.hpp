#ifndef _DEPTH_FIRST_SEARCH_HPP_
#define _DEPTH_FIRST_SEARCH_HPP_

#include <iostream>
#include <stack>
#include <vector>
#include <queue>
#include <map>
#include <utility>

#include "position.hpp"
#include "pathNode.hpp"

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

        class PositionComparison
        {
            public:
                
                PositionComparison(Position);
                Position targetPosition;
                int calculateHeuristic(const int&, const int&) const;
                bool operator() (const Position&, const Position&) const;
        };        

        std::stack<Position> searchSpace;
        std::stack<Position> path;
        std::vector<std::vector<int> > visitedPositions;
        std::map<std::pair<int, int>, std::vector<Position> > positionEdges;
        Position neighbours[4];
        Position valid_neighbours[4];
        Position actualPosition;
        Position lastPosition;
        bool pathAvailable;

};

#endif
