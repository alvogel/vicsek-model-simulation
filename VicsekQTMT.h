#ifndef VICSEKQTMT_H_INCLUDED
#define VICSEKQTMT_H_INCLUDED

#include "Vicsek.h"
#include "QuadTree.h"

class VicsekQTMT : public Vicsek
{
private:


public:

    //QuadTree qt;
    QuadTree qt1,qt2,qt3,qt4,qt5,qt6,qt7,qt8;

    std::vector<Particle> p_temp;

    VicsekQTMT();

    VicsekQTMT(unsigned short width, unsigned short height, float v,  float radius, unsigned int n_particles) : Vicsek(width, height, v, radius, n_particles)
    {

    }

    Start();

    void Tree(QuadTree& quad);

    void Chunk(unsigned int from, unsigned int to);

    Step();

    Draw(SDL_Renderer* r);

};

#endif // VICSEKQTMT_H_INCLUDED
