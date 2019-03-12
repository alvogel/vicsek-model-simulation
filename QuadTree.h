#ifndef QUADTREE_H_INCLUDED
#define QUADTREE_H_INCLUDED

#include "Rectangle.h"
#include "Particle.h"
#include <vector>
#include "SDL.h"

class QuadTree
{
private:

    Rectangle boundary;
    bool splitted;
    std::vector<Particle*> p;
    unsigned short capacity;
    unsigned short mul;


public:

    unsigned short deleted;

    QuadTree *nw;
    QuadTree *ne;
    QuadTree *sw;
    QuadTree *se;

    ~QuadTree();

    QuadTree();

    QuadTree(Rectangle b, unsigned short cap, unsigned short mul);

    void clear();

    void Draw(SDL_Renderer* r);

    unsigned short size();

    unsigned short nodes();

    bool query(Rectangle& r, std::vector<Particle*>& rp);

    bool insertPoint(Particle* ip);

};


#endif // QUADTREE_H_INCLUDED
