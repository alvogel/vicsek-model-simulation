#ifndef VICSEK_H_INCLUDED
#define VICSEK_H_INCLUDED

#include "Particle.h"
#include "SDL.h"
#include <vector>
#include <math.h>

class Vicsek
{
private:

public:
    unsigned short w;
    unsigned short h;
    float eta;
    float v;
    float radius;
    unsigned int n;
    std::vector<Particle> p;

    Vicsek();

    Vicsek(unsigned short width, unsigned short height, float v, float radius, unsigned int n_particles);

    Step();

    Draw(SDL_Renderer* r);

    void update_pos_vel();

    float calc_avg_norm_vel();

};


#endif // VICSEK_H_INCLUDED
