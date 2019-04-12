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
    QuadTree *parent;


public:

    unsigned short deleted;

    QuadTree *nw;
    QuadTree *ne;
    QuadTree *sw;
    QuadTree *se;

    // Destructor
    ~QuadTree();

    // Constructor
    QuadTree();

    // Constructor
    QuadTree(QuadTree *parent, Rectangle b, unsigned short cap, unsigned short mul);

    void reserve(int depth);

    void clear();

    void empty();

    void cleanup();

    void update();

    void Draw(SDL_Renderer* r);

    unsigned short size();

    unsigned short nodes();

    bool queryAll(std::vector<Particle*>& rp);

    bool query(Rectangle& r, std::vector<Particle*>& rp);

    bool insertPoint(Particle* ip);

    void split();

};


#endif // QUADTREE_H_INCLUDED
