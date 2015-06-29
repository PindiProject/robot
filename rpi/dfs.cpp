#include "dfs.h"
#include <cstdlib>

const int DepthFirstSearch::INCREASING_SIZE = 50;

DepthFirstSearch::DepthFirstSearch():pathAvailable(false)
{

}

int
DepthFirstSearch::findHighestColumn(bool positive)
{
   typedef std::map<int, std::map<int, int> >::iterator matrix_iterator;
   typedef std::map<int, int>::iterator inner_iterator;
   int highestColumn = 0;

   for(matrix_iterator iterator = visitedPositions.begin(); iterator != visitedPositions.end(); iterator++) 
   {
       for(inner_iterator iterator2 = iterator->second.begin(); iterator2 != iterator->second.end(); iterator2++)
        {
            if(!positive)
            {
                if(iterator2->first<highestColumn && iterator2->second != 0)
                    highestColumn = iterator2->first;
            }
            else
            {   if(iterator2->first>highestColumn && iterator2->second != 0)
                    highestColumn = iterator2->first;
            }

        }
    }

   return abs(highestColumn);

}


void 
DepthFirstSearch::display_visited_positions()
{
   typedef std::map<int, std::map<int, int> >::iterator matrix_iterator;
   typedef std::map<int, int>::iterator inner_iterator;
   int highestColumnPos = findHighestColumn(true);
   int highestColumnNeg = findHighestColumn(false);
   std::cout<<highestColumnPos<<std::endl;

   for(matrix_iterator iterator = visitedPositions.begin(); iterator != visitedPositions.end(); iterator++) 
   {
        int x = iterator->first;
        int anterior = -2;
        int sum = 0;

       for(inner_iterator iterator2 = iterator->second.begin(); iterator2 != iterator->second.end(); iterator2++)
        {
            int y = iterator2->first;
            int temp;


            temp = (y<0)?highestColumnNeg:highestColumnPos;
            temp -= sum;

            if(anterior<=0 && y>=0)
                sum = 0;

            if(anterior<=0 && y>=0)
            {    
                temp = anterior+1;
                sum = 0;
            }

            int numSpaces = abs(temp - abs(y));
//           std::cout<<x<<" "<<y<<", ";
            if(!numSpaces)
                sum++;

            if(abs(abs(anterior) - abs(y)) == 1)
                numSpaces = 0;

            for(int i = 0; i<numSpaces;i++)
                std::cout<<"       ";

            if(visitedPositions[x][y] >= 0)
                std::cout<<" "<<visitedPositions[x][y]<<"     ";
            else
                std::cout<<visitedPositions[x][y]<<"     ";

            anterior = y;
                
        }

       std::cout<<std::endl;
    }

}

int
DepthFirstSearch::init(int x, int y)
{
    int cost = 0;
    Position pos = Position(x, y, Position::FRONT, cost);
    searchSpace.push(pos);

    visitedPositions[x][y] = 1;

    valid_neighbours[0] = Position(1, 0, Position::BACK, cost);
    valid_neighbours[1] = Position(0, -1, Position::LEFT, cost);
    valid_neighbours[2] = Position(0, 1, Position::RIGHT, cost);
    valid_neighbours[3] = Position(-1, 0, Position::FRONT, cost);

    return 0;
}


void
DepthFirstSearch::defineStateOrder(int direction)
{
    switch(direction)
    {
        case Position::FRONT:
            neighbours[0] = valid_neighbours[0];
            neighbours[1] = valid_neighbours[1];
            neighbours[2] = valid_neighbours[2];
            neighbours[3] = valid_neighbours[3];
            break;

         case Position::RIGHT:
            neighbours[0] = valid_neighbours[3];
            neighbours[1] = valid_neighbours[1];
            neighbours[2] = valid_neighbours[0];
            neighbours[3] = valid_neighbours[2];
            break;

         case Position::LEFT:
            neighbours[0] = valid_neighbours[2];
            neighbours[1] = valid_neighbours[0];
            neighbours[2] = valid_neighbours[3];
            neighbours[3] = valid_neighbours[1];
            break;

         case Position::BACK:
            neighbours[0] = valid_neighbours[3];
            neighbours[1] = valid_neighbours[2];
            neighbours[2] = valid_neighbours[1];
            neighbours[3] = valid_neighbours[0];
            break;

         default:
            break;
    }     
           
}

void
DepthFirstSearch::actualizeCost(Position p, int distance, bool reverse)
{
    int x = p.x;
    int y = p.y;
    int direction = p.direction;

    if(reverse)
        direction = reverseDirection(direction);

    std::vector<Position>& pos = positionEdges[std::make_pair(x, y)];

    for(unsigned int i =0; i<pos.size(); i++)
    {
        if(pos[i].direction == direction)
        {
            pos[i].cost = distance;
            break;
        }    
    }

}

int
DepthFirstSearch::getDistance()
{

    Position pos;

    if(pathAvailable)
        pos = actualPosition;
    else
        return 200;

    std::cout<<"Pos: "<<pos.x<<" "<<pos.y<<std::endl;
    std::vector<Position> edges = positionEdges[std::make_pair(pos.x, pos.y)];
    int direction = pos.direction;

   
    std::cout<<"Pos d: "<<direction<<std::endl;

    if(direction != -2)
    {
        for(unsigned int i =0; i<edges.size(); i++)
        {
            std::cout<<edges[i].x<<" "<<edges[i].y<<std::endl;
            std::cout<<edges[i].direction<<std::endl;
            if(direction == edges[i].direction)
                return edges[i].cost;
        }   

    }
    
    return 100;
}    

int
DepthFirstSearch::move(bool isObstacleAhead, int distance)
{
    if(pathAvailable)
    {
        int nextPosition = pathToLocation();

        if(nextPosition != -1)
            return nextPosition;
    }

    explore(isObstacleAhead, distance);

    if(!searchSpace.empty())
    {
        Position pos = nextPositionData();

        if(isNeighbour(pos.x, pos.y))
            return searchSpace.top().direction;
        else
        {
            createPathToLocation();
            pathAvailable = true;
            return pathToLocation();
        }
            
    }
    else
        return -2;
}

int
DepthFirstSearch::explore(bool isObstacleAhead, int distance)
{
    std::cout<<"EXPLORE"<<std::endl;

    if(searchSpace.empty())
        return -2;

    Position pos = searchSpace.top();
    searchSpace.pop();

    actualizeCost(actualPosition, distance, false);
    lastPosition = actualPosition;
    actualPosition = pos;

    int x = pos.x;
    int y = pos.y;
    int direction = pos.direction;
    int obstacle = 0;
    //visitedPositions[x][y] = 1;

    defineStateOrder(direction);

    if(!isObstacleAhead)
    {
       visitedPositions[x][y] = -1;
       obstacle = -1;
    }

    for(int i =0; i<4; i++)
    {
        int tempx = x;
        int tempy = y;

        tempx += neighbours[i].x;
        tempy += neighbours[i].y;
        direction = neighbours[i].direction;
       
        //if(tempx<0 || tempy<0)
        //    continue;

        //std::cout<<tempx<<" "<<tempy<<std::endl;
        Position p = Position(tempx, tempy, direction, 200);
        positionEdges[std::make_pair(x, y)].push_back(p);
        
        if(visitedPositions[tempx][tempy] == 1)
            continue;
   
        if(!obstacle)
        {
            visitedPositions[tempx][tempy] = (visitedPositions[tempx][tempy] == -1?-1:1);
            searchSpace.push(p);
        }    
    }    
 
    actualizeCost(actualPosition, distance, true);
    return obstacle;

}

bool
DepthFirstSearch::isNeighbour(int x, int y)
{

    for(int i =0;i<4; i++)
    {
        int tempx = valid_neighbours[i].x + actualPosition.x;
        int tempy = valid_neighbours[i].y + actualPosition.y;

//        std::cout<<tempx<<" "<<tempy<<std::endl;

        if(x == tempx && y == tempy)
            return true;
    }

    return false;
}

int
DepthFirstSearch::reverseDirection(int d)
{
    int direction = 0;
    std::cout<< "Direction: "<<d<<std::endl;

    switch(d)
    {
        case Position::BACK:
            direction = Position::FRONT;
            break;
        
        case Position::LEFT:
            direction = Position::RIGHT;
            break;

        case Position::RIGHT:
            direction = Position::LEFT;
            break;

        case Position::FRONT:
            direction = Position::BACK;
            break;
        
        default:
            return -1;
    }

    return direction;
}    

void
DepthFirstSearch::createPathToLocation()
{

    Position targetPos = nextPositionData();
    std::cout<<"PATH TO LOCATION"<<std::endl;
    std::cout<<"actual position: \nX: "<<actualPosition.x<<"\nY: "<<actualPosition.y<<std::endl;
    std::cout<<"\ntarget position: \nX: "<<targetPos.x<<"\nY: "<<targetPos.y<<std::endl;

    std::priority_queue<Position, std::vector<Position> , PositionComparison > space((PositionComparison(targetPos)));
    std::map<int, std::map<int, int> > visited_nodes;
    PathNode* lastPos;
    PathNode* pastPos = NULL;
    std::vector<PathNode*> closed;

    int find = 0;

    actualPosition.cost = 0;
    space.push(actualPosition);

    while(!space.empty())
    {
        Position pos = space.top();
        space.pop();

        if(pos.x == targetPos.x && pos.y == targetPos.y)
        {
            find = 1;
            break;
        }

        std::vector<Position> positions = positionEdges[std::make_pair(pos.x, pos.y)];
        //std::cout<<"Size: "<<positions.size()<<std::endl;
        //std::cout<<"Node: "<<pos.x<<" "<<pos.y<<" "<<pos.cost<<std::endl;
        PathNode *parent = new PathNode(pos.x, pos.y, pos.direction, pos.cost);
        parent->parent = pastPos; 

        for(unsigned int i =0; i<positions.size(); i++)
        {
            int x = positions[i].x;
            int y = positions[i].y;
            int direction = positions[i].direction;
            int cost = positions[i].cost+pos.cost;

            //std::cout<<x<<" "<<y<<" "<<cost<<std::endl;
            if(!visited_nodes[x][y] && visitedPositions[x][y] != -1)
            {
                positions[i].cost += pos.cost;
                space.push(positions[i]);
                visited_nodes[x][y] = 1;
                
            }
            
            PathNode *p = new PathNode(x, y, direction, cost);
            p->parent = parent;
                
            closed.push_back(p);

            if(x == targetPos.x && y == targetPos.y)
                lastPos = p;

            pastPos = parent;
        }
    }

    if(find)
    {
        PathNode* temp = lastPos;
        std::cout<<"PATH FIND"<<std::endl;

        while(true)
        {
            int x = temp->x;
            int y = temp->y;
            int direction = temp->direction;
            int cost = temp->cost;

            //direction = reverseDirection(direction);
            Position pos = Position(x, y, direction, cost);
            path.push(pos);
            temp = temp->parent; 

           std::cout<<pos.x<<" "<<pos.y<<std::endl;
           // std::cout<<temp->direction<<std::endl;
           std::cout<<pos.direction<<std::endl;
    
            if(isNeighbour(pos.x, pos.y))
                break;

        }
    }

    for(unsigned int i =0; i<closed.size();i++)
        delete closed[i];   

}

int
DepthFirstSearch::pathToLocation()
{
    
    if(path.empty())
    {
        pathAvailable = false;
        return -1;
    }
    else
    {
        Position pos = path.top();
        path.pop();
        lastPosition = actualPosition;
        actualPosition = pos;
        std::cout<<pos.x<<" "<<pos.y<<std::endl;
        std::cout<<pos.direction<<std::endl;
        return pos.direction;
    }    

}

int
DepthFirstSearch::nextPosition()
{
    //std::cout<<"\n\n"<<(isNeighbour() == true?"true":"false")<<std::endl;

    if(!searchSpace.empty())
    {
        if(pathAvailable)
            return pathToLocation();


        Position pos = nextPositionData();

        if(isNeighbour(pos.x, pos.y))
        {
            return searchSpace.top().direction;
        }
        else
        {
            createPathToLocation();
            return pathToLocation();
        }
            
    }
    else
        return -1;
}    

Position
DepthFirstSearch::nextPositionData()
{
    if(pathAvailable)
        return actualPosition;
    else if(!searchSpace.empty())
        return searchSpace.top();
    else
        return Position(0, 0,Position::FRONT, 0);
}    

int
DepthFirstSearch::numSearchSpace()
{
    return searchSpace.size();
}

