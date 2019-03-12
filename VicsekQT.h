#ifndef VICSEKQT_H_INCLUDED
#define VICSEKQT_H_INCLUDED

#include "Vicsek.h"
#include "QuadTree.h"

class VicsekQT : public Vicsek
{
private:


public:
    QuadTree qt;

    std::vector<Particle*> neighbour_points;

    VicsekQT();

    VicsekQT(int width, int height, float v, float radius, unsigned int n_particles) : Vicsek(width, height, v, radius, n_particles)
    {
        this->neighbour_points.reserve(10000);

    }

    Step();

    Draw(SDL_Renderer* r);
};

#endif // VICSEKQT_H_INCLUDED
