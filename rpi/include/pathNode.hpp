#ifndef _PATH_NODE_HPP_
#define _PATH_NODE_HPP_

class PathNode
{
    public:

        int x;
        int y;
        int direction;
        int cost;
        PathNode *parent;

        PathNode(int, int, int, int);
};        

#endif
